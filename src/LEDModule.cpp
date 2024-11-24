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

void LEDModule::setup() 
{
    // save default values from PA
    deviceSelect = ParamAPP_ControllerType;
    pwmFreqSelect = ParamAPP_PwmFrequenz;
    
    switch (deviceSelect)
    {
    case 0: // BOARD_KNXLED_DK_06_V10 - LED-DK-06x24V
        operatinModeSelect = ParamAPP_OperatingMode;
        logInfoP("Device: LED-DK-06x24V - 6-Kanal OpenKNX LED Dimmer");
        break;
    case 1: // BOARD_KNXLED_DK_12_V10 - LED-DK-12x24V
        operatinModeSelect = ParamAPP_OperatingMode - 10;
        logInfoP("Device: LED-DK-12x24V - 12-Kanal OpenKNX LED Dimmer");
        break;
    }

    // Debug
    logDebugP("Selected Device: %i", deviceSelect);
    logDebugP("Operating Mode: %i", operatinModeSelect);
    logDebugP("PWM frequenz: %i", pwmFreqSelect);
    logDebugP("DayNight: %i", ParamAPP_DayNight);

    // Init I2C connection and Lib
    _pwm = Adafruit_PWMServoDriver(I2C_PCA9685_DEVICE_ADDRESS, Wire1);
    initI2cConnection();

    // Set all Channels off for a defined status
    for (byte ch = 0; ch < MAXCHANNELSHW; ch++) {
        _pwm.setPin(ch, 0);
    }

    // Create dimmer groups based on the parameter PT_OperationMode
    // Pin 0 = A    Pin  6 = G
    // Pin 1 = B    Pin  7 = H
    // Pin 2 = C    Pin  8 = I      _____________________   _______________________
    // Pin 3 = D    Pin  9 = J      | V+ V+ G H I J K L |   | V+ V+ 6 7 8 9 10 11 |   __________________
    // Pin 4 = E    Pin 10 = K      | V+ V+ A B C D E F |   | V+ V+ 0 1 2 3  4  5 |   | V+ 0 1 2 3 4 5 |
    // Pin 5 = F    Pin 11 = L      =====================   =======================   ==================

    switch (operatinModeSelect)
    {
    case 0:
    {
        #if defined(BOARD_KNXLED_DK_06_V10)
            usedChannels = 6;   // 6x EK
        #elif defined(BOARD_KNXLED_DK_12_V10)
            usedChannels = 12;  // 12x EK
        #endif

        for (uint8_t i = 0; i < usedChannels; ++i)
        {
            channelEK[i] = channel[i] = new DimChannel_EK(i);
            uint8_t hwchannel[] = {i};
            channel[i]->setup(hwchannel);
        }
    }
    break;
    case 1:
    {
        #if defined(BOARD_KNXLED_DK_06_V10)
            usedChannels = 3;   // 3x TW
        #elif defined(BOARD_KNXLED_DK_12_V10)
            usedChannels = 6;   // 6x TW
        #endif

        for (uint8_t i = 0; i < usedChannels; ++i)
        {
            channelTW[i] = channel[i] = new DimChannel_TW(i);
            uint8_t hwchannel[] = {static_cast<uint8_t>(i * 2), static_cast<uint8_t>(i * 2 + 1)};
            channel[i]->setup(hwchannel);
        }
    }
    break;
    case 2:
    {
        // 2xTW + 2xEK 
        #ifdef BOARD_KNXLED_DK_06_V10
            // 2x TW
            channelTW[0] = channel[0] = new DimChannel_TW(0);
            uint8_t hwchannel0[] = {0, 1};
            channel[0]->setup(hwchannel0);
            channelTW[1] = channel[1] = new DimChannel_TW(1);
            uint8_t hwchannel1[] = {2, 3};
            channel[1]->setup(hwchannel1);
            // 2x EK
            channelEK[0] = channel[2] = new DimChannel_EK(0);
            uint8_t hwchannel2[] = {4};
            channel[2]->setup(hwchannel2);
            channelEK[1] = channel[3] = new DimChannel_EK(1);
            uint8_t hwchannel3[] = {5};
            channel[3]->setup(hwchannel3);
            //used Channels
            usedChannels = 4;
        #endif
        // 4x TW + 4x EK
        #ifdef BOARD_KNXLED_DK_12_V10
            // 4x TW
            channelTW[0] = channel[0] = new DimChannel_TW(0);
            uint8_t hwchannel0[] = {0, 1};
            channel[0]->setup(hwchannel0);
            channelTW[1] = channel[1] = new DimChannel_TW(1);
            uint8_t hwchannel1[] = {2, 3};
            channel[1]->setup(hwchannel1);
            channelTW[2] = channel[2] = new DimChannel_TW(2);
            uint8_t hwchannel2[] = {4, 5};
            channel[2]->setup(hwchannel2);
            channelTW[3] = channel[3] = new DimChannel_TW(3);
            uint8_t hwchannel3[] = {6, 7};
            channel[3]->setup(hwchannel3);
            // 4x EK
            channelEK[0] = channel[4] = new DimChannel_EK(0);
            uint8_t hwchannel4[] = {8};
            channel[4]->setup(hwchannel4);
            channelEK[1] = channel[5] = new DimChannel_EK(1);
            uint8_t hwchannel5[] = {9};
            channel[5]->setup(hwchannel5);
            channelEK[2] = channel[6] = new DimChannel_EK(2);
            uint8_t hwchannel6[] = {10};
            channel[6]->setup(hwchannel6);
            channelEK[3] = channel[7] = new DimChannel_EK(3);
            uint8_t hwchannel7[] = {11};
            channel[7]->setup(hwchannel7);
            //used Channels
            usedChannels = 8;
        #endif
    }
    break;
    case 3:
    {
        #if defined(BOARD_KNXLED_DK_06_V10)
            usedChannels = 2;   // 2x RGB
        #elif defined(BOARD_KNXLED_DK_12_V10)
            usedChannels = 4;   // 4x RGB
        #endif

        for (uint8_t i = 0; i < usedChannels; ++i)
        {
            channelRGB[i] = channel[i] = new DimChannel_RGB(i);
            uint8_t hwchannel[] = {static_cast<uint8_t>(i * 3), static_cast<uint8_t>(i * 3 + 1), static_cast<uint8_t>(i * 3 + 2)};
            channel[i]->setup(hwchannel);
        }
    }
    break;
    case 4:
    {
        // 1x RGB + 1x TW + 1x EK
        #ifdef BOARD_KNXLED_DK_06_V10
            // 1x EK
            channelEK[0] = channel[0] = new DimChannel_EK(0);
            uint8_t hwchannel0[] = {5};
            channel[0]->setup(hwchannel0);
            // 1x TW
            channelTW[0] = channel[1] = new DimChannel_TW(0);
            uint8_t hwchannel1[] = {3, 4};
            channel[1]->setup(hwchannel1);
            // 1x RGB
            channelRGB[0] = channel[2] = new DimChannel_RGB(0);
            uint8_t hwchannel2[] = {0, 1, 2};
            channel[2]->setup(hwchannel2);
            //used Channels
            usedChannels = 3; 
        #endif  
        // 2x RGB + 2x TW + 2x EK
        #ifdef BOARD_KNXLED_DK_12_V10
            // 2x RGB
            channelRGB[0] = channel[0] = new DimChannel_RGB(0);
            uint8_t hwchannel0[] = {0, 1, 2};
            channel[0]->setup(hwchannel0);
            channelRGB[1] = channel[1] = new DimChannel_RGB(1);
            uint8_t hwchannel1[] = {6, 7, 8};
            channel[1]->setup(hwchannel1);
            // 2x TW
            channelTW[0] = channel[2] = new DimChannel_TW(0);
            uint8_t hwchannel2[] = {3, 4};
            channel[2]->setup(hwchannel2);
            channelTW[1] = channel[3] = new DimChannel_TW(1);
            uint8_t hwchannel3[] = {9, 10};
            channel[3]->setup(hwchannel3);
            // 2x EK
            channelEK[0] = channel[4] = new DimChannel_EK(0);
            uint8_t hwchannel4[] = {5};
            channel[4]->setup(hwchannel4);
            channelEK[1] = channel[5] = new DimChannel_EK(1);
            uint8_t hwchannel5[] = {11};
            channel[5]->setup(hwchannel5);
            //used Channels
            usedChannels = 6; 
        #endif       
    }
    break;
    case 5:
    {
        // 1x RGB + 3x EK
        #ifdef BOARD_KNXLED_DK_06_V10
            //3x EK
            channelEK[0] = channel[0] = new DimChannel_EK(0);
            uint8_t hwchannel0[] = {3};
            channel[0]->setup(hwchannel0);
            channelEK[1] = channel[1] = new DimChannel_EK(1);
            uint8_t hwchannel1[] = {4};
            channel[1]->setup(hwchannel1);
            channelEK[2] = channel[2] = new DimChannel_EK(2);
            uint8_t hwchannel2[] = {5};
            channel[2]->setup(hwchannel2);
            // 1x RGB
            channelRGB[0] = channel[3] = new DimChannel_RGB(0);
            uint8_t hwchannel3[] = {0, 1, 2};
            channel[3]->setup(hwchannel3);
            //used Channels
            usedChannels = 4;
        #endif
        // 1x RGB + 4x TW + 1x EK
        #ifdef BOARD_KNXLED_DK_12_V10
            // 1x RGB            
            channelRGB[0] = channel[0] = new DimChannel_RGB(0);
            uint8_t hwchannel0[] = {0, 1, 2};
            channel[0]->setup(hwchannel0);
            // 4x TW
            channelTW[0] = channel[1] = new DimChannel_TW(0);
            uint8_t hwchannel1[] = {3, 4};
            channel[1]->setup(hwchannel1);
            channelTW[1] = channel[2] = new DimChannel_TW(1);
            uint8_t hwchannel2[] = {6, 7};
            channel[2]->setup(hwchannel2);           
            channelTW[2] = channel[3] = new DimChannel_TW(2);
            uint8_t hwchannel3[] = {8, 9};
            channel[3]->setup(hwchannel3);
            channelTW[3] = channel[4] = new DimChannel_TW(3);
            uint8_t hwchannel4[] = {10, 11};
            channel[4]->setup(hwchannel4);   
            // 1x EK
            channelEK[0] = channel[5] = new DimChannel_EK(0);
            uint8_t hwchannel5[] = {5};
            channel[5]->setup(hwchannel5);
            //used Channels
            usedChannels = 6;
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
            uint8_t sett = ParamAPP_Func1BtnClick;
            handleFunc1(sett);
        });
        openknx.func1Button.onLongClick([=] { 
            logDebugP("Func1 Button - pressed long");
            uint8_t sett = ParamAPP_Func1BtnLongClick;
            handleFunc1(sett);
        });
        openknx.func1Button.onDoubleClick([=] {
            logDebugP("Func1 Button - pressed double");
            uint8_t sett = ParamAPP_Func1BtnDblClick;
            handleFunc1(sett);
        });
    #endif
}

