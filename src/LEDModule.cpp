#include "LEDModule.h"

#include "DimChannel_EK.h"
#include "DimChannel_TW.h"
#include "DimChannel_RGB.h"
#include "DimChannel.h"
#include "HwChannel.h"

LEDModule *LEDModule::_instance = nullptr;

LEDModule::LEDModule() {
    LEDModule::_instance = this;
}

LEDModule *LEDModule::instance() {
    return LEDModule::_instance;
}

const std::string LEDModule::name() {
    return "LED";
}

const std::string LEDModule::version() {
    return "0.1dev";
}

void LEDModule::setup() {
    // save default values from PA
    operatinModeSelect = ParamAPP_PT_OperatingMode;
    pwmFreqSelect = ParamAPP_PT_PwmFrequenz;
    useDiagnoseKo = ParamAPP_PT_Diagnose;
    
    // Debug
    logDebugP("Operating Mode: %i", operatinModeSelect);
    logDebugP("PWM frequenz: %i", pwmFreqSelect);
    logDebugP("DayNight: %i", ParamAPP_PT_DayNight);

    // Init I2C connection and Lib
    if (initI2cConnection()) {
        logInfoP("Init pwm I2C connection sucessful");
    }

    // Set all Channels off for a defined status
    for (byte ch = 0; ch < MAXCHANNELSHW; ch++) {
        _pwm.setPin(ch, 0);
    }

    // Create dimmer groups based on the parameter PT_OperationMode
    // Pin 0 = A    Pin  6 = G
    // Pin 1 = B    Pin  7 = H
    // Pin 2 = C    Pin  8 = I
    // Pin 3 = D    Pin  9 = J
    // Pin 4 = E    Pin 10 = K
    // Pin 5 = F    Pin 11 = L
    switch (operatinModeSelect)
    {
    case 0:
    {
        // 6xEK
        #ifdef BOARD_KNXLED_DK_06_V10
            usedChannels = 6;
        #endif
        // 12xEK
        #ifdef BOARD_KNXLED_DK_12_V10
            usedChannels = 12;
        #endif

        for (uint8_t i = 0; i < usedChannels; ++i)
        {
            channel[i] = new DimChannel_EK(i);
            uint8_t hwchannel[] = {i};
            channel[i]->setup(hwchannel, KO_OFFSET_EK_BLOCK + (i * KO_PER_CHANNEL_EK));
        }
    }
    break;
    case 1:
    {
        // 3xTW
        #ifdef BOARD_KNXLED_DK_06_V10
            usedChannels = 3;
        #endif
        // 6xTW
        #ifdef BOARD_KNXLED_DK_12_V10
            usedChannels = 6;
        #endif

        for (uint8_t i = 0; i < usedChannels; ++i)
        {
            channel[i] = new DimChannel_TW(i);
            uint8_t hwchannel[] = {static_cast<uint8_t>(i * 2), static_cast<uint8_t>(i * 2 + 1)};
            channel[i]->setup(hwchannel, KO_OFFSET_TW_BLOCK + (i * KO_PER_CHANNEL_TW));
        }
    }
    break;
    case 2:
    {
        // 2xTW and 2xEK 
        #ifdef BOARD_KNXLED_DK_06_V10
            channel[0] = new DimChannel_TW(0);
            uint8_t hwchannel0[] = {0, 1};
            channel[0]->setup(hwchannel0, KO_OFFSET_TW_BLOCK + (0 * KO_PER_CHANNEL_TW));
            channel[1] = new DimChannel_TW(1);
            uint8_t hwchannel1[] = {2, 3};
            channel[1]->setup(hwchannel1, KO_OFFSET_TW_BLOCK + (1 * KO_PER_CHANNEL_TW));
            channel[2] = new DimChannel_EK(0);
            uint8_t hwchannel2[] = {4};
            channel[2]->setup(hwchannel2, KO_OFFSET_EK_BLOCK + (0 * KO_PER_CHANNEL_EK));
            channel[3] = new DimChannel_EK(1);
            uint8_t hwchannel3[] = {5};
            channel[3]->setup(hwchannel3, KO_OFFSET_EK_BLOCK + (1 * KO_PER_CHANNEL_EK));
            //used Channels
            usedChannels = 4;
        #endif
        // 4xTW and 4xEK - TODO
        #ifdef BOARD_KNXLED_DK_12_V10
            channel[0] = new DimChannel_TW(0);
            uint8_t hwchannel0[] = {0, 1};
            channel[0]->setup(hwchannel0, KO_OFFSET_TW_BLOCK + (0 * KO_PER_CHANNEL_TW));
            channel[1] = new DimChannel_TW(1);
            uint8_t hwchannel1[] = {2, 3};
            channel[1]->setup(hwchannel1, KO_OFFSET_TW_BLOCK + (1 * KO_PER_CHANNEL_TW));
            channel[2] = new DimChannel_EK(0);
            uint8_t hwchannel2[] = {4};
            channel[2]->setup(hwchannel2, KO_OFFSET_EK_BLOCK + (0 * KO_PER_CHANNEL_EK));
            channel[3] = new DimChannel_EK(1);
            uint8_t hwchannel3[] = {5};
            channel[3]->setup(hwchannel3, KO_OFFSET_EK_BLOCK + (1 * KO_PER_CHANNEL_EK));
            //used Channels
            usedChannels = 4;
        #endif
    }
    break;
    case 3:
    {
        // 2xRGB
        #ifdef BOARD_KNXLED_DK_06_V10
            usedChannels = 2;
        #endif
        // 4xRGB
        #ifdef BOARD_KNXLED_DK_12_V10
            usedChannels = 4;
        #endif

        for (uint8_t i = 0; i < usedChannels; ++i)
        {
            channel[i] = new DimChannel_RGB(i);
            uint8_t hwchannel[] = {static_cast<uint8_t>(i * 3), static_cast<uint8_t>(i * 3 + 1), static_cast<uint8_t>(i * 3 + 2)};
            channel[i]->setup(hwchannel, KO_OFFSET_RGB_BLOCK + (i * KO_PER_CHANNEL_RGB));
        }
    }
    break;
    case 4: // 1x RGB + 1x TW + 1x EK
    {
        // 1x RGB + 1x TW + 1x EK
        #ifdef BOARD_KNXLED_DK_06_V10
            channel[0] = new DimChannel_EK(0);
            uint8_t hwchannel0[] = {0};
            channel[0]->setup(hwchannel0, KO_OFFSET_EK_BLOCK + (0 * KO_PER_CHANNEL_EK));
            channel[1] = new DimChannel_TW(0);
            uint8_t hwchannel1[] = {1, 2};
            channel[1]->setup(hwchannel1, KO_OFFSET_TW_BLOCK + (0 * KO_PER_CHANNEL_TW));
            channel[2] = new DimChannel_RGB(0);
            uint8_t hwchannel2[] = {3, 4, 5};
            channel[2]->setup(hwchannel2, KO_OFFSET_RGB_BLOCK + (0 * KO_PER_CHANNEL_RGB));
            usedChannels = 3; 
        #endif  
        // 2x RGB + 2x TW + 2x EK - TODO
        #ifdef BOARD_KNXLED_DK_12_V10
            channel[0] = new DimChannel_EK(0);
            uint8_t hwchannel0[] = {0};
            channel[0]->setup(hwchannel0, KO_OFFSET_EK_BLOCK + (0 * KO_PER_CHANNEL_EK));
            channel[1] = new DimChannel_TW(0);
            uint8_t hwchannel1[] = {1, 2};
            channel[1]->setup(hwchannel1, KO_OFFSET_TW_BLOCK + (0 * KO_PER_CHANNEL_TW));
            channel[2] = new DimChannel_RGB(0);
            uint8_t hwchannel2[] = {3, 4, 5};
            channel[2]->setup(hwchannel2, KO_OFFSET_RGB_BLOCK + (0 * KO_PER_CHANNEL_RGB));
            usedChannels = 3; 
        #endif       
    }
    break;
    case 5:
    {
        // 1x RGB + 3x EK
        #ifdef BOARD_KNXLED_DK_06_V10
            channel[0] = new DimChannel_EK(0);
            uint8_t hwchannel0[] = {0};
            channel[0]->setup(hwchannel0, KO_OFFSET_EK_BLOCK + (0 * KO_PER_CHANNEL_EK));
            channel[1] = new DimChannel_EK(1);
            uint8_t hwchannel1[] = {1};
            channel[1]->setup(hwchannel1, KO_OFFSET_EK_BLOCK + (1 * KO_PER_CHANNEL_EK));
            channel[2] = new DimChannel_EK(2);
            uint8_t hwchannel2[] = {2};
            channel[2]->setup(hwchannel2, KO_OFFSET_EK_BLOCK + (2 * KO_PER_CHANNEL_EK));
            channel[3] = new DimChannel_RGB(1);
            uint8_t hwchannel3[] = {3, 4, 5};
            channel[3]->setup(hwchannel3, KO_OFFSET_RGB_BLOCK + (1 * KO_PER_CHANNEL_RGB));
            usedChannels = 4;
        #endif
        // 2x RGB + 6x EK - TODO
        #ifdef BOARD_KNXLED_DK_12_V10
            channel[0] = new DimChannel_EK(0);
            uint8_t hwchannel0[] = {0};
            channel[0]->setup(hwchannel0, KO_OFFSET_EK_BLOCK + (0 * KO_PER_CHANNEL_EK));
            channel[1] = new DimChannel_EK(1);
            uint8_t hwchannel1[] = {1};
            channel[1]->setup(hwchannel1, KO_OFFSET_EK_BLOCK + (1 * KO_PER_CHANNEL_EK));
            channel[2] = new DimChannel_EK(2);
            uint8_t hwchannel2[] = {2};
            channel[2]->setup(hwchannel2, KO_OFFSET_EK_BLOCK + (2 * KO_PER_CHANNEL_EK));
            channel[3] = new DimChannel_RGB(1);
            uint8_t hwchannel3[] = {3, 4, 5};
            channel[3]->setup(hwchannel3, KO_OFFSET_RGB_BLOCK + (1 * KO_PER_CHANNEL_RGB));
            usedChannels = 4;
        #endif
    }
    break;
    default:
        logErrorP("Operation Mode not valide");
        break;
    }

    #ifdef FUNC1_BUTTON_PIN
        openknx.func1Button.onShortClick([=] { 
            logDebugP("Func1 Button - pressed short");
            uint8_t sett = ParamAPP_PT_Func1BtnClick;
            handleFunc1(sett);
        });
        openknx.func1Button.onLongClick([=] { 
            logDebugP("Func1 Button - pressed long");
            uint8_t sett = ParamAPP_PT_Func1BtnLongClick;
            handleFunc1(sett);
        });
        openknx.func1Button.onDoubleClick([=] {
            logDebugP("Func1 Button - pressed double");
            uint8_t sett = ParamAPP_PT_Func1BtnDblClick;
            handleFunc1(sett);
        });
    #endif
}

