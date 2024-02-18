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
    measurementSend = ParamAPP_PT_SendMeasuredValues;
    measurementSelectInterval = ParamAPP_PT_MeasurementInterval;
    tempSensorPresent = ParamAPP_PT_TempSensorPresent;
    shuntValue = ParamAPP_PT_ShuntValue;
    maxcurrent = (shuntValue == 10) ? 8.0 : 16.0;

    // Debug
    logInfoP("Measuring send: %i", measurementSend);
    logInfoP("Measuring interval: %i", MeasurementInterval[measurementSelectInterval]);
    logInfoP("Shunt value: %f", shuntValue / 1000);
    logInfoP("Max current in A: %f", maxcurrent);
    logInfoP("Temp sensor present: %i", tempSensorPresent);

    // Init TMP100 Lib if present
    if (tempSensorPresent) {
        _tmp100 = TMP100_WE(&Wire1, I2C_TMP100_DEVICE_ADDRESS);
        // Call dependend init for sensor
        if (!_tmp100.init()) {
            logErrorP("ERROR: initialization for TMP100 failed...");
        }
        // Set default values for sensor
        _tmp100.setResolution(RES025);
    }
    
    delay(100);

    // Init INA226 Lib
    _ina226 = INA226_WE(&Wire1, I2C_INA226_DEVICE_ADDRESS);
    // Call dependend init for sensor
    if (!_ina226.init()) {
        logErrorP("ERROR: initialization for INA226 failed...");
    }
    // Set default values for sensor
    _ina226.setAverage(AVERAGE_128);                            // Anzahl Einzelmessungen für die Shunt- und Busspannungskonversion
    _ina226.setConversionTime(CONV_TIME_1100);                  // Einstellung der A/D-Wandlungszeit für die Shunt- und Busspannung
    _ina226.setMeasureMode(CONTINUOUS);                         // Messmodus
    _ina226.setResistorRange(shuntValue / 1000, maxcurrent);    // Resistor 0.01 Ohm, Max current 8.0 A, 0,005 Ohm, Max current 16.0 A
    _ina226.waitUntilConversionCompleted();                     
}

void MeasuringModule::loop() {
    // Nothing to do on Core 0
}

void MeasuringModule::loop1() {
    // do nothing when not parameterized
    if (!knx.configured())
        return;

    if (measurementSend == 1) {
        if (delayCheck(_lastMeasurementSend, MeasurementInterval[measurementSelectInterval])) {
            _lastMeasurementSend = millis();
            getSingleMeasurement();
        }
    }
}

void MeasuringModule::getSingleMeasurement()
{
    _ina226.readAndClearFlags();
    busVoltage_V = round(_ina226.getBusVoltage_V() * 10) / 10;  // rounded to one decimal places
    current_A = round(_ina226.getCurrent_A() * 100) / 100;      // rounded to two decimal places
    power_W = round(_ina226.getBusPower_W() * 100) / 100;       // rounded to two decimal places

    // logInfoP("BusVoltage: %f", busVoltage_V);
    // logInfoP("Current: %f", current_A);
    // logInfoP("Power: %f", power_W);

    KoAPP_KO_VoltageV.value(busVoltage_V, DPT_Value_Electric_Potential);
    KoAPP_KO_CurrentA.value(current_A, DPT_Value_Electric_Current);
    KoAPP_KO_PowerW.value(power_W, DPT_Value_Power);

    if (!_ina226.overflow) {
        // Noch zu definieren, evtl. KO für Überlast
        // logInfoP("Values OK - no overflow");
    } else {
        // Noch zu definieren, evtl. KO für Überlast und abschalten der Dim Funktion
        // logInfoP("Overflow! Choose higher current range");
    }

    // run Temp Measurment if sensor present
    if (tempSensorPresent) {
        temperatur_C = _tmp100.getTemperature();
        // logInfoP("Temperatur: %f", temperatur_C);
        KoAPP_KO_TempC.value(temperatur_C, DPT_Value_Common_Temperature);
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
    if (!diagnoseKo && (cmd == "temp")) {
        getSingleMeasurement();
        logInfoP("Temperatur: %f °C", temperatur_C);
        return true;
    } else if (!diagnoseKo && (cmd == "voltage")) {
        getSingleMeasurement();
        logInfoP("Voltage: %f V", busVoltage_V);
        return true;
    } else if (!diagnoseKo && (cmd == "current")) {
        getSingleMeasurement();
        logInfoP("Current: %f A", current_A);
        return true;
    } else if (!diagnoseKo && (cmd == "power")) {
        getSingleMeasurement();
        logInfoP("Power: %f W",  power_W);
        return true;
    }
    return false;
}

MeasuringModule openknxMeasuringModule;