void LEDModule::setup1() 
{
    // Nothing to do on Core 1
}

void LEDModule::loop() 
{
    //check if I2C connection possible, if not reset and init the pwm
    if (delayCheck(_timerCheckI2cConnection, 30000)) {
        checkI2cConnection();
        _timerCheckI2cConnection = millis();
    }
}

void LEDModule::loop1() 
{
    for (int i = 0; i < usedChannels; i++)
        channel[i]->task();
}

// Core function to set value, change if you use other hardware
void LEDModule::setHwChannelValue(byte channel, byte value, int curve) 
{
    _pwm.setPin(channel, curves[value][curve]);
}

void LEDModule::processInputKo(GroupObject &ko) 
{
    uint16_t koNum = ko.asap();
    logDebugP("Received KO %i", koNum);
    
    // EK Dimmer Class
    if (koNum >= EK_KoOffset && koNum < EK_KoOffset + EK_KoBlockSize * MAXCHANNELSEK)
    {
        int channelIndexEK = floor((koNum - EK_KoOffset) / EK_KoBlockSize);
        logDebugP("For Channel EG %i", channelIndexEK);
        channelEK[channelIndexEK]->processInputKo(ko);
        return;
    }
    // TW Dimmer Class
    if (koNum >= TW_KoOffset && koNum < TW_KoOffset + TW_KoBlockSize * MAXCHANNELSTW)
    {
        int channelIndexTW = floor((koNum - TW_KoOffset) / TW_KoBlockSize);
        logDebugP("For Channel TW %i", channelIndexTW);
        channelTW[channelIndexTW]->processInputKo(ko); 
        return;
    }
    // RGB Dimmer Class
    if (koNum >= RGB_KoOffset && koNum < RGB_KoOffset + RGB_KoBlockSize * MAXCHANNELSRGB)
    {
        int channelIndexRGB = floor((koNum - RGB_KoOffset) / RGB_KoBlockSize);
        logDebugP("For Channel RGB %i", channelIndexRGB);
        channelRGB[channelIndexRGB]->processInputKo(ko); 
        return;
    }

    switch(koNum)
    {
        //Diagnose
        case BASE_Share_KoOffset + BASE_KoDiagnose:
            openknx.console.processDiagnoseKo(ko);
            break;

        //Tag/Nacht Objekt
        case APP_KoDayNight:
            koHandleDayNight(ko);
            break;

        default:
            //logDebugP("unhandled KO: %i", ko.asap());
            break;
    }    
}

