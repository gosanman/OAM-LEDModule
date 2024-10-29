#ifndef MEASURINGMODULE_H
#define MEASURINGMODULE_H

#include "INA226_WE.h"
#include "LIB_TEMP100.h"
#include <OpenKNX.h>
#include "hardware.h"

#ifndef OPENKNX_DUALCORE
    #pragma warn "MeasuringModule needs OPENKNX_DUALCORE"
#endif

#define TIMEBASE_SECONDS        0
#define TIMEBASE_MINUTES        1
#define TIMEBASE_HOURS          2
#define TIMEBASE_TENTH_SECONDS  3

class MeasuringModule : public OpenKNX::Module
{
public:
    MeasuringModule();
    void setup() override;
    void setup1();
    void loop() override;
    void loop1();
    const std::string name() override;
    const std::string version() override;
    
    void readFlash(const uint8_t *buffer, const uint16_t size) override;
    void writeFlash() override;
    uint16_t flashSize() override;

    void sendSingleMeasurement();
    void showHelp() override;
    bool processCommand(const std::string cmd, bool diagnoseKo);

    static MeasuringModule *instance();
    static MeasuringModule *_instance;

private:
    float shuntValue = 0;                   // value will divided by 1000
    float maxcurrent = 0;
    bool measurementSend = true;            // 0=No, 1=Yes
    bool tempSensorPresent = false;         // 0=No, 1=Yes
    uint32_t measurementInterval = 60000;   
    uint32_t _lastMeasurementSend = 0;
    uint32_t _lastMeasurementGet = 0;

    bool checkTemp = false;
    float overTemp = 0;
    bool checkVoltage = false;
    float overVoltage = 0;
    float underVoltage = 0;
    bool checkCurrent = false;
    float overCurrent = 0;

    // Status flags for alarms
    bool overTempTriggered = false;
    bool overVoltageTriggered = false;
    bool underVoltageTriggered = false;
    bool overCurrentTriggered = false;

    float elapsedTime_s = 0;
    unsigned long currentTime = 0;
    unsigned long lastUpdateTime = 0;

    float shuntVoltage_mV = 0.0;
    float loadVoltage_V = 0.0;
    float busVoltage_V = 0.0;
    float current_mA = 0.0;
    float current_A = 0.0;
    float power_mW = 0.0;
    float power_W = 0.0;
    float totalEnergy_Wh = 0.0;
    
    float temperatur_C = 0.0;

    uint32_t _timerCheckOverflow = 0;
    uint32_t _timerCheckI2cConnection = 0;
    bool doResetI2cTemp = false;
    bool doResetI2cIna = false;

    void getSingleMeasurement();
    bool initI2cConnectionTemp();
    bool checkI2cConnectionTemp();
    bool initI2cConnectionIna();
    bool checkI2cConnectionIna();
    void getOverflowValue();
    void checkAlarmDefinitions();
    void checkAndTriggerAlarm(bool condition, bool &triggeredFlag, uint16_t alarmKo, const String &messageDiagnoseKo);

    uint32_t getTimeWithPattern(uint16_t time, uint8_t base);

    INA226_WE _ina226;
    TMP100_WE _tmp100;
};

extern MeasuringModule openknxMeasuringModule;

#endif