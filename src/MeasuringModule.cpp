#include "MeasuringModule.h"

MeasuringModule *MeasuringModule::_instance = nullptr;

MeasuringModule::MeasuringModule() {
    MeasuringModule::_instance = this;
}

MeasuringModule *MeasuringModule::instance() {
    return MeasuringModule::_instance;
}

const std::string MeasuringModule::name() {
    return "Measuring";
}

const std::string MeasuringModule::version() {
    return "0.1dev";
}

void MeasuringModule::setup() 
{
    // save default values from PA
    measurementSend = ParamAPP_SendMeasuredValues;
    measurementInterval = getTimeWithPattern(ParamAPP_MeasurementIntervalTime, ParamAPP_MeasurementIntervalBase);
    tempSensorPresent = ParamAPP_TempSensorPresent;
    shuntValue = ParamAPP_ShuntValue;
    maxcurrent = (shuntValue == 10) ? 8.0 : 16.0; // 0,010Ω = max.  8A, 0,005Ω = max. 16A

    // Init I2C connection and Lib
    if (initI2cConnection()) {
        logInfoP("Init messurment I2C connection sucessful");
    }

    // Debug
    logDebugP("Measuring send: %i", measurementSend);
    logDebugP("Measuring interval: %i", measurementInterval);
    logDebugP("Shunt value: %f", shuntValue / 1000);
    logDebugP("Max current in A: %f", maxcurrent);
    logDebugP("Temp sensor present: %i", tempSensorPresent);          
}

void MeasuringModule::setup1() {
    // Nothing to do on Core 1
}

void MeasuringModule::loop() {
    // do nothing when not parameterized
    if (!knx.configured())
        return;

    if (measurementSend == 1) {
        if (delayCheck(_lastMeasurementSend, measurementInterval)) {
            getSingleMeasurement();
            _lastMeasurementSend = millis();
        }
    }
    //check if I2C connection possible, if not reset and init the pwm
    if (delayCheck(_timerCheckI2cConnection, 45000)) {
        checkI2cConnection();
        _timerCheckI2cConnection = millis();
    }
    //check ina226 overflow every 30 seconds
    if (delayCheck(_timerCheckOverflow, 30000)) {
        checkI2cConnection();
        _timerCheckOverflow = millis();
    }     
}

void MeasuringModule::loop1() {
    // Nothing to do on Core 1
}

void MeasuringModule::getSingleMeasurement()
{
    busVoltage_V = round(_ina226.getBusVoltage_V() * 10) / 10;  // rounded to one decimal places
    current_A = round(_ina226.getCurrent_A() * 100) / 100;      // rounded to two decimal places
    power_W = round(_ina226.getBusPower() / 1000 * 100) / 100;  // rounded to two decimal places

    KoAPP_VoltageV.value(busVoltage_V, DPT_Value_Electric_Potential);
    KoAPP_CurrentA.value(current_A, DPT_Value_Electric_Current);
    KoAPP_PowerW.value(power_W, DPT_Value_Power);

    // run Temp Measurment if sensor present
    if (tempSensorPresent) {
        temperatur_C = _tmp100.getTemperature();
        KoAPP_TempC.value(temperatur_C, DPT_Value_Common_Temperature);
    }
}

void MeasuringModule::getOverflowValue()
{
    _ina226.readAndClearFlags();
    if (_ina226.overflow) {
        // power off all LED channels and reboot
        logErrorP("Overflow %.2f", maxcurrent);
        openknx.console.writeDiagenoseKo("OF");
        delay(20);
        openknx.restart();
    }
}

void MeasuringModule::showHelp()
{
    openknx.logger.color(CONSOLE_HEADLINE_COLOR);
    openknx.logger.log("======================== Measuring Module ======================================");
    openknx.logger.color(0);
    openknx.console.printHelpLine("temp", "Display the internal temperature in °C");
    openknx.console.printHelpLine("voltage", "Display the current voltage in volt");
    openknx.console.printHelpLine("current", "Display the current current in ampere");
    openknx.console.printHelpLine("power", "Display the current power in watt");
}

