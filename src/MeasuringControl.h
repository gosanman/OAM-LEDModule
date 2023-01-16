#pragma once

#include <Arduino.h>
#include <knx.h>

#include <LEDModule.h>
#include <LEDModuleHardware.h>
#include <LibMeasuring.h>

class MeasuringControl
{
public:
    MeasuringControl();
    ~MeasuringControl();

    void setup();
    void loop();
    static MeasuringControl *sInstance;

    void getSingleMeasurement();

private:
    int MeasurementInterval[4] = {5000, 10000, 30000, 60000};
    byte measurementSend = 1;           // 0=No, 1=Yes
    byte measurementSelectInterval = 1; // 0=5sec, 1=10sec, 2=30sec, 3=60sec
    unsigned long currentTime = 0;

    float shuntVoltage_mV = 0.0;
    float loadVoltage_V = 0.0;
    float busVoltage_V = 0.0;
    float current_mA = 0.0;
    float current_A = 0.0;
    float power_mW = 0.0;
    float power_W = 0.0;
};