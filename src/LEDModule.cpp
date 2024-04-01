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
    
    // Debug
    logDebugP("Operating Mode: %i", operatinModeSelect);
    logDebugP("PWM frequenz: %i", pwmFreqSelect);
    logDebugP("DayNight: %i", ParamAPP_PT_DayNight);

    // Init Lib
    _pwm = Adafruit_PWMServoDriver(I2C_PCA9685_DEVICE_ADDRESS, Wire1);

    // Call dependend init for led
    if (!_pwm.begin()) {
        logErrorP("ERROR: initialization for PCA9685 failed...");
    }

    // Set default values for led
    _pwm.setOscillatorFrequency(25000000);
    _pwm.setPWMFreq(pwmFreqSelect); // 1600 is the maximum PWM frequency

    // Set all Channels off for a defined status
    for (byte ch = 0; ch < MAXCHANNELSHW; ch++) {
        _pwm.setPin(ch, 0);
    }

    // Create dimmer groups based on the parameter PT_OperationMode
    switch (operatinModeSelect)
    {
    case 0: // 6xEK
    {
        channel[0] = new DimChannel_EK(0);
        uint8_t hwchannel0[] = {0};
        channel[0]->setup(hwchannel0, KO_OFFSET_EK_BLOCK + (0 * KO_PER_CHANNEL_EK));
        channel[1] = new DimChannel_EK(1);
        uint8_t hwchannel1[] = {1};
        channel[1]->setup(hwchannel1, KO_OFFSET_EK_BLOCK + (1 * KO_PER_CHANNEL_EK));
        channel[2] = new DimChannel_EK(2);
        uint8_t hwchannel2[] = {2};
        channel[2]->setup(hwchannel2, KO_OFFSET_EK_BLOCK + (2 * KO_PER_CHANNEL_EK));
        channel[3] = new DimChannel_EK(3);
        uint8_t hwchannel3[] = {3};
        channel[3]->setup(hwchannel3, KO_OFFSET_EK_BLOCK + (3 * KO_PER_CHANNEL_EK));
        channel[4] = new DimChannel_EK(4);
        uint8_t hwchannel4[] = {4};
        channel[4]->setup(hwchannel4, KO_OFFSET_EK_BLOCK + (4 * KO_PER_CHANNEL_EK));
        channel[5] = new DimChannel_EK(5);
        uint8_t hwchannel5[] = {5};
        channel[5]->setup(hwchannel5, KO_OFFSET_EK_BLOCK + (5 * KO_PER_CHANNEL_EK));
        //used Channels
        usedChannels = 6;
    }
    break;
    case 1: // 3xTW
    {
        channel[0] = new DimChannel_TW(0);
        uint8_t hwchannel0[] = {0, 1};
        channel[0]->setup(hwchannel0, KO_OFFSET_TW_BLOCK + (0 * KO_PER_CHANNEL_TW));
        channel[1] = new DimChannel_TW(1);
        uint8_t hwchannel1[] = {2, 3};
        channel[1]->setup(hwchannel1, KO_OFFSET_TW_BLOCK + (1 * KO_PER_CHANNEL_TW));
        channel[2] = new DimChannel_TW(2);
        uint8_t hwchannel2[] = {4, 5};
        channel[2]->setup(hwchannel2, KO_OFFSET_TW_BLOCK + (2 * KO_PER_CHANNEL_TW));
        //used Channels
        usedChannels = 3;
    }
    break;
    case 2: // 2xTW and 2xEK
    {
        channel[0] = new DimChannel_TW(0);
        uint8_t hwchannel0[] = {0, 1};
        channel[0]->setup(hwchannel0, KO_OFFSET_TW_BLOCK + (0 * KO_PER_CHANNEL_TW));
        channel[1] = new DimChannel_TW(1);
        uint8_t hwchannel1[] = {2, 3};
        channel[1]->setup(hwchannel1, KO_OFFSET_TW_BLOCK + (1 * KO_PER_CHANNEL_TW));
        channel[2] = new DimChannel_EK(4);
        uint8_t hwchannel4[] = {4};
        channel[2]->setup(hwchannel4, KO_OFFSET_EK_BLOCK + (4 * KO_PER_CHANNEL_EK));
        channel[3] = new DimChannel_EK(5);
        uint8_t hwchannel5[] = {5};
        channel[3]->setup(hwchannel5, KO_OFFSET_EK_BLOCK + (5 * KO_PER_CHANNEL_EK));
        //used Channels
        usedChannels = 4;
    }
    break;
    case 3: // 2xRGB
    {
        channel[0] = new DimChannel_RGB(0);
        uint8_t hwchannel0[] = {0, 1, 2};
        channel[0]->setup(hwchannel0, KO_OFFSET_RGB_BLOCK + (0 * KO_PER_CHANNEL_RGB));
        channel[1] = new DimChannel_RGB(1);
        uint8_t hwchannel1[] = {3, 4, 5};
        channel[1]->setup(hwchannel1, KO_OFFSET_RGB_BLOCK + (1 * KO_PER_CHANNEL_RGB));
        //used Channels
        usedChannels = 2;
    }
    break;
    case 4: // 1x RGB + 1x TW + 1x EK
    {

    }
    break;
    case 5: // 1x RGB + 3x EK
    {

    }
    break;
    default:
        logErrorP("Operation Mode not valide");
        break;
    }
}

void LEDModule::loop()
{
    for(int i = 0; i < usedChannels; i++)
    channel[i]->task();
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
        //broadcast switch
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
    logDebugP("Broadcast Day/Night %i", value);
    if(ParamAPP_PT_DayNight) value = !value;

    for(int i = 0; i < usedChannels; i++)
        channel[i]->setDayNight(value);
}

void LEDModule::showHelp()
{
    openknx.logger.color(CONSOLE_HEADLINE_COLOR);
    openknx.logger.log("======================== LED Module ============================================");
    openknx.logger.color(0);
    openknx.console.printHelpLine("chon <ch>", "Switch Channel 0-5 on");
    openknx.console.printHelpLine("choff <ch>", "Switch Channel 0-5 off");
    openknx.console.printHelpLine("chval <ch> <value>", "Switch Channel 0-5 to value 0-4095");
}

bool LEDModule::processCommand(const std::string cmd, bool diagnoseKo) 
{
    if (!diagnoseKo && (cmd.rfind("chon ", 0) == 0 || cmd.rfind("choff ", 0) == 0)) {
        setHwChannelValue(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"), 0);
        openknx.logger.logWithPrefixAndValues("Channel", "Switch channel %i to %i", std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"));
        return true;
    } else if (!diagnoseKo && cmd.rfind("chval ", 0) == 0) {
        setHwChannelValue(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.substr(cmd.find(' ') + 3)), 0);
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

LEDModule openknxLEDModule;
