#include "LEDModule.h"

#include "DimChannel_EK.h"
#include "DimChannel_TW.h"
#include "DimChannel_RGB.h"
#include "DimChannel.h"
#include "MeasuringModule.h" // für readCurrentNow() im Testmodus

LEDModule *LEDModule::_instance = nullptr;

LEDModule::LEDModule()
{
    LEDModule::_instance = this;
}

LEDModule *LEDModule::instance()
{
    return LEDModule::_instance;
}

const std::string LEDModule::name()
{
    return "LED";
}

const std::string LEDModule::version()
{
    return "0.4.0";
}

void LEDModule::setup()
{
    // save default values from PA
    deviceSelect = ParamAPP_ControllerType;
    pwmFreqSelect = ParamAPP_PwmFrequenz;

    switch (deviceSelect)
    {
    case 0: // BOARD_KNXLED_DK_06_V10 or V12 - LED-DK-06x24V
        operatinModeSelect = ParamAPP_OperatingMode;
        logInfoP("Device: %sx24V - 6-Kanal OpenKNX LED Dimmer", DEVICE_ID);
        break;
    case 1: // BOARD_KNXLED_DK_12_V10 or V12 - LED-DK-12x24V
        operatinModeSelect = ParamAPP_OperatingMode - 10;
        logInfoP("Device: %sx24V - 12-Kanal OpenKNX LED Dimmer", DEVICE_ID);
        break;
    }

    // Debug
    logDebugP("CONFIG - Controller Device: %s (%i) - Operating Mode: %i - PWM freq: %i Hz - DayNight: %i",
              DEVICE_ID, deviceSelect, operatinModeSelect, pwmFreqSelect, ParamAPP_DayNight);

    // Init I2C connection and Lib
    _pwm = Adafruit_PWMServoDriver(I2C_PCA9685_DEVICE_ADDRESS, Wire1);
    initI2cConnection();

    // Set all Channels off for a defined status
    for (byte ch = 0; ch < MAXCHANNELSHW; ch++)
    {
        _pwm.setPin(ch, 0);
    }

    // Create dimmer groups based on the parameter PT_OperationMode
    // Pin 0 = A    Pin  6 = G
    // Pin 1 = B    Pin  7 = H            DK-12x24V               DK-12x24V               DK-06x24V            DK-06x24V           RG-06x24V
    // Pin 2 = C    Pin  8 = I      _____________________   _______________________                                             
    // Pin 3 = D    Pin  9 = J      | V+ V+ G H I J K L |   | V+ V+ 6 7 8 9 10 11 |   __________________   __________________   _______________
    // Pin 4 = E    Pin 10 = K      | V+ V+ A B C D E F |   | V+ V+ 0 1 2 3  4  5 |   | V+ A B C D E F |   | V+ 0 1 2 3 4 5 |   | F E D C B A |
    // Pin 5 = F    Pin 11 = L      =====================   =======================   ==================   ==================   ===============

    switch (operatinModeSelect)
    {
    case 0:
    {
#if (LED_HW_CHANNEL_COUNT == 6)
        usedChannels = 6; // 6x EK
#elif (LED_HW_CHANNEL_COUNT == 12)
        usedChannels = 12; // 12x EK
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
#if (LED_HW_CHANNEL_COUNT == 6)
        usedChannels = 3; // 3x TW
#elif (LED_HW_CHANNEL_COUNT == 12)
        usedChannels = 6; // 6x TW
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
#if (LED_HW_CHANNEL_COUNT == 6)
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
        // used Channels
        usedChannels = 4;
#endif
// 4x TW + 4x EK
#if (LED_HW_CHANNEL_COUNT == 12)
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
        // used Channels
        usedChannels = 8;
#endif
    }
    break;
    case 3:
    {
#if (LED_HW_CHANNEL_COUNT == 6)
        usedChannels = 2; // 2x RGB
#elif (LED_HW_CHANNEL_COUNT == 12)
        usedChannels = 4; // 4x RGB
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
#if (LED_HW_CHANNEL_COUNT == 6)
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
        // used Channels
        usedChannels = 3;
#endif
// 2x RGB + 2x TW + 2x EK
#if (LED_HW_CHANNEL_COUNT == 12)
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
        // used Channels
        usedChannels = 6;
#endif
    }
    break;
    case 5:
    {
// 1x RGB + 3x EK
#if (LED_HW_CHANNEL_COUNT == 6)
        // 3x EK
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
        // used Channels
        usedChannels = 4;
#endif
// 1x RGB + 4x TW + 1x EK
#if (LED_HW_CHANNEL_COUNT == 12)
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
        // used Channels
        usedChannels = 6;
#endif
    }
    break;
    default:
        logErrorP("Operation Mode not valide");
        break;
    }

    // Set the default value for the HCL channels
    for (int i = 0; i < MAXCHANNELSHCL; i++)
    {
        hclchannel[i] = new HclChannel();
        hclchannel[i]->setup(i);
    }

// ▲ - Symbol for Program Button
// ••• - Symbol for Func1 Button
#ifdef FUNC1_BUTTON_PIN
    openknx.func1Button.onShortClick([=]
                                     { 
            logDebugP("Func1 Button - pressed short");
            uint8_t sett = ParamAPP_Func1BtnClick;
            handleFunc1(sett); });
    openknx.func1Button.onLongClick([=]
                                    { 
            logDebugP("Func1 Button - pressed long");
            uint8_t sett = ParamAPP_Func1BtnLongClick;
            handleFunc1(sett); });
    openknx.func1Button.onDoubleClick([=]
                                      {
            logDebugP("Func1 Button - pressed double");
            uint8_t sett = ParamAPP_Func1BtnDblClick;
            handleFunc1(sett); });
#endif
}

