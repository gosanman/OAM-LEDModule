#ifndef MEASURINGMODULE_H
#define MEASURINGMODULE_H

#include "LIB_INA226.h"
#include "LIB_TEMP100.h"
#include <OpenKNX.h>
#include "hardware.h"

#ifndef OPENKNX_DUALCORE
    #pragma warn "MeasuringModule needs OPENKNX_DUALCORE"
#endif

class MeasuringModule : public OpenKNX::Module
{
public:
    MeasuringModule();
    void setup() override;
    void setup1() override;
    void loop() override;
    void loop1() override;
    const std::string name() override;
    const std::string version() override;
    
    void getSingleMeasurement();
    void showHelp() override;
    bool processCommand(const std::string cmd, bool diagnoseKo);

    static MeasuringModule *instance();
    static MeasuringModule *_instance;

private:
    float shuntValue = 0;               // value will divided by 1000
    float maxcurrent = 0;
    byte measurementSend = 1;           // 0=No, 1=Yes
    byte tempSensorPresent = 0;         // 0=No, 1=Yes
    int measurementSelectInterval = 1;  // 0=5sec, 1=10sec, 2=30sec, 3=60sec
    uint32_t _lastMeasurementSend = 0;

    int MeasurementInterval[4] = {5000, 10000, 30000, 60000};
    float shuntVoltage_mV = 0.0;
    float loadVoltage_V = 0.0;
    float busVoltage_V = 0.0;
    float current_mA = 0.0;
    float current_A = 0.0;
    float power_mW = 0.0;
    float power_W = 0.0;
    
    float temperatur_C = 0.0;

    uint32_t _timerCheckI2cConnection = 0;
    bool doResetI2c = false;

    bool initI2cConnection();
    bool checkI2cConnection();

    INA226_WE _ina226;
    TMP100_WE _tmp100;
};

extern MeasuringModule openknxMeasuringModule;

#endif