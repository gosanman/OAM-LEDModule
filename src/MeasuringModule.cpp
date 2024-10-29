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

    // Alarm definitions
    if (tempSensorPresent) {
        checkTemp = ParamAPP_AlarmUseOverTemp;
        if (checkTemp) {
            overTemp = ParamAPP_AlarmOverTemp;
        }
    }
    checkVoltage = ParamAPP_AlarmUseVoltage;
    if (checkVoltage) {
        overVoltage = ParamAPP_AlarmOverVoltage;
        underVoltage = ParamAPP_AlarmUnderVoltage;
    }
    checkCurrent = ParamAPP_AlarmUseOverCurrent;
    if (checkCurrent) {
        overCurrent = ParamAPP_AlarmOverCurrent;
    }
 
    lastUpdateTime = millis(); 

    // Init TMP100 Lib if present
    if (tempSensorPresent) {
        // Init I2C connection and Lib TMP100
        _tmp100 = TMP100_WE(&Wire1, I2C_TMP100_DEVICE_ADDRESS);
        initI2cConnectionTemp();
    }
    
    // Init I2C connection and Lib INA226
    _ina226 = INA226_WE(&Wire1, I2C_INA226_DEVICE_ADDRESS);
    initI2cConnectionIna();
    
    // Debug
    logDebugP("Measuring send: %i", measurementSend);
    logDebugP("Measuring interval: %i", measurementInterval);
    logDebugP("Shunt value in Ohm: %.3f", shuntValue / 1000);
    logDebugP("Max current in A: %.2f", maxcurrent);
    logDebugP("Temp sensor present: %i", tempSensorPresent);
    logDebugP("Monitor Temp: %i", checkTemp);
    if (tempSensorPresent) {
        if (checkTemp) { 
            logDebugP("Max Temp: %.2f", overTemp); 
        }
    }    
    logDebugP("Monitor Voltage: %i", checkVoltage);
    if (checkVoltage) { 
        logDebugP("Over Voltage: %.2f", overVoltage);
        logDebugP("Under Voltage: %.2f", underVoltage); 
    }   
    logDebugP("Monitor Current: %i", checkCurrent);
        if (checkCurrent) { 
        logDebugP("Max Current: %.2f", overCurrent); 
    }                 
}

void MeasuringModule::setup1() {
    // Nothing to do on Core 1
}

void MeasuringModule::loop() {
    // do nothing when not parameterized
    if (!knx.configured())
        return;
    
    // always run measurment for alarm features
    if (delayCheck(_lastMeasurementGet, 500)) {
        getSingleMeasurement();
        checkAlarmDefinitions();
        _lastMeasurementGet = millis();
    }
    // only run if measurment send allow
    if (measurementSend) {
        if (delayCheck(_lastMeasurementSend, measurementInterval)) {
            sendSingleMeasurement();
            _lastMeasurementSend = millis();
        }
    }
    // check ina226 overflow every 30 seconds
    if (delayCheck(_timerCheckOverflow, 30000)) {
        getOverflowValue();
        _timerCheckOverflow = millis();
    }  
    // check if I2C connection possible every 45 seconds, if not reset and init the connection
    if (delayCheck(_timerCheckI2cConnection, 45000)) {
        if (tempSensorPresent) { 
            checkI2cConnectionTemp();
        }
        checkI2cConnectionIna();
        _timerCheckI2cConnection = millis();
    }
}

void MeasuringModule::loop1() {
    // Nothing to do on Core 1
}

void MeasuringModule::getSingleMeasurement()
{
    busVoltage_V = _ina226.getBusVoltage_V();
    current_A = _ina226.getCurrent_A();
    power_mW = _ina226.getBusPower();
    power_W = power_mW / 1000.0;

    // calculate total energy usage
    currentTime = millis();
    elapsedTime_s = (currentTime - lastUpdateTime) / 1000.0;
    totalEnergy_Wh += (busVoltage_V * current_A * elapsedTime_s) / 3600.0;
    lastUpdateTime = currentTime;

    // run Temp Measurment if sensor present
    if (tempSensorPresent) {
        temperatur_C = _tmp100.getTemperature();
    }
}