void LEDModule::setup1()
{
}

void LEDModule::loop()
{
    // do nothing when not parameterized
    if (!knx.configured())
        return;
    // run loop of all HCL channels
    if (delayCheck(_timerCheckHclChannel, HCL_TIMER_BROADCAST))
    {
        for (int ch = 0; ch < MAXCHANNELSHCL; ch++)
        {
            // nur broadcasten, wenn der HCL-Kanal gültige Werte geliefert hat
            // (sonst würden 0-/Vorgänger-Werte eingeschaltete Kanäle dunkeldimmen)
            if (!hclchannel[ch]->loop(hclKelvin, hclBrightness))
                continue;
            logDebugP("Broadcast values from HCL%i", ch+1);
            for (int i = 0; i < usedChannels; i++)
            {
                channel[i]->setHcl(ch, hclKelvin, hclBrightness);
            }
        }
        _timerCheckHclChannel = millis();
    }
}

void LEDModule::loop1()
{
    // do nothing when not parameterized
    if (!knx.configured())
        return;
    // check if I2C connection possible, if not reset and init the pwm
    if (delayCheck(_timerCheckI2cConnection, LED_CHECK_I2C)) 
    {
        checkI2cConnection();
        _timerCheckI2cConnection = millis();
    }
    // Testmodus-Zustandsmaschine (nicht blockierend)
    testLoop();
    // run task of all channels if pca connection ok, no power fault and no test running
    if (pcaI2cConnection && !_powerFault && !_testActive)
    {
        for (int i = 0; i < usedChannels; i++)
            channel[i]->task();
    }
}

// Core function to set value, change if you use other hardware
void LEDModule::setHwChannelValue(byte channel, byte value, int curve)
{
    _pwm.setPin(channel, curves[value][curve]);
}
void LEDModule::setHwChannelValuePWM(byte channel, word start, word end, int curve)
{
    _pwm.setPWM(channel, start, end);
}

