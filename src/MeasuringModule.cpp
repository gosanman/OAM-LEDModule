#include "MeasuringModule.h"

MeasuringModule *MeasuringModule::_instance = nullptr;

MeasuringModule::MeasuringModule() 
    : _ina(&Wire1, I2C_INA22x_DEVICE_ADDRESS)
{
    MeasuringModule::_instance = this;
}

MeasuringModule *MeasuringModule::instance() {
    return MeasuringModule::_instance;
}

const std::string MeasuringModule::name() {
    return "Measuring";
}

const std::string MeasuringModule::version() {
    return "0.4.0";
}

void MeasuringModule::setup() 
{
    // save default values from PA
    measurementSend = ParamAPP_SendMeasuredValues;
    measurementInterval = LEDHelper::getTimeWithPattern(ParamAPP_MeasurementIntervalTime, ParamAPP_MeasurementIntervalBase);
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

    // Init INA I2C connection
    initI2cConnectionIna();
    
    // Debug - All in one line
    logDebugP("Send: %i, Int: %i sec | InaType: INA%i, Shunt: %.3f Ohm, MaxI: %.2f A | TempSens: %i | MonTemp: %i%s | MonV: %i%s, | MonI: %i%s",
              measurementSend, measurementInterval / 1000, _ina.getChipType(), shuntValue / 1000, maxcurrent, tempSensorPresent,
              checkTemp, (tempSensorPresent && checkTemp) ? (", MaxT: " + String(overTemp)).c_str() : "",
              checkVoltage, checkVoltage ? (", OverV: " + String(overVoltage) + ", UnderV: " + String(underVoltage)).c_str() : "",
              checkCurrent, checkCurrent ? (", MaxI: " + String(overCurrent)).c_str() : "");
}

void MeasuringModule::setup1() {

}

void MeasuringModule::loop() 
{
    // do nothing when not parameterized
    if (!knx.configured())
        return;
}

