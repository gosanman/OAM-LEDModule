#ifndef HWCHANNEL_H
#define HWCHANNEL_H

#include "LEDModule.h"
#include "DimChannel.h"
#include <OpenKNX.h>

// Dimmer  
enum DimTask {
    DIM_IDLE,
    DIM_STOP,
    DIM_ON,
    DIM_OFF,
    DIM_SOFTON,
    DIM_SOFTOFF,
    DIM_UP,
    DIM_DOWN,
    DIM_SET,
    DIM_VALUE
};

class LEDModule;
class HWChannel
{
public:
    HWChannel(uint8_t iId);
    ~HWChannel();

    void setup(uint8_t hwchannel, uint8_t curve, uint16_t durationabsolute, uint16_t durationrelative);
    void setChannelValue(byte value);
    void task();
    void taskStop();
    void taskOn();
    void taskOff();
    void taskSoftOn();
    void taskSoftOff();
    void taskDimUp();
    void taskDimDown();
    void taskNewValue(byte valueNew);
    void taskSetValue(byte valueNew);

    // Information
    bool isBusy();
    bool updateAvailable();
    byte getCurrentValue();
    void resetUpdateFlag();

private:
    uint8_t mId = 0;
    uint8_t _hwchannel;
    uint8_t _curve;

    uint8_t _valueMin;
    uint8_t _valueMax;
    uint8_t _valueNew;
    uint8_t _valueSetNew;
    uint8_t _valueCurrent;
    uint8_t _currentTask;

    uint8_t _updateCounter;
    uint8_t _updateInterval;

    bool _updateAvailable;
    bool _busy;
    uint16_t _durationAbsolute;
    uint16_t _durationRelative;

    uint32_t _currentMillis;
    uint32_t _lastTaskExecution;
    uint32_t _delayAbsolute;
    uint32_t _delayRelative;

    // Private Methoden für die Task-Logik
    void handleDimStop();
    void handleDimOn();
    void handleDimOff();
    void handleDimSoftOn();
    void handleDimSoftOff();
    void handleDimUp();
    void handleDimDown();
    void handleDimSet();
    void handleDimValue();

    void updateValue(int8_t step, uint32_t delay);
    void calculateUpdateInterval();
};

#endif