void LEDModule::processInputKo(GroupObject &ko)
{
    uint16_t koNum = ko.asap();
    if (koNum < EK_KoOffset && koNum != APP_KoDayNight) return; // ignore KOs below EK block, außer gemeinsame KOs (Tag/Nacht)
    logDebugP("Received KO %i", koNum);

    // Im Latch-Zustand löst ein Kanalbefehl (Schalten/Dimmen) einen strom-geprüften
    // Reaktivierungsversuch aus; die eigentliche Ausführung erfolgt in loop1 (Core 1).
    if (_powerFault && koNum >= EK_KoOffset)
        requestReactivation();

    // EK Dimmer Class
    if (koNum >= EK_KoOffset && koNum < EK_KoOffset + EK_KoBlockSize * MAXCHANNELSEK)
    {
        int channelIndexEK = (koNum - EK_KoOffset) / EK_KoBlockSize; // ganzzahlige Division, kein float floor()
        logDebugP("For Channel EG %i", channelIndexEK);
        if (channelEK[channelIndexEK] == nullptr) return; // Slot in dieser Betriebsart nicht belegt
        channelEK[channelIndexEK]->processInputKo(ko);
        return;
    }
    // TW Dimmer Class
    if (koNum >= TW_KoOffset && koNum < TW_KoOffset + TW_KoBlockSize * MAXCHANNELSTW)
    {
        int channelIndexTW = (koNum - TW_KoOffset) / TW_KoBlockSize;
        logDebugP("For Channel TW %i", channelIndexTW);
        if (channelTW[channelIndexTW] == nullptr) return; // Slot in dieser Betriebsart nicht belegt
        channelTW[channelIndexTW]->processInputKo(ko);
        return;
    }
    // RGB Dimmer Class
    if (koNum >= RGB_KoOffset && koNum < RGB_KoOffset + RGB_KoBlockSize * MAXCHANNELSRGB)
    {
        int channelIndexRGB = (koNum - RGB_KoOffset) / RGB_KoBlockSize;
        logDebugP("For Channel RGB %i", channelIndexRGB);
        if (channelRGB[channelIndexRGB] == nullptr) return; // Slot in dieser Betriebsart nicht belegt
        channelRGB[channelIndexRGB]->processInputKo(ko);
        return;
    }

    switch (koNum)
    {
    // Tag/Nacht Objekt
    case APP_KoDayNight:
        koHandleDayNight(ko);
        break;

    default:
        // logDebugP("unhandled KO: %i", ko.asap());
        break;
    }
}

void LEDModule::koHandleDayNight(GroupObject &ko)
{
    bool value = ko.value(DPT_Switch);
    if (ParamAPP_DayNight)
        value = !value;
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
    openknx.console.printHelpLine("show con", "Show connection plan on console");
    openknx.console.printHelpLine("i2c", "Scan I2C devices on Wire1");
    openknx.console.printHelpLine("resetfault", "Reset latched over-current fault (current-checked)");
    openknx.console.printHelpLine("test walk", "Channel walk-through test (auto), measures current per port");
    openknx.console.printHelpLine("test ch <n>", "Test single HW port at 100 percent");
    openknx.console.printHelpLine("test next/prev/stop", "Advance / previous port / end test mode");
}