void LEDModule::koHandleDayNight(GroupObject & ko)
{
    bool value = ko.value(DPT_Switch);
    if (ParamAPP_DayNight) value = !value;
    logDebugP("Broadcast Day/Night %i to channels", value);

    for (int i = 0; i < usedChannels; i++)
        channel[i]->setDayNight(value);
}

void LEDModule::showHelp()
{
    openknx.logger.color(CONSOLE_HEADLINE_COLOR);
    openknx.logger.log("======================== LED Module ============================================");
    openknx.logger.color(0);
    logInfo("chon <ch>", "Switch Channel 0-%i on", LED_HW_CHANNEL_COUNT - 1);
    logInfo("choff <ch>", "Switch Channel 0-%i off", LED_HW_CHANNEL_COUNT - 1);
    logInfo("chval <ch> <value>", "Switch Channel 0-%i to value 0-4095", LED_HW_CHANNEL_COUNT - 1);
}

bool LEDModule::processCommand(const std::string cmd, bool diagnoseKo) 
{
    if (!diagnoseKo && (cmd.rfind("chon ", 0) == 0 || cmd.rfind("choff ", 0) == 0)) {
        _pwm.setPin(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"));
        openknx.logger.logWithPrefixAndValues("CH", "Switch channel %i to %i", std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"));
        return true;
    } else if (!diagnoseKo && cmd.rfind("chval ", 0) == 0) {
        _pwm.setPin(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.substr(cmd.find(' ') + 3)));
        openknx.logger.logWithPrefixAndValues("CH", "Set value channel %i to %i", std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.substr(cmd.find(' ') + 3)));
        return true;
    } else if (cmd == "init_pca") {
        initI2cConnection();
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("INIT PCA"); }
        openknx.logger.logWithPrefixAndValues("LED", "Init PCA9685 i2c connection");
        return true;
    } else if (cmd == "state_pca") {
        Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
        byte result = Wire1.endTransmission();      // 0: Success  1: Data too long  2: NACK on transmit of address  3: NACK on transmit of data  4: Other error  5: Timeout
        byte mode1Value = readRegister(0x00);       // Register - 0x00: MODE1
        byte mode2Value = readRegister(0x01);       // Register - 0x01: MODE2
        if (diagnoseKo) { openknx.console.writeDiagenoseKo("STAT %i %.2X %.2X", result, mode1Value, mode2Value); }
        openknx.logger.logWithPrefixAndValues("LED", "State PCA9685 i2c connection: %i and MODE1: 0x%.2X - MODE2: 0x%.2X", result, mode1Value, mode2Value);
        return true;
    } else if (cmd == "test_pwm") {
        openknx.logger.logWithPrefixAndValues("LED", "Start PCA9685 LED test....");
        // Phasenverschiebung berechnen und einstellen
        for (uint8_t i = 0; i < LED_HW_CHANNEL_COUNT; i++) {
            uint16_t phaseShift = (4096 / LED_HW_CHANNEL_COUNT) * i;
            uint16_t on_time = phaseShift % 4096;
            uint16_t off_time = 4095;
            _pwm.setPWM(i, on_time, off_time);
        }
        openknx.logger.logWithPrefixAndValues("LED", "Finish PCA9685 LED test....");
        return true;
    }
    return false;
}