void MeasuringModule::loop1() {
    // do nothing when not parameterized
    if (!knx.configured())
        return;
    
    // always run measurment for alarm features
    if (delayCheck(_lastMeasurementGet, MEASUREMENT_ALARM_CHECK)) {
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
    // check always alerts to protect hardware
    if (delayCheck(_timerCheckOverflow, MEASUREMENT_INA_OVERFLOW)) {
        getAlertValues();
        _timerCheckOverflow = millis();
    }  
    // check if I2C connection possible, if not reset and init the connection
    if (delayCheck(_timerCheckI2cConnection, MEASUREMENT_CHECK_I2C)) {
        if (tempSensorPresent) { 
            checkI2cConnectionTemp();
        }
        checkI2cConnectionIna();
        _timerCheckI2cConnection = millis();
    }
}

void MeasuringModule::getSingleMeasurement()
{
    if (inaI2cConnection) {
        busVoltage_V = _ina.getVoltage();
        current_A = _ina.getCurrent();
        power_W = _ina.getPower();

        // calculate total energy usage
        if (_ina.getChipType() == INA226_TYPE) {
            currentTime = millis();
            elapsedTime_s = (currentTime - lastUpdateTime) / 1000.0;
            totalEnergy_Wh += (busVoltage_V * current_A * elapsedTime_s) / 3600.0;
            lastUpdateTime = currentTime;
        } else if (_ina.getChipType() == INA228_TYPE) {
            double energy = _ina.getEnergy(); // nur einmal lesen (Register akkumuliert kontinuierlich)
            totalEnergy_Wh += energy - lastEnergy_Wh;
            lastEnergy_Wh = energy;
        }
    }
    // run Temp Measurment if sensor present
    if (tempSensorPresent && tempI2cConnection) {
        temperatur_C = _tmp100.getTemperature();
    }
}

void MeasuringModule::sendSingleMeasurement()
{
    if (inaI2cConnection) {
        KoAPP_VoltageV.value(round(busVoltage_V * 10) / 10, DPT_Value_Electric_Potential);  // rounded to one decimal places
        KoAPP_CurrentA.value(round(current_A * 100) / 100, DPT_Value_Electric_Current);     // rounded to two decimal places
        KoAPP_PowerW.value(round(power_W * 100) / 100, DPT_Value_Power);                    // rounded to two decimal places
        KoAPP_ActivePowerWh.value(round(totalEnergy_Wh * 1000) / 1000, DPT_ActiveEnergy);   // rounded to three decimal places
    }
    if (tempSensorPresent && tempI2cConnection) {
        KoAPP_TempC.value(temperatur_C, DPT_Value_Common_Temperature);
    }
}

// Read alert flags from INA sensor and log any alerts.
// Runs periodically to check for alert conditions.
// Not configured via ETS, Hardwareimplementation to protect the hardware
void MeasuringModule::getAlertValues()
{
    if (!inaI2cConnection) 
        return;
    
    uint8_t flags = _ina.getAlertFlags();
    if (flags == 0) {
        openknxLEDModule.setPowerFault(false); // Entwarnung: kein Alert mehr -> Ausgänge freigeben
        return;
    }
    
    // Alle Alert-Typen tabellarisch prüfen (Bit -> Log-/Diagnosetext)
    static const struct { uint8_t bit; const char *logMsg; const char *diagnose; } alerts[] = {
        {0, "Alert: Power Over-Limit",                 "AL PWR OVR"},
        {1, "Alert: Bus Undervoltage",                 "AL BUS UND"},
        {2, "Alert: Bus Overvoltage",                  "AL BUS OVR"},
        {3, "Alert: Shunt Undervoltage / Overcurrent", "AL SHNT UND"},
        {4, "Alert: Shunt Overvoltage / Overcurrent",  "AL OVER CUR"},
        {5, "Alert: Temperature Overlimit",            "AL TEMP OVR"},
    };

    for (const auto &a : alerts) {
        if (flags & (1 << a.bit)) {
            logErrorP("%s", a.logMsg);
            openknx.console.writeDiagenoseKo("%s", a.diagnose);
        }
    }
    openknxLEDModule.savePower(); // einmal abschalten/latchen, sobald irgendein Alert ansteht
}

void MeasuringModule::checkAlarmDefinitions() {
    if (tempSensorPresent && checkTemp && tempI2cConnection) {
        checkAndTriggerAlarm(temperatur_C > overTemp, overTempTriggered, APP_KoAlarmOverTemp, "OVER TEMP");
    }
    if (checkVoltage && inaI2cConnection) {
        checkAndTriggerAlarm(busVoltage_V > overVoltage, overVoltageTriggered, APP_KoAlarmOverVoltage, "OVER VOLT");
        checkAndTriggerAlarm(busVoltage_V < underVoltage, underVoltageTriggered, APP_KoAlarmUnderVoltage, "UNDER VOLT");
    }
    if (checkCurrent && inaI2cConnection) {
        checkAndTriggerAlarm(current_A > overCurrent, overCurrentTriggered, APP_KoAlarmOverCurrent, "OVER CURRENT");
    }
}

void MeasuringModule::checkAndTriggerAlarm(bool condition, bool &triggeredFlag, uint16_t alarmKo, const String &messageDiagnoseKo) {
    if (condition) {
        if (!triggeredFlag) {
            knx.getGroupObject(alarmKo).value(true, DPT_Alarm);             // Trigger alarm
            openknx.console.writeDiagenoseKo(messageDiagnoseKo.c_str());    // Send alarm to diagnose ko
            triggeredFlag = true;                                           // Set flag
#ifdef INFO3_LED_PIN
            openknx.info3Led.blinking(500);                                 // Blink info/alert LED
#endif
        }
    } else {
        if (triggeredFlag) {
            knx.getGroupObject(alarmKo).value(false, DPT_Alarm);    // Reset alarm
            triggeredFlag = false;                                  // Reset flag if status is normal
#ifdef INFO3_LED_PIN
            openknx.info3Led.off();                                 // Turn off info/alert LED
#endif
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
    openknx.console.printHelpLine("all", "Display all values");
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
        } else if (cmd == "current debug") {
        openknx.logger.logWithPrefixAndValues("Current", "%f A", current_A);
        return true;
        } else if (cmd == "power") {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("P %.2fW", power_W); }
        openknx.logger.logWithPrefixAndValues("Power", "%.2f W", power_W);
        return true;
    } else if (cmd == "energy") {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("E %.2fWh", totalEnergy_Wh); }
        openknx.logger.logWithPrefixAndValues("Energy", "%.4f Wh", totalEnergy_Wh);
        return true;
    } else if (cmd == "all") {
        openknx.logger.logWithPrefixAndValues("Temperatur", "%.2f °C", temperatur_C);
        openknx.logger.logWithPrefixAndValues("Voltage", "%.1f V", busVoltage_V);
        openknx.logger.logWithPrefixAndValues("Current", "%.2f A", current_A);
        openknx.logger.logWithPrefixAndValues("Power", "%.2f W", power_W);
        openknx.logger.logWithPrefixAndValues("Energy", "%.4f Wh", totalEnergy_Wh);
        openknx.logger.logWithPrefixAndValues("Die Temp INA", "%.2f °C", _ina.getTemperature());
        return true;
    } else if (cmd == "ccenergy") {
        _ina.resetEnergy();
        totalEnergy_Wh = 0.00;
        lastEnergy_Wh = 0.00; // Baseline mit HW-Akkumulator zurücksetzen (sonst negatives Delta beim nächsten Read)
        openknx.flash.save(true); // force save
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("E cleared"); }
        openknx.logger.logWithPrefixAndValues("Energy", "Clear counter finish");        
        return true;
    } else if (cmd.rfind("set ", 0) == 0) {
    const std::string valueStr = cmd.substr(cmd.find(' ') + 1);
    char* end = nullptr;
    float value = std::strtof(valueStr.c_str(), &end);
    // Prüfung: konvertiert und keine Reste im String
    if (end != valueStr.c_str() && *end == '\0') {
        totalEnergy_Wh = value;
        openknx.flash.save(true); // force save
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("E set ok"); }
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("E %.2fWh", totalEnergy_Wh); }
        openknx.logger.logWithPrefixAndValues("Energy", "Set %.4f Wh as new counter", totalEnergy_Wh);
    } else {
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("E set fail"); }
        openknx.logger.logWithPrefixAndValues("Energy", "Invalid argument for set command");
        return false;
        }
        return true;
    } else if (cmd == "limits") {
        //openknx.logger.logWithPrefixAndValues("Alert Limit", "%i mV", _ina.getAlertLimit());
        return true;
    }
    return false;
}