bool LEDModule::processCommand(const std::string cmd, bool diagnoseKo)
{
    if (cmd == "resetfault")
    {
        requestReactivation(); // strom-geprüfter Reaktivierungsversuch (Ausführung in loop1)
        if (diagnoseKo)
            openknx.console.writeDiagenoseKo("FAULT RST");
        openknx.logger.logWithPrefixAndValues("LED", "Over-current fault reset requested (fault=%i)", isPowerFault());
        return true;
    }
    // Testmodus-Kommandos (Konsole und Diagnose-KO)
    if (cmd == "test walk") { testStart(true); openknx.console.writeDiagenoseKo("T WALK"); return true; }
    if (cmd == "test next") { testNext();      openknx.console.writeDiagenoseKo("T NEXT"); return true; }
    if (cmd == "test prev") { testPrev();      openknx.console.writeDiagenoseKo("T PREV"); return true; }
    if (cmd == "test stop") { testStop();      openknx.console.writeDiagenoseKo("T STOP"); return true; }
    if (cmd.rfind("test ch ", 0) == 0)
    {
        const char *s = cmd.c_str() + 8;
        char *end = nullptr;
        long n = strtol(s, &end, 10); // sicher parsen (kein std::stoi -> keine Exception)
        if (end != s && n >= 0 && n < LED_HW_CHANNEL_COUNT)
        {
            testGoTo((uint8_t)n);
            openknx.console.writeDiagenoseKo("T CH %i", (int)n);
        }
        else
            openknx.console.writeDiagenoseKo("T CH ERR");
        return true;
    }
    // Direkt-PWM-Konsolenbefehle im Fehler-Latch oder Testmodus sperren
    if ((_powerFault || _testActive) && !diagnoseKo &&
        (cmd.rfind("chon ", 0) == 0 || cmd.rfind("choff ", 0) == 0 || cmd.rfind("chval ", 0) == 0 || cmd == "test_pwm"))
    {
        openknx.logger.logWithPrefixAndValues("LED", "Blocked - over-current fault or test mode active");
        return true;
    }
    if (!diagnoseKo && (cmd.rfind("chon ", 0) == 0 || cmd.rfind("choff ", 0) == 0))
    {
        _pwm.setPin(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"));
        openknx.logger.logWithPrefixAndValues("CH", "Switch channel %i to %i", std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.rfind("chon", 0) == 0 ? "4095" : "0"));
        return true;
    }
    else if (!diagnoseKo && cmd.rfind("chval ", 0) == 0)
    {
        _pwm.setPin(std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.substr(cmd.find(' ') + 3)));
        openknx.logger.logWithPrefixAndValues("CH", "Set value channel %i to %i", std::stoi(cmd.substr(cmd.find(' ') + 1)), std::stoi(cmd.substr(cmd.find(' ') + 3)));
        return true;
    }
    else if (cmd == "init_pca")
    {
        initI2cConnection();
        if (diagnoseKo)
        {
            openknx.console.writeDiagenoseKo("INIT PCA");
        }
        openknx.logger.logWithPrefixAndValues("LED", "Init PCA9685 i2c connection");
        return true;
    }
    else if (cmd == "state_pca")
    {
        Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
        byte result = Wire1.endTransmission(); // 0: Success  1: Data too long  2: NACK on transmit of address  3: NACK on transmit of data  4: Other error  5: Timeout
        byte mode1Value = readRegister(0x00);  // Register - 0x00: MODE1
        byte mode2Value = readRegister(0x01);  // Register - 0x01: MODE2
        if (diagnoseKo)
        {
            openknx.console.writeDiagenoseKo("STAT %i %.2X %.2X", result, mode1Value, mode2Value);
        }
        openknx.logger.logWithPrefixAndValues("LED", "State PCA9685 i2c connection: %i and MODE1: 0x%.2X - MODE2: 0x%.2X", result, mode1Value, mode2Value);
        return true;
    }
    else if (cmd == "test_pwm")
    {
        openknx.logger.logWithPrefixAndValues("LED", "Start PCA9685 LED test....");
        // Phasenverschiebung berechnen und einstellen
        for (uint8_t i = 0; i < LED_HW_CHANNEL_COUNT; i++)
        {
            uint16_t phaseShift = (4096 / LED_HW_CHANNEL_COUNT) * i;
            uint16_t on_time = phaseShift % 4096;
            uint16_t off_time = 4095;
            _pwm.setPWM(i, on_time, off_time);
            openknx.logger.logWithPrefixAndValues("LED", "Set Channel %i - ON: %i - OFF: %i", i, on_time, off_time);
        }
        openknx.logger.logWithPrefixAndValues("LED", "Finish PCA9685 LED test....");
        return true;
    }
    else if (cmd == "show con")
    {
        for (uint8_t i = 0; i < getUsedChannels(); ++i)
        {
            openknx.logger.logWithPrefixAndValues("LED", "Channel %d", i);
            openknx.logger.logWithPrefixAndValues("LED", "Name: %s%d", getChannelName(i).c_str(), getChannelIndex(i) + 1);
            uint8_t ports[3];
            uint8_t numberOfPorts = getChannelHWPort(i, ports);
            openknx.logger.logWithPrefixAndValues("LED", "HWPorts: %d", numberOfPorts);
            if (numberOfPorts == 1)
            {
                openknx.logger.logWithPrefixAndValues("LED", "EK%d -> %c", getChannelIndex(i) + 1, HWPortsMapping[ports[0]]);
            }
            else if (numberOfPorts == 2)
            {
                openknx.logger.logWithPrefixAndValues("LED", "WW%d -> %c", getChannelIndex(i) + 1, HWPortsMapping[ports[0]]);
                openknx.logger.logWithPrefixAndValues("LED", "KW%d -> %c", getChannelIndex(i) + 1, HWPortsMapping[ports[1]]);
            }
            else if (numberOfPorts == 3)
            {
                openknx.logger.logWithPrefixAndValues("LED", "R%d -> %c", getChannelIndex(i) + 1, HWPortsMapping[ports[0]]);
                openknx.logger.logWithPrefixAndValues("LED", "G%d -> %c", getChannelIndex(i) + 1, HWPortsMapping[ports[1]]);
                openknx.logger.logWithPrefixAndValues("LED", "B%d -> %c", getChannelIndex(i) + 1, HWPortsMapping[ports[2]]);
            }
            openknx.logger.logWithPrefixAndValues("LED", "--------------------");
        }
        return true;
    }
    else if (cmd == "i2c")
    {
        byte error, address;
        int nDevices;
        logInfoP("Scanning...");
        nDevices = 0;
        for (address = 1; address < 127; address++)
        {
            Wire1.beginTransmission(address);
            error = Wire1.endTransmission();
            if (error == 0)
            {
                switch (address)
                {
                    case I2C_SSD1306_DEVICE_ADDRESS:
                        logInfoP("SD1306 OLED display found at address 0x%.2X", address);
                        break;
                    case I2C_PCA9685_DEVICE_ADDRESS:
                        logInfoP("PCA9685 PWM chip found at address 0x%.2X", address);
                        break;
                    case I2C_INA22x_DEVICE_ADDRESS:
                        logInfoP("INA226/228 current and power sensor found at address 0x%.2X", address);
                        break;
                    case I2C_TMP100_DEVICE_ADDRESS:
                        logInfoP("TMP100 temperature sensor found at address 0x%.2X", address);
                        break;
                    default:
                        logInfoP("Unknown device found at address 0x%.2X", address);
                        break;
                }
                nDevices++;
            }
            else if (error == 4)
            {
                logInfoP("Unknown error at address 0x%.2X", address);
            }
        }
        if (nDevices == 0)
            logInfoP("No I2C devices found");
        else
            logInfoP("done");
        return true;
    }
    return false;
}