void LEDModule::processBeforeRestart()
{
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
    Wire1.write(0xFD);  // Adresse des ALL_LED_OFF_H Registers
    Wire1.write(0x10);  // Setze das Bit 4 im ALL_LED_OFF_H Register
    Wire1.endTransmission();
}

// Allgemeine Funktion zum Auslesen eines Registers
byte LEDModule::readRegister(byte registerAddress) 
{
  Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS); // Starte I2C-Übertragung
  Wire1.write(registerAddress); // Registeradresse senden
  Wire1.endTransmission(); // Beende die Übertragung
  
  Wire1.requestFrom(I2C_PCA9685_DEVICE_ADDRESS, (uint8_t)1); // Fordere ein Byte vom Register an
  
  if (Wire1.available()) { // Prüfen, ob Daten empfangen wurden
    return Wire1.read(); // Registerinhalt zurückgeben
  } else {
    return 0xFF; // Fehlerwert zurückgeben, falls keine Daten empfangen wurden
  }
}

void LEDModule::savePower() 
{
    processBeforeRestart();
}

bool LEDModule::initI2cConnection() 
{
    // Call dependend begin for led
    if (!_pwm.begin()) {
        logErrorP("ERROR: initialization for PCA9685 failed...");
        doResetI2c = true;
        return false;
    }
    // Set default values for led
    _pwm.setPWMFreq(pwmFreqSelect);             // 1600 is the maximum PWM frequency
    //_pwm.setOutputMode(true);                   // External N-type driver, set to output mode INVRT = 0 OUTDRV = 1, Totempole (Push-Pull) = true, open drain = false
    logInfoP("Init pwm I2C connection for PCA9685 sucessful");
    doResetI2c = false;
    return true;
}