void LEDModule::loop()
{
    for (int i = 0; i < usedChannels; i++)
        channel[i]->task();
    //check if I2C connection possible, if not reset and init the pwm
    if (delayCheck(_timerCheckI2cConnection, 30000)) {
        checkI2cConnection();
        _timerCheckI2cConnection = millis();
    }
}

// Core function to set value, change if you use other hardware
void LEDModule::setHwChannelValue(byte channel, byte value, int curve) {
    _pwm.setPin(channel, curves[value][curve]);
}

void LEDModule::processInputKo(GroupObject &ko) {
    // we have to check first, if external KO are used
    uint16_t koNum = ko.asap();
    logDebugP("Got KO %i", koNum);
    switch(koNum)
    {
        //diagnose
        case BASE_KoDiagnose:
            openknx.console.processDiagnoseKo(ko);
            break;

        //Tag/Nacht Objekt
        case APP_KoKO_DayNight:
            koHandleDayNight(ko);
            break;
    }    
    // check if KO for Channels
    if (koNum < 100) // KO not for Channels
        return;
    // send Input KO to the correct dimmer
    int channelIndexEK = (koNum - KO_OFFSET_EK_BLOCK) / KO_PER_CHANNEL_EK;
    if (channelIndexEK >= 0 && channelIndexEK < MAXCHANNELSEK) { channel[channelIndexEK]->processInputKo(ko); }
    int channelIndexTW = (koNum - KO_OFFSET_TW_BLOCK) / KO_PER_CHANNEL_TW;
    if (channelIndexTW >= 0 && channelIndexTW < MAXCHANNELSTW) { channel[channelIndexTW]->processInputKo(ko); }
    int channelIndexRGB = (koNum - KO_OFFSET_RGB_BLOCK) / KO_PER_CHANNEL_RGB;
    if (channelIndexRGB >= 0 && channelIndexRGB < MAXCHANNELSRGB) { channel[channelIndexRGB]->processInputKo(ko); }
}