void LEDModule::processBeforeRestart()
{
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
    Wire1.write(0xFD); // Adresse des ALL_LED_OFF_H Registers
    Wire1.write(0x10); // Setze das Bit 4 im ALL_LED_OFF_H Register
    Wire1.endTransmission();
}

// Allgemeine Funktion zum Auslesen eines Registers
byte LEDModule::readRegister(byte registerAddress)
{
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS); // Starte I2C-Übertragung
    Wire1.write(registerAddress);                        // Registeradresse senden
    Wire1.endTransmission();                             // Beende die Übertragung

    Wire1.requestFrom(I2C_PCA9685_DEVICE_ADDRESS, (uint8_t)1); // Fordere ein Byte vom Register an

    if (Wire1.available())
    {                        // Prüfen, ob Daten empfangen wurden
        return Wire1.read(); // Registerinhalt zurückgeben
    }
    else
    {
        return 0xFF; // Fehlerwert zurückgeben, falls keine Daten empfangen wurden
    }
}

void LEDModule::savePower()
{
    processBeforeRestart(); // ALL_LED_OFF -> alle Ausgänge sofort aus
    _powerFault = true;     // harter Latch: verhindert Neubestromen über channel->task() und Direktpfade
}

// Latch lösen und die Kanäle aus ihrem SOFTWARE-Zustand neu schreiben.
// Kein Löschen des ALL_LED_OFF-Broadcast-Registers (das würde bei jedem Kanal die
// oberen OFF-Bits verfälschen) - stattdessen liefert jeder Kanal seinen aktuellen
// Wert erneut per sendDimValue() aus, was die LEDn-Register korrekt überschreibt.
void LEDModule::resendChannels()
{
    for (int i = 0; i < usedChannels; i++)
        channel[i]->resend();
}