bool MeasuringModule::processCommand(const std::string cmd, bool diagnoseKo) 
{
    if (cmd == "temp") {
        getSingleMeasurement();
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("T %.2fC", temperatur_C); }
        logInfoP("Temperatur: %.2f °C", temperatur_C);
        return true;
    } else if (cmd == "voltage") {
        getSingleMeasurement();
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("V %.1fV", busVoltage_V); }
        logInfoP("Voltage: %.1f V", busVoltage_V);
        return true;
    } else if (cmd == "current") {
        getSingleMeasurement();
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("C %.2fA", current_A); }
        logInfoP("Current: %.2f A", current_A);
        return true;
    } else if (cmd == "power") {
        getSingleMeasurement();
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("P %.2fW", power_W); }
        logInfoP("Power: %.2f W",  power_W);
        return true;
    }
    return false;
}

bool MeasuringModule::initI2cConnection() {
    // Init TMP100 Lib if present
    if (tempSensorPresent) {
        _tmp100 = TMP100_WE(&Wire1, I2C_TMP100_DEVICE_ADDRESS);
        // Call dependend init for sensor
        if (!_tmp100.init()) {
            logErrorP("ERROR: initialization for TMP100 failed...");
            doResetI2c = true;
            return false;
        }
        delay(10);
        // Set default values for sensor
        _tmp100.setResolution(RES025);

        doResetI2c = false;
    }
    
    // Init INA226 Lib
    _ina226 = INA226_WE(&Wire1, I2C_INA226_DEVICE_ADDRESS);
    // Call dependend init for sensor
    if (!_ina226.init()) {
        logErrorP("ERROR: initialization for INA226 failed...");
        doResetI2c = true;
        return false;
    }
    delay(10);
    // Set default values for sensor
    _ina226.setAverage(AVERAGE_1);                              // Anzahl Einzelmessungen für die Shunt- und Busspannungskonversion
    _ina226.setConversionTime(CONV_TIME_1100);                  // Einstellung der A/D-Wandlungszeit für die Shunt- und Busspannung
    _ina226.setMeasureMode(CONTINUOUS);                         // Messmodus
    _ina226.setResistorRange(shuntValue / 1000, maxcurrent);    // Resistor 0.01 Ohm, Max current 8.0 A, 0,005 Ohm, Max current 16.0 A
    _ina226.startSingleMeasurementNoWait();                     // Don't wait for conversion to complete     

    doResetI2c = false;
    return true;
}

bool MeasuringModule::checkI2cConnection() {
    if (doResetI2c) { 
        return initI2cConnection();
    }
    byte result1 = 0;
    byte result2;
    if (tempSensorPresent) {
        Wire1.beginTransmission(I2C_TMP100_DEVICE_ADDRESS);
        result1 = Wire1.endTransmission();       //  0 : Success  1 : Data too long  2 : NACK on transmit of address  3 : NACK on transmit of data  4 : Other error  5 : Timeout
    }
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
    result2 = Wire1.endTransmission();           //  0 : Success  1 : Data too long  2 : NACK on transmit of address  3 : NACK on transmit of data  4 : Other error  5 : Timeout
    if (result1 == 0 && result2 == 0) {
        return true;
    } else {
        if (result1 != 0) {
            logErrorP("TMP100 not available via I2C %d", result1);
            openknx.console.writeDiagenoseKo("ER I2C TMP");
        }
        if (result2 != 0) {
            logErrorP("INA226 not available via I2C %d", result2);
            openknx.console.writeDiagenoseKo("ER I2C INA");
        }
        doResetI2c = true;
        return false;
    }
    return false;
}

uint32_t MeasuringModule::getTimeWithPattern(uint16_t time, uint8_t base) {
    switch (base)
    {
        case TIMEBASE_TENTH_SECONDS:
            return time * 100;
            break;
        case TIMEBASE_SECONDS:
            return time * 1000;
            break;
        case TIMEBASE_MINUTES:
            return time * 60000;
            break;
        case TIMEBASE_HOURS:
            // for hour, we can only cover 1193 hours in milliseconds, we allow just 1000 here
            if (time > 1000) 
                time = 1000;
            return time * 3600000;
            break;
        default:
            return 0;
            break;
    }
}

MeasuringModule openknxMeasuringModule;