void LEDModule::koHandleDayNight(GroupObject & ko)
{
    bool value = ko.value(DPT_Switch);
    if(ParamAPP_PT_DayNight) value = !value;
    logDebugP("Broadcast Day/Night %i to channels", value);
    if(ParamAPP_PT_DayNight) value = !value;

    for(int i = 0; i < usedChannels; i++)
        channel[i]->setDayNight(value);
}

void LEDModule::showHelp()
{
    openknx.logger.color(CONSOLE_HEADLINE_COLOR);
    openknx.logger.log("======================== LED Module ============================================");
    openknx.logger.color(0);
    #ifdef BOARD_KNXLED_DK_06_V10
        openknx.console.printHelpLine("chon <ch>", "Switch Channel 0-5 on");
        openknx.console.printHelpLine("choff <ch>", "Switch Channel 0-5 off");
        openknx.console.printHelpLine("chval <ch> <value>", "Switch Channel 0-5 to value 0-4095");
    #endif
    #ifdef BOARD_KNXLED_DK_12_V10
        openknx.console.printHelpLine("chon <ch>", "Switch Channel 0-11 on");
        openknx.console.printHelpLine("choff <ch>", "Switch Channel 0-11 off");
        openknx.console.printHelpLine("chval <ch> <value>", "Switch Channel 0-11 to value 0-4095");
    #endif
}