void LEDModule::clearFaultAndResend()
{
    _powerFault = false;
    resendChannels();
}

void LEDModule::requestReactivation()
{
    if (_powerFault)
        _reactivationRequested = true;
}

bool LEDModule::consumeReactivationRequest()
{
    bool r = _reactivationRequested;
    _reactivationRequested = false;
    return r;
}

bool LEDModule::isPowerFault()
{
    return _powerFault;
}

// ------------------------------ Testmodus ------------------------------
// Anforderungen werden von Core 0 (Konsole/Diagnose) oder dem Display gesetzt und in
// testLoop() auf Core 1 ausgeführt, damit alle I2C-Zugriffe auf einem Core bleiben.
void LEDModule::testStart(bool autoAdvance) { _testReq = autoAdvance ? 2 : 1; }
void LEDModule::testStop()                  { _testReq = 5; }
void LEDModule::testNext()                  { _testReq = 3; }
void LEDModule::testPrev()                  { _testReq = 4; }
void LEDModule::testGoTo(uint8_t port)      { _testReqPort = port; _testReq = 6; }
bool LEDModule::isTestActive()              { return _testActive; }
uint8_t LEDModule::testPort()               { return _testPort; }
float LEDModule::testCurrent()              { return _testCurrentA; }

void LEDModule::testEnterPort(uint8_t port)
{
    for (uint8_t p = 0; p < LED_HW_CHANNEL_COUNT; p++)
        _pwm.setPin(p, 0);
    _testPort = (port < LED_HW_CHANNEL_COUNT) ? port : 0;
    _pwm.setPin(_testPort, 4095); // 100 % (Volllast)
    _testPhase = 0;
    _testStepStart = millis();
    _testCurrentA = -1.0f;
}

