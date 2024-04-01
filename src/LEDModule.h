#ifndef LEDMODULE_H
#define LEDMODULE_H

#include <Adafruit_PWMServoDriver.h>
#include <OpenKNX.h>
#include "hardware.h"

#include "LEDHelper.h"

class DimChannel_EK;
class DimChannel_TW;
class DimChannel_RGB;
class DimChannel;
class HWChannel;

class LEDModule : public OpenKNX::Module
{
public:
    LEDModule();
    void setup() override;
    void loop() override;
    const std::string name() override;
    const std::string version() override;
    void processInputKo(GroupObject &ko) override;
    void showHelp() override;
    bool processCommand(const std::string cmd, bool diagnoseKo);
    
    void processBeforeRestart();
    void savePower();
    
    static LEDModule *instance();
    static LEDModule *_instance;
    
    void setHwChannelValue(byte channel, byte value, int curve);

private:
    int8_t operatinModeSelect = 0;  // 0=6xEK, 1=1xRGBCTT, 2=1xRGBW and 1xEK, 3=1xRGB and 2xEK, 4=1xRGB and 1xTW, 5=2xTW and 1xEK, 6=1xTW and 3xEK
    int16_t pwmFreqSelect = 488;    // 211, 488, 600, 832, 1000, 1200
    unsigned long currentTime = 0;
    int8_t usedChannels = 0;

    void koHandleDayNight(GroupObject & ko);

    Adafruit_PWMServoDriver _pwm;
 
    DimChannel *channel[MAXCHANNELSHW];
    HWChannel *hwchannels[MAXCHANNELSHW]; 
};

extern LEDModule openknxLEDModule;

#endif