void MeasuringModule::sendSingleMeasurement()
{
    KoAPP_VoltageV.value(round(busVoltage_V * 10) / 10, DPT_Value_Electric_Potential);  // rounded to one decimal places
    KoAPP_CurrentA.value(round(current_A * 100) / 100, DPT_Value_Electric_Current);     // rounded to two decimal places
    KoAPP_PowerW.value(round(power_mW / 1000 * 100) / 100, DPT_Value_Power);            // rounded to two decimal places
    KoAPP_ActivePowerWh.value(round(totalEnergy_Wh * 1000) / 1000, DPT_ActiveEnergy);   // rounded to three decimal places
    
    if (tempSensorPresent) {
        KoAPP_TempC.value(temperatur_C, DPT_Value_Common_Temperature);
    }
}

void MeasuringModule::getOverflowValue()
{
    _ina226.readAndClearFlags();
    if (_ina226.overflow) {
        // to power off all LED channels reboot
        logErrorP("Current over limit %.2f", current_A);
        openknx.console.writeDiagenoseKo("OF");
        //openknx.restart();
    }
}

void MeasuringModule::checkAlarmDefinitions() {
    if (tempSensorPresent && checkTemp) {
        checkAndTriggerAlarm(temperatur_C > overTemp, overTempTriggered, APP_KoAlarmOverTemp, "OVER TEMP");
    }
    if (checkVoltage) {
        checkAndTriggerAlarm(busVoltage_V > overVoltage, overVoltageTriggered, APP_KoAlarmOverVoltage, "OVER VOLT");
        checkAndTriggerAlarm(busVoltage_V < underVoltage, underVoltageTriggered, APP_KoAlarmUnderVoltage, "UNDER VOLT");
    }
    if (checkCurrent) {
        checkAndTriggerAlarm(current_A > overCurrent, overCurrentTriggered, APP_KoAlarmOverCurrent, "OVER CURRENT");
    }
}

void MeasuringModule::checkAndTriggerAlarm(bool condition, bool &triggeredFlag, uint16_t alarmKo, const String &messageDiagnoseKo) {
    if (condition) {
        if (!triggeredFlag) {
            knx.getGroupObject(alarmKo).value(true, DPT_Alarm);             // Trigger alarm
            openknx.console.writeDiagenoseKo(messageDiagnoseKo.c_str());    // Send alarm to diagnose ko
            triggeredFlag = true;                                           // Set flag
        }
    } else {
        if (triggeredFlag) {
            knx.getGroupObject(alarmKo).value(false, DPT_Alarm);    // Reset alarm
            triggeredFlag = false;                                  // Reset flag if status is normal
        }
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
    openknx.console.printHelpLine("energy", "Display the total energy in watt hour");
}

bool MeasuringModule::processCommand(const std::string cmd, bool diagnoseKo) 
{
    if (cmd == "temp") {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("T %.2fC", temperatur_C); }
        openknx.logger.logWithPrefixAndValues("Temperatur", "%.2f °C", temperatur_C);
        return true;
    } else if (cmd == "voltage") {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("V %.1fV", busVoltage_V); }
        openknx.logger.logWithPrefixAndValues("Voltage", "%.1f V", busVoltage_V);
        return true;
    } else if (cmd == "current") {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("C %.2fA", current_A); }
        openknx.logger.logWithPrefixAndValues("Current", "%.2f A", current_A);
        return true;
    } else if (cmd == "power") {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("P %.2fW", power_W); }
        openknx.logger.logWithPrefixAndValues("Power", "%.2f W", power_W);
        return true;
    } else if (cmd == "energy") {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("E %.2fWh", totalEnergy_Wh); }
        openknx.logger.logWithPrefixAndValues("Energy", "%.4f Wh", totalEnergy_Wh);
        return true;
    }else if (cmd == "ccenergy") {
        totalEnergy_Wh = 0.00;
        openknx.flash.save();
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("E cleared"); }
        openknx.logger.logWithPrefixAndValues("Energy", "Clear counter finish");        
        return true;
    }
    return false;
}

bool MeasuringModule::initI2cConnectionTemp() 
{
    // Call dependend init for temp100 sensor
    if (!_tmp100.init()) {
        logErrorP("ERROR: initialization for TMP100 failed...");
        doResetI2cTemp = true;
        return false;
    }
    // Set default values for sensor
    _tmp100.setResolution(RES025);
    logInfoP("Init messurment I2C connection for TEMP100 sucessful");
    doResetI2cTemp = false;
    return true;
}