void LEDModule::testLoop()
{
    // Anforderung abholen (Core 0 / Display)
    uint8_t req = _testReq;
    _testReq = 0;
    switch (req)
    {
    case 1: // start manuell
    case 2: // start auto
        _testActive = true;
        _testAuto = (req == 2);
        _testLastActivity = millis();
        testEnterPort(0);
        logInfoP("Test mode started (%s)", _testAuto ? "auto" : "manual");
        break;
    case 3: // next
        if (_testActive) { _testLastActivity = millis(); testEnterPort((_testPort + 1) % LED_HW_CHANNEL_COUNT); }
        break;
    case 4: // prev
        if (_testActive) { _testLastActivity = millis(); testEnterPort((_testPort + LED_HW_CHANNEL_COUNT - 1) % LED_HW_CHANNEL_COUNT); }
        break;
    case 5: // stop
        if (_testActive)
        {
            _testActive = false;
            for (uint8_t p = 0; p < LED_HW_CHANNEL_COUNT; p++) _pwm.setPin(p, 0);
            resendChannels(); // Normalbetrieb wiederherstellen
            logInfoP("Test mode stopped");
        }
        break;
    case 6: // Einzeltest eines Ports (startet den Test)
        if (_testReqPort >= 0 && _testReqPort < LED_HW_CHANNEL_COUNT)
        {
            _testActive = true;
            _testAuto = false;
            _testLastActivity = millis();
            testEnterPort((uint8_t)_testReqPort);
        }
        _testReqPort = -1;
        break;
    }

    if (!_testActive)
        return;

    // Sicherheit: bei Fehler-Latch Test sofort beenden (savePower hat bereits alles aus)
    if (_powerFault)
    {
        _testActive = false;
        logErrorP("Test mode aborted - over-current latched");
        return;
    }
    // Auto-Ende nach Inaktivität
    if (delayCheck(_testLastActivity, TEST_TIMEOUT_MS))
    {
        _testActive = false;
        for (uint8_t p = 0; p < LED_HW_CHANNEL_COUNT; p++) _pwm.setPin(p, 0);
        resendChannels();
        logInfoP("Test mode timeout - back to normal");
        return;
    }

    // Settle-Zeit abgelaufen -> echten Strom messen und melden
    if (_testPhase == 0 && delayCheck(_testStepStart, TEST_SETTLE_MS))
    {
        _testCurrentA = openknxMeasuringModule.readCurrentNow();
        _testPhase = 1;
        logInfoP("Test port %c (%i): %.2f A", HWPortsMapping[_testPort], _testPort, _testCurrentA);
        openknx.console.writeDiagenoseKo("T %c %.2fA", HWPortsMapping[_testPort], _testCurrentA);
    }
    // Auto-Weiterschalten nach Verweildauer
    if (_testAuto && _testPhase == 1 && delayCheck(_testStepStart, TEST_DWELL_MS))
    {
        _testLastActivity = millis();
        testEnterPort((_testPort + 1) % LED_HW_CHANNEL_COUNT);
    }
}

bool LEDModule::initI2cConnection()
{
    // Call dependend begin for led
    if (!_pwm.begin())
    {
        logErrorP("ERROR: initialization for PCA9685 failed...");
        openknx.console.writeDiagenoseKo("ER PWM INIT");
        doResetI2c = true;
        pcaI2cConnection = false;
#ifdef INFO2_LED_PIN  
        openknx.info2Led.on();
#endif
        return false;
    }
    // Set default values for led
    _pwm.setPWMFreq(pwmFreqSelect); // 1600 is the maximum PWM frequency
    _pwm.setOutputMode(true);       // External N-type driver, set to output mode INVRT = 0 OUTDRV = 1, Totempole (Push-Pull) = true, open drain = false
    logInfoP("Init pwm I2C connection for PCA9685 sucessful");
    openknx.console.writeDiagenoseKo("OK PWM INIT");
    doResetI2c = false;
    pcaI2cConnection = true;
#ifdef INFO2_LED_PIN  
        openknx.info2Led.off();
#endif
    return true;
}

bool LEDModule::checkI2cConnection()
{
    if (doResetI2c) 
    {
        return initI2cConnection();
    }
    Wire1.beginTransmission(I2C_PCA9685_DEVICE_ADDRESS);
    byte result = Wire1.endTransmission(); // 0: Success  1: Data too long  2: NACK on transmit of address  3: NACK on transmit of data  4: Other error  5: Timeout
    byte mode1Value = readRegister(0x00);  // Register - 0x00: MODE1 -> 0x20
    byte mode2Value = readRegister(0x01);  // Register - 0x01: MODE2 -> 0x04 -> Totempole (Push-Pull)
    if (result != 0 || mode1Value != 0x20 || (mode2Value & 0x04) != 0x04)
    {
        logErrorP("PCA9685 PWM not available via I2C - State: %i and MODE1: 0x%.2X - MODE2: 0x%.2X", result, mode1Value, mode2Value);
        openknx.console.writeDiagenoseKo("ER PWM %i %.2X %.2X", result, mode1Value, mode2Value);
        doResetI2c = true;
        pcaI2cConnection = false; // Verbindung als weg markieren -> loop1 sperrt Kanal-Tasks, Frontpanel zeigt Fehler
        return false;
    }
    return true;
}