bool LEDModule::processCommand(const std::string cmd, bool diagnoseKo) 
{
    if (!diagnoseKo && (cmd.rfind("chon ", 0) == 0 || cmd.rfind("choff ", 0) == 0)) {
        _pwm.setPin(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"));
        openknx.logger.logWithPrefixAndValues("Channel", "Switch channel %i to %i", std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"));
        return true;
    } else if (!diagnoseKo && cmd.rfind("chval ", 0) == 0) {
        _pwm.setPin(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.substr(cmd.find(' ') + 3)));
        openknx.logger.logWithPrefixAndValues("Channel", "Set value channel %i to %i", std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.substr(cmd.find(' ') + 3)));
        return true;
    }
    return false;
}

void LEDModule::processBeforeRestart() {
    for (byte ch = 0; ch < MAXCHANNELSHW; ch++) {
        _pwm.setPin(ch, 0);
    }
}

void LEDModule::savePower() {
    processBeforeRestart();
}

bool LEDModule::initI2cConnection() {
    _pwm = Adafruit_PWMServoDriver(I2C_PCA9685_DEVICE_ADDRESS, Wire1);
    // Call dependend init for led
    if (!_pwm.begin()) {
        logErrorP("ERROR: initialization for PCA9685 failed...");
        doResetI2c = true;
        return false;
    }
    delay(10);
    // Set default values for led
    _pwm.setOscillatorFrequency(25000000);
    _pwm.setPWMFreq(pwmFreqSelect);         // 1600 is the maximum PWM frequency
    _pwm.setOutputMode(true);               // External N-type driver, set to output mode INVRT = 0 OUTDRV = 1
    
    doResetI2c = false;
    return true;
}

bool LEDModule::checkI2cConnection() {
    if (doResetI2c) { 
        return initI2cConnection();
    }
    byte result;
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
    result = Wire1.endTransmission();       //  0: Success  1: Data too long  2: NACK on transmit of address  3: NACK on transmit of data  4: Other error  5: Timeout
    if (result == 0) {
        return true;
    } else {
        logErrorP("PCA9685 PWM not available via I2C %d", result);
        openknx.console.writeDiagenoseKo("ER I2C PWM");
        doResetI2c = true;
        return false;
    }
    return false;
}

#ifdef FUNC1_BUTTON_PIN
void LEDModule::handleFunc1(uint8_t setting)
{
    switch(setting)
    {
        case PT_FuncClickAction_on:
            logDebugP("All channels on");
            for (byte ch = 0; ch < MAXCHANNELSHW; ch++) { _pwm.setPin(ch, 4095); }
            openknx.info1Led.on();
            break;
        case PT_FuncClickAction_off:
            logDebugP("All channels off");
            for (byte ch = 0; ch < MAXCHANNELSHW; ch++) { _pwm.setPin(ch, 0); }
            openknx.info1Led.off();
            break;
        case PT_FuncClickAction_toggle:
            _currentToggleState = !_currentToggleState;
            logDebugP("Toggle %i all Channels", _currentToggleState);
            for (byte ch = 0; ch < MAXCHANNELSHW; ch++) { _pwm.setPin(ch, _currentToggleState == 0 ? 0 : 4095); }
            if (_currentToggleState == 0) {
                openknx.info1Led.off();
            } else if (_currentToggleState == 1) {
                openknx.info1Led.on();
            }
            break;
    }
}
#endif

LEDModule openknxLEDModule;
