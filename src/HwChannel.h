#ifndef HWCHANNEL_H
#define HWCHANNEL_H

#include "LEDModule.h"
#include "DimChannel.h"
#include <OpenKNX.h>

// Dimmer  
#define DIM_IDLE    0
#define DIM_STOP    1
#define DIM_ON      2
#define DIM_OFF     3
#define DIM_SOFTON  4
#define DIM_SOFTOFF 5
#define DIM_UP      6
#define DIM_DOWN    7
#define DIM_SET     8
#define DIM_VALUE   9

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

    //information
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
    
    void calculateUpdateInterval();

};

#endif