bool LEDModule::getPcaI2cConnectionState()
{
    return pcaI2cConnection;
}

uint8_t LEDModule::getUsedChannels()
{
    return usedChannels;
}

const std::string LEDModule::getChannelName(uint8_t channelIndex)
{
    if (channelIndex < usedChannels)
    {
        return channel[channelIndex]->name();
    }
    return "";
}

uint8_t LEDModule::getChannelHWPort(uint8_t channelIndex, uint8_t *ports)
{
    if (channelIndex < usedChannels)
        return channel[channelIndex]->getHWPorts(ports);
    return 0;
}

void LEDModule::toggleChannelHWPort(uint8_t channel)
{
    if (_powerFault) return; // im Fehler-Latch keine Ausgänge direkt bestromen
    if (channel < LED_HW_CHANNEL_COUNT) // Parameter ist eine HW-Portnummer (0..N-1), nicht der logische Kanal
    {
        // OFF-Register (Pegel) lesen, nicht den ON-Zähler; gesetztes Voll-AUS-Bit (>=0x1000) => Port ist aus
        bool isOff = (_pwm.getPWM(channel, true) >= 4096);
        logDebugP("  HW Port: %i -> %s", channel, isOff ? "on" : "off");
        _pwm.setPin(channel, isOff ? 4095 : 0);
    }
}

uint8_t LEDModule::getChannelIndex(uint8_t channelIndex)
{
    if (channelIndex < usedChannels)
    {
        return channel[channelIndex]->getChannelIndex();
    }
    return 0;
}

#ifdef FUNC1_BUTTON_PIN
void LEDModule::handleFunc1(uint8_t setting)
{
    if (_powerFault) return; // im Fehler-Latch keine Ausgänge direkt bestromen
    switch (setting)
    {
    case PT_FuncClickAction_on:
        logDebugP("All channels on");
        for (byte ch = 0; ch < MAXCHANNELSHW; ch++)
        {
            _pwm.setPin(ch, 4095);
        }
        _currentToggleState = 1;
        openknx.info1Led.on();
        break;
    case PT_FuncClickAction_off:
        logDebugP("All channels off");
        for (byte ch = 0; ch < MAXCHANNELSHW; ch++)
        {
            _pwm.setPin(ch, 0);
        }
        _currentToggleState = 0;
        openknx.info1Led.off();
        break;
    case PT_FuncClickAction_toggle:
        _currentToggleState = !_currentToggleState;
        logDebugP("Toggle %i all Channels", _currentToggleState);
        for (byte ch = 0; ch < MAXCHANNELSHW; ch++)
        {
            _pwm.setPin(ch, _currentToggleState == 0 ? 0 : 4095);
        }
        if (_currentToggleState == 0)
        {
            openknx.info1Led.off();
        }
        else if (_currentToggleState == 1)
        {
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

    switch (data[0])
    {
    case 1: // internal function ID
        handleFunctionPropertySwitch(data, resultData, resultLength);
        return true;
    case 2:
        // noch nicht implementiert: definierte, leere Antwort setzen, damit der
        // KNX-BAU nicht bis zu 255 Byte uninitialisierten resultData zurücksendet
        resultData[0] = 0;
        resultLength = 1;
        return true;
    }
    return false;
}

void LEDModule::handleFunctionPropertySwitch(uint8_t *data, uint8_t *resultData, uint8_t &resultLength)
{
    logInfoP("Function property: LED action switch");
    logIndentUp();
    if (_powerFault) // im Fehler-Latch keine Ausgänge direkt bestromen
    {
        resultData[0] = 0;
        resultLength = 1;
        logIndentDown();
        return;
    }
    _pwm.setPin(data[1], data[2] == 1 ? 4095 : 0);
    resultData[0] = 0;
    resultLength = 1;
    logIndentDown();
}

LEDModule openknxLEDModule;