float MeasuringModule::getMeasurementValue(const std::string &parameter)
{
    if (parameter == "temp") {
        return temperatur_C;
    } else if (parameter == "voltage") {
        return busVoltage_V;
    } else if (parameter == "current") {
        return current_A;
    } else if (parameter == "power") {
        return power_W;
    } else if (parameter == "energy") {
        return totalEnergy_Wh;
    }
    return 0.0;
}

bool MeasuringModule::initI2cConnectionTemp() 
{
    // Call dependend init for temp100 sensor
    if (!_tmp100.init()) {
        logErrorP("ERROR: initialization for TMP100 failed...");
        doResetI2cTemp = true;
        tempI2cConnection = false;
#ifdef INFO2_LED_PIN  
        openknx.info2Led.on();
#endif
        return false;
    }
    // Set default values for sensor
    _tmp100.setResolution(RES025);
    logInfoP("Init messurment I2C connection for TEMP100 sucessful");
    doResetI2cTemp = false;
    tempI2cConnection = true;
#ifdef INFO2_LED_PIN  
        openknx.info2Led.off();
#endif
    return true;
}

bool MeasuringModule::initI2cConnectionIna() 
{
    // Call dependend init for ina sensor
    if (!_ina.begin(shuntValue / 1000, maxcurrent, CT_5, AVG_64)) {
        logErrorP("ERROR: initialization for INA%i failed...", _ina.getChipType());
        doResetI2cIna = true;
        inaI2cConnection = false;
#ifdef INFO2_LED_PIN  
        openknx.info2Led.on();
#endif
        return false;
    }
    // Set default values for sensor
    _ina.configureAlert(ALERT_OVER_CURRENT, OVER_CURRENT, true, true);     // HW-Schutz fest auf 6 A (ETS-Schnellwertalarm ist separat)
    // INA228: HW-Energieregister läuft seit Power-on – Baseline angleichen,
    // damit die erste Messung keinen Altbestand auf den Flash-Wert addiert.
    if (_ina.getChipType() == INA228_TYPE)
        lastEnergy_Wh = _ina.getEnergy();
    logInfoP("Init messurment I2C connection INA%i sucessful", _ina.getChipType());
    doResetI2cIna = false;
    inaI2cConnection = true;
#ifdef INFO2_LED_PIN  
        openknx.info2Led.off();
#endif
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
    Wire1.beginTransmission(I2C_INA22x_DEVICE_ADDRESS);
    byte resultIna = Wire1.endTransmission();       //  0 : Success  1 : Data too long  2 : NACK on transmit of address  3 : NACK on transmit of data  4 : Other error  5 : Timeout
    if (resultIna != 0) {
        logErrorP("INA%i not available via I2C %d", _ina.getChipType(), resultIna);
        openknx.console.writeDiagenoseKo("ER I2C INA %d", resultIna);
        doResetI2cIna = true;
        return false;
    }
    return true;
}

bool MeasuringModule::getTempI2cConnectionState() 
{
    return tempI2cConnection;
}

bool MeasuringModule::getInaI2cConnectionState() 
{
    return inaI2cConnection;
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

MeasuringModule openknxMeasuringModule;