bool MeasuringModule::initI2cConnectionIna() 
{
    // Call dependend init for ina226 sensor
    if (!_ina226.init()) {
        logErrorP("ERROR: initialization for INA226 failed...");
        doResetI2cIna = true;
        return false;
    }
    // Set default values for sensor
    _ina226.setAverage(AVERAGE_1);                              // Anzahl Einzelmessungen für die Shunt- und Busspannungskonversion
    _ina226.setConversionTime(CONV_TIME_1100);                  // Einstellung der A/D-Wandlungszeit für die Shunt- und Busspannung
    _ina226.setMeasureMode(CONTINUOUS);                         // Messmodus
    _ina226.setResistorRange(shuntValue / 1000, maxcurrent);    // Resistor 0.01 Ohm, Max current 8.0 A, 0,005 Ohm, Max current 16.0 A
    _ina226.setCorrectionFactor(0.95);                          // Correction factor = current delivered from calibrated equipment / current delivered by INA226
    _ina226.startSingleMeasurementNoWait();                     // Don't wait for conversion to complete     
    _ina226.enableAlertLatch();                                 // With enableAltertLatch(), the flag will have to be deleted with readAndClearFlags()

    /* Set the alert type and the limit
      * Mode *        * Description *           * limit unit *
    SHUNT_OVER     Shunt Voltage over limit          mV
    SHUNT_UNDER    Shunt Voltage under limit         mV
    CURRENT_OVER   Current over limit                mA
    CURRENT_UNDER  Current under limit               mA
    BUS_OVER       Bus Voltage over limit            V
    BUS_UNDER      Bus Voltage under limit           V
    POWER_OVER     Power over limit                  mW
    */
    _ina226.setAlertType(CURRENT_OVER, 4000);    // Testweise abgesichert auf 4.0 A
    logInfoP("Init messurment I2C connection INA226 sucessful");
    doResetI2cIna = false;
    return true;
}

bool MeasuringModule::checkI2cConnectionTemp() 
{
    if (doResetI2cTemp) { 
        return initI2cConnectionTemp();
    }
    Wire1.beginTransmission(I2C_TMP100_DEVICE_ADDRESS);
    byte resultTemp = Wire1.endTransmission();       //  0 : Success  1 : Data too long  2 : NACK on transmit of address  3 : NACK on transmit of data  4 : Other error  5 : Timeout
    if (resultTemp != 0) {
        logErrorP("TMP100 not available via I2C %d", resultTemp);
        openknx.console.writeDiagenoseKo("ER I2C TMP %d", resultTemp);
        doResetI2cTemp = true;
        return false;
    }
    return true;
}

bool MeasuringModule::checkI2cConnectionIna() 
{
    if (doResetI2cIna) { 
        return initI2cConnectionIna();
    }
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
    byte resultIna = Wire1.endTransmission();       //  0 : Success  1 : Data too long  2 : NACK on transmit of address  3 : NACK on transmit of data  4 : Other error  5 : Timeout
    if (resultIna != 0) {
        logErrorP("INA226 not available via I2C %d", resultIna);
        openknx.console.writeDiagenoseKo("ER I2C INA %d", resultIna);
        doResetI2cIna = true;
        return false;
    }
    return true;
}

void MeasuringModule::readFlash(const uint8_t *buffer, const uint16_t size)
{
    // first call - without data
    if (size == 0) return;

    uint8_t version = openknx.flash.readByte();
    if (version != 1) // version unknown
    {
        logErrorP("Wrong version of flash data (%i)", version);
        return;
    }

    float counter = openknx.flash.readFloat();
    logDebugP("Restore totalEnergy_Wh with value: %f", counter);
    totalEnergy_Wh = counter;
}

void MeasuringModule::writeFlash()
{
    openknx.flash.writeByte(1); // Version
    openknx.flash.writeFloat(totalEnergy_Wh);
}

uint16_t MeasuringModule::flashSize()
{
    // Version + Data (Channel * Inputs * (Dpt + Value))
    return 1 + 8;
}

uint32_t MeasuringModule::getTimeWithPattern(uint16_t time, uint8_t base) 
{
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