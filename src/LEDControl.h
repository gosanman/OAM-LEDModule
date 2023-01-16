#pragma once

#include <Arduino.h>
#include <knx.h>

#include <LEDModule.h>
#include <LEDModuleHardware.h>
#include <LEDHelper.h>

#include <LibPWM.h>
#include <LibDimControl.h>
#include <DimChannel_EK.h>
//#include <DimChannel_TW.h>
//#include <DimChannel_RGB.h>

void setChannelValue(byte channel, byte value);

class LEDControl
{
public:
    LEDControl();
    ~LEDControl();

    void setup();
    void loop();
    void task();
    void inform();
    void onChannel(uint8_t channel);
    void offChannel(uint8_t channel);
    void dimStop(uint8_t channel);
    void dimUp(uint8_t channel);
    void dimDown(uint8_t channel);
    void dimChannel(uint8_t channel, uint8_t brightness);
    void dimCCT(uint8_t startChannel, uint8_t brightness, uint16_t kelvin, uint16_t colortempww, uint16_t colortempkw);
    void dimRGB(uint8_t startChannel, uint8_t brightness, uint8_t r, uint8_t g, uint8_t b);
    void processInputKo(GroupObject &iKo);
    static LEDControl *sInstance;

    void setChannelDurationRelativ(uint8_t channel, uint16_t value);
    void setChannelDurationAbsolut(uint8_t channel, uint16_t value);
    int getChannelDurationRelativ(uint8_t channel);
    int getChannelDurationAbsolut(uint8_t channel);

private:
    DimChannel_EK *channels_ek[0];

    GroupObject *getKo(uint16_t iKo);

    uint16_t PwmFrequenz[6] = {211, 488, 600, 832, 1000, 1200};

    int numberOfChannel_ek = 0;
 
    int pwmFreqSelect = 1; // 0=211, 1=488, 2=600, 3=832, 4=1000, 5=1200
    byte operatinModeSelect = 0; // 0=5xEK, 1=1xRGBCTT, 2=1xRGBW and 1xEK, 3=1xRGB and 2xEK, 4=1xRGB and 1xTW, 5=2xTW and 1xEK, 6=1xTW and 3xEK
};