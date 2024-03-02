#include "LEDModule.h"

#include "DimChannel_EK.h"
#include "DimChannel_TW.h"
#include "DimChannel_RGB.h"
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
    for (byte ch = 0; ch < CHANNELSHW; ch++) {
        _pwm.setPin(ch, 0);
    }

    // Create dimmer groups based on the parameter PT_OperationMode
    switch (operatinModeSelect)
    {
    case 0: // 6xEK
    {
        channels_ek[0] = new DimChannel_EK(0);
        channels_ek[0]->setup(0, APP_KoKO_ChannelASwitch);
        channels_ek[1] = new DimChannel_EK(1);
        channels_ek[1]->setup(1, APP_KoKO_ChannelBSwitch);
        channels_ek[2] = new DimChannel_EK(2);
        channels_ek[2]->setup(2, APP_KoKO_ChannelCSwitch);
        channels_ek[3] = new DimChannel_EK(3);
        channels_ek[3]->setup(3, APP_KoKO_ChannelDSwitch);
        channels_ek[4] = new DimChannel_EK(4);
        channels_ek[4]->setup(4, APP_KoKO_ChannelESwitch);
        channels_ek[5] = new DimChannel_EK(5);
        channels_ek[5]->setup(5, APP_KoKO_ChannelFSwitch);
    }
    break;
    case 1: // 3xTW
    {
        channels_tw[0] = new DimChannel_TW(0);
        channels_tw[0]->setup(0, 1, APP_KoKO_ChannelTW1Switch);
        channels_tw[1] = new DimChannel_TW(1);
        channels_tw[1]->setup(2, 3, APP_KoKO_ChannelTW2Switch);
        channels_tw[2] = new DimChannel_TW(2);
        channels_tw[2]->setup(4, 5, APP_KoKO_ChannelTW3Switch);
    }
    break;
    case 2: // 2xTW and 2xEK
    {
        channels_tw[0] = new DimChannel_TW(0);
        channels_tw[0]->setup(0, 1, APP_KoKO_ChannelTW1Switch);
        channels_tw[1] = new DimChannel_TW(1);
        channels_tw[1]->setup(2, 3, APP_KoKO_ChannelTW2Switch);
        channels_ek[4] = new DimChannel_EK(4);
        channels_ek[4]->setup(4, APP_KoKO_ChannelESwitch);
        channels_ek[5] = new DimChannel_EK(5);
        channels_ek[5]->setup(5, APP_KoKO_ChannelFSwitch);
    }
    break;
    case 3: // 2xRGB
    {
        channels_rgb[0] = new DimChannel_RGB(0);
        channels_rgb[0]->setup(0, 1, 2, APP_KoKO_ChannelRGB1Switch);
        channels_rgb[1] = new DimChannel_RGB(1);
        channels_rgb[1]->setup(3, 4, 5, APP_KoKO_ChannelRGB2Switch);
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
    switch (operatinModeSelect)
    {
    case 0: // 6xEK
    {
        channels_ek[0]->task();
        channels_ek[1]->task();
        channels_ek[2]->task();
        channels_ek[3]->task();
        channels_ek[4]->task();
        channels_ek[5]->task();
    }
    break;
    case 1: // 3xTW
    {
        channels_tw[0]->task();
        channels_tw[1]->task();
        channels_tw[2]->task();
    }
    break;
    case 2: // 2xTW and 2xEK
    {
        channels_tw[0]->task();
        channels_tw[1]->task();
        channels_ek[4]->task();
        channels_ek[5]->task();
    }
    break;
    case 3: // 2xRGB
    {
        channels_rgb[0]->task();
        channels_rgb[1]->task();
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

// Core function to set value
void LEDModule::setHwChannelValue(byte channel, byte value, int curve) {
    _pwm.setPin(channel, curves[value][curve]);
}

void LEDModule::processInputKo(GroupObject &ko) {
    // we have to check first, if external KO are used
    uint16_t asap = ko.asap();
    // check if KO for Channels
    if (asap < 100) // KO not for Channels
        return;
    // send Input KO to the correct dimmer based on the parameter PT_OperationMode
    switch (operatinModeSelect)
    {
    case 0: // 6xEK
    {
        int channelIndexEK = (asap - 101) / KO_PER_CHANNEL_EK;
        if (channelIndexEK >= 0 && channelIndexEK < 6) { channels_ek[channelIndexEK]->processInputKoEK(ko); }
    }
    break;
    case 1: // 3xTW
    {
        int channelIndexTW = (asap - 262) / KO_PER_CHANNEL_TW;
        if (channelIndexTW >= 0 && channelIndexTW < 3) { channels_tw[channelIndexTW]->processInputKoTW(ko); }
    }
    break;
    case 2: // 2xTW and 2xEK
    {
        int channelIndexTW = (asap - 262) / KO_PER_CHANNEL_TW;
        if (channelIndexTW >= 0 && channelIndexTW < 2) { channels_tw[channelIndexTW]->processInputKoTW(ko); }       
        int channelIndexEK = (asap - 181) / KO_PER_CHANNEL_EK;
        if (channelIndexEK >= 4 && channelIndexEK < 6) { channels_ek[channelIndexEK]->processInputKoEK(ko); }
    }
    break;
    case 3: // 2xRGB
    {
        int channelIndexRGB = (asap - 362) / KO_PER_CHANNEL_RGB;
        if (channelIndexRGB >= 0 && channelIndexRGB < 2) { channels_rgb[channelIndexRGB]->processInputKoRGB(ko); }
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
    for (byte ch = 0; ch < CHANNELSHW; ch++) {
        _pwm.setPin(ch, 0);
    }
}

void LEDModule::savePower() {
    processBeforeRestart();
}

LEDModule openknxLEDModule;