bool LEDModule::checkI2cConnection() 
{
    if (doResetI2c) { 
        return initI2cConnection();
    }
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
    byte result = Wire1.endTransmission();      //  0: Success  1: Data too long  2: NACK on transmit of address  3: NACK on transmit of data  4: Other error  5: Timeout
    if (result != 0) {
        logErrorP("PCA9685 PWM not available via I2C %d", result);
        openknx.console.writeDiagenoseKo("ER I2C PWM %d", result);
        doResetI2c = true;
        return false;
    }
    return true;
}

#ifdef FUNC1_BUTTON_PIN
void LEDModule::handleFunc1(uint8_t setting)
{
    switch(setting)
    {
        case PT_FuncClickAction_on:
            logDebugP("All channels on");
            for (byte ch = 0; ch < MAXCHANNELSHW; ch++) { _pwm.setPin(ch, 4095); }
            _currentToggleState = 1;
            openknx.info1Led.on();
            break;
        case PT_FuncClickAction_off:
            logDebugP("All channels off");
            for (byte ch = 0; ch < MAXCHANNELSHW; ch++) { _pwm.setPin(ch, 0); }
            _currentToggleState = 0;
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

bool LEDModule::processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) 
{
    if (!knx.configured() || objectIndex != 160 || propertyId != 6)
        return false;

    switch(data[0])
    {
        case 1: // internal function ID
            handleFunctionPropertySwitch(data, resultData, resultLength);
            return true;
        case 2:
            //To-Do
            return true;
    }
    return false;
}

void LEDModule::handleFunctionPropertySwitch(uint8_t *data, uint8_t *resultData, uint8_t &resultLength) 
{
    logInfoP("Function property: LED action switch");
    logIndentUp();
    _pwm.setPin(data[1], data[2] == 1 ? 4095 : 0);
    resultData[0] = 0;
    resultLength = 1;
    logIndentDown();
}

LEDModule openknxLEDModule;
