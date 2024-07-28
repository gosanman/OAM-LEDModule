#ifndef LEDMODULE_H
#define LEDMODULE_H

#include <Adafruit_PWMServoDriver.h>
#include <OpenKNX.h>
#include "hardware.h"

#include "LEDHelper.h"

#ifndef OPENKNX_DUALCORE
    #pragma warn "LEDModule needs OPENKNX_DUALCORE"
#endif

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
    void setup1();
    void loop() override;
    void loop1();
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
    bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;

private:
    int8_t deviceSelect = 0;        // 0=LED-DK-06x24V, 1=LED-DK-12x24V
    int8_t operatinModeSelect = 0;  // 0=6xEK, 1=1xRGBCTT, 2=1xRGBW and 1xEK, 3=1xRGB and 2xEK, 4=1xRGB and 1xTW, 5=2xTW and 1xEK, 6=1xTW and 3xEK
    int16_t pwmFreqSelect = 488;    // 211, 488, 600, 832, 1000, 1200
    bool useDiagnoseKo = 0;
    unsigned long currentTime = 0;
    int8_t usedChannels = 0;
    uint32_t _timerCheckI2cConnection = 0;
    bool doResetI2c = false;

    void koHandleDayNight(GroupObject & ko);
    bool initI2cConnection();
    bool checkI2cConnection();
    #ifdef FUNC1_BUTTON_PIN
		  void handleFunc1(uint8_t setting);
		  bool _currentToggleState = false;
    #endif
    void handleFunctionPropertySwitch(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);

    Adafruit_PWMServoDriver _pwm;
 
    DimChannel *channel[MAXCHANNELSHW];
    DimChannel *channelEK[MAXCHANNELSHW];
    DimChannel *channelTW[MAXCHANNELSHW];
    DimChannel *channelRGB[MAXCHANNELSHW];
    HWChannel *hwchannels[MAXCHANNELSHW]; 
};

extern LEDModule openknxLEDModule;

#endif