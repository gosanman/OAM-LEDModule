#include "FrontPanelModule.h"

FrontPanelModule *FrontPanelModule::_instance = nullptr;

FrontPanelModule::FrontPanelModule()
{
    FrontPanelModule::_instance = this;
}

FrontPanelModule *FrontPanelModule::instance()
{
    return FrontPanelModule::_instance;
}

const std::string FrontPanelModule::name()
{
    return "FrontPanel";
}

const std::string FrontPanelModule::version()
{
    return "0.4.0";
}

void FrontPanelModule::setup()
{
    // do nothing when no front panel is present
    if (!ParamAPP_FrontPanelPresent)
        return;

    // save default values from PA
    _menuTimeout = (ParamAPP_DisplayTimeOut * 1000);

    // Init I2C connection and Lib SSD1306
    _display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
    initI2cConnectionLcd();
#ifdef FRONT_PANEL_PRESENT
    pinMode(IO1_PIN, INPUT_PULLUP); // Button back - ■
    pinMode(IO2_PIN, INPUT_PULLUP); // Button left - ◀
    pinMode(IO3_PIN, INPUT_PULLUP); // Button right - ▶
    pinMode(IO4_PIN, INPUT_PULLUP); // Button select - ⬤
#endif
    // Debug
    logDebugP("Timeout: %i sec", _menuTimeout / 1000);
}

void FrontPanelModule::setup1()
{
}

void FrontPanelModule::loop()
{
}

void FrontPanelModule::loop1()
{
    // do nothing when not parameterized or no front panel is present
    if (!knx.configured() || !ParamAPP_FrontPanelPresent)
        return;
#ifdef FRONT_PANEL_PRESENT
    if (!digitalRead(IO1_PIN))
        handleButtonPress(BUTTON_BACK); // Button back pressed   - ■
    if (!digitalRead(IO2_PIN))
        handleButtonPress(BUTTON_LEFT); // Button left pressed   - ◀
    if (!digitalRead(IO3_PIN))
        handleButtonPress(BUTTON_RIGHT); // Button right pressed  - ▶
    if (!digitalRead(IO4_PIN))
        handleButtonPress(BUTTON_SELECT); // Button select pressed - ⬤
#endif
    // Show OpenKNX Logo and welcome message once at startup
    if (startupscreen == true)
    {
        startUpScreen();
    }

    if (delayCheck(_lastScreenUpdate, SCREEN_UPDATE_INTERVAL) && _runScreenUpdate)
    {
        _lastScreenUpdate = millis(); // 250-ms-Throttle auch für den Warn-Pfad einhalten
        // Temp-Sensor nur als Fehler werten, wenn er laut Parametrierung bestückt ist
        bool tempError = ParamAPP_TempSensorPresent && !openknxMeasuringModule.getTempI2cConnectionState();
        if (tempError ||
            !openknxMeasuringModule.getInaI2cConnectionState() ||
            !openknxLEDModule.getPcaI2cConnectionState())
        {
            showWarningScreen();
        }
        else
        {
            updateCurrentScreen();
        }
    }
}

void FrontPanelModule::showHelp()
{
    // do nothing when no front panel is present
    if (!ParamAPP_FrontPanelPresent)
        return;

    openknx.logger.color(CONSOLE_HEADLINE_COLOR);
    openknx.logger.log("======================== FrontPanel Module ===================================");
    openknx.logger.color(0);
    openknx.console.printHelpLine("test char", "Test draw all characters");
    openknx.console.printHelpLine("test pixel", "Test draw pixel");
}

bool FrontPanelModule::processCommand(const std::string cmd, bool diagnoseKo)
{
    if (cmd == "test char")
    {
        _display.clearDisplay();
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(0, 0);
        _display.cp437(true); // Use full 256 char 'Code Page 437' font
        for (int16_t i = 0; i < 256; i++)
        {
            if (i == '\n')
                _display.write(' ');
            else
                _display.write(i);
        }
        _display.display();
        _lastButtonPressed = millis();
        _runScreenUpdate = true;
        return true;
    }
    else if (cmd == "test pixel")
    {
        _display.clearDisplay();
        for (int16_t i = 0; i < SCREEN_WIDTH; i += 2)
        {
            for (int16_t j = 0; j < SCREEN_HEIGHT; j += 2)
            {
                _display.drawPixel(i, j, SSD1306_WHITE);
            }
        }
        _display.display();
        _lastButtonPressed = millis();
        _runScreenUpdate = true;
        return true;
    }
    return false;
}

bool FrontPanelModule::isButtonDebounced(uint8_t button)
{
    uint32_t currentTime = millis();

    if ((currentTime - _lastDebounceTime[button]) <= BUTTON_DEBOUNCE_TIME)
    {
        return false;
    }

    _lastDebounceTime[button] = currentTime;
    _lastButtonPressed = currentTime;
    _runScreenUpdate = true;

    return true;
}

void FrontPanelModule::handleButtonPress(uint8_t button)
{
    if (!isButtonDebounced(button))
        return;

    switch (button)
    {
    case BUTTON_RIGHT:
        if (currentscreen < SCREEN_MAX)
        {
            currentscreen++;
        }
        else if (currentscreen == SUBSCREEN_CONNECTIONS &&
                 currentconnectionscreen < openknxLEDModule.getUsedChannels() - 1)
        {
            currentconnectionscreen++;
        }
        break;

    case BUTTON_LEFT:
        if (currentscreen > 1 && currentscreen != SUBSCREEN_CONNECTIONS)
        {
            currentscreen--;
        }
        else if (currentscreen == SUBSCREEN_CONNECTIONS && currentconnectionscreen > SCREEN_OFF)
        {
            currentconnectionscreen--;
        }
        break;

    case BUTTON_SELECT:
        if (currentscreen == SCREEN_INFORMATION)
        {
            currentscreen = SUBSCREEN_CONNECTIONS;
            currentconnectionscreen = 0;
        }
        else if (currentscreen == SUBSCREEN_CONNECTIONS)
        {
            if (ParamAPP_FrontPanelControl)
                toggleLedChannel(currentconnectionscreen);
        }
        break;

    case BUTTON_BACK:
        currentscreen = SCREEN_INFORMATION;
        break;
    }
}

void FrontPanelModule::updateCurrentScreen()
{
    if ((millis() - _lastButtonPressed) > _menuTimeout)
    { // Screen off
        _display.clearDisplay();
        _display.display();
        currentscreen = SCREEN_OFF;
        _runScreenUpdate = false;
    }
    else if (currentscreen == SCREEN_INFORMATION)
    { // Screen information
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_openknx, 32, 32, 1);
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 0);
        _display.print(DEVICE_ID);
        _display.setCursor(38, 10);
        _display.print(openknx.info.firmwareName().substr(0, 15).c_str());
        _display.setCursor(38, 24);
        _display.print("PA: ");
        _display.print(openknx.info.humanIndividualAddress().c_str());
        _display.display();
    }
    else if (currentscreen == SCREEN_STATUS)
    { // Screen status
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_list_status, 32, 32, 1);
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 0);
        _display.print("Temp: ");
        _display.print(openknxMeasuringModule.getTempI2cConnectionState() ? "OK.." : "Error");
        _display.setCursor(38, 10);
        _display.print("Power: ");
        _display.print(openknxMeasuringModule.getInaI2cConnectionState() ? "OK.." : "Error");
        _display.setCursor(38, 20);
        _display.print("Dimmer: ");
        _display.print(openknxLEDModule.getPcaI2cConnectionState() ? "OK.." : "Error");
        _display.display();
    }
    else if (currentscreen == SCREEN_MEASURING)
    { // Screen measuring values
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_information_box_outline, 32, 32, 1);
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 0);
        _display.print("V: ");
        _display.print(openknxMeasuringModule.getMeasurementValue("voltage"), 2);
        _display.print(" V");
        _display.setCursor(38, 10);
        _display.print("A: ");
        _display.print(openknxMeasuringModule.getMeasurementValue("current"), 2);
        _display.print(" A");
        _display.setCursor(38, 20);
        _display.print("P: ");
        _display.print(openknxMeasuringModule.getMeasurementValue("power"), 2);
        _display.print(" W");
        _display.display();
    }
    else if (currentscreen == SCREEN_TEMPERATURE)
    { // Screen temperature values
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_thermometer_lines, 32, 32, 1);
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 9);
        _display.print(openknxMeasuringModule.getMeasurementValue("temp"), 1);
        _display.print(" ");
        _display.print((char)247);
        _display.print("C");
        _display.display();
    }
    else if (currentscreen == SCREEN_VOLTAGE)
    { // Screen voltage values
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_lightning_bolt_outline, 32, 32, 1);
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 9);
        _display.print(openknxMeasuringModule.getMeasurementValue("voltage"), 1);
        _display.print(" V");
        _display.display();
    }
    else if (currentscreen == SCREEN_CURRENT)
    { // Screen current values
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_current_dc, 32, 32, 1);
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 9);
        _display.print(openknxMeasuringModule.getMeasurementValue("current"), 2);
        _display.print(" A");
        _display.display();
    }
    else if (currentscreen == SCREEN_POWER)
    { // Screen power values
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_gauge, 32, 32, 1);
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 9);
        _display.print(openknxMeasuringModule.getMeasurementValue("power"), 2);
        _display.print(" W");
        _display.display();
    }
    else if (currentscreen == SCREEN_ENERGY)
    { // Screen energy values
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_meter_electric_outline, 32, 32, 1);
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 9);
        _display.print(openknxMeasuringModule.getMeasurementValue("energy"), 2);
        _display.print(" Wh");
        _display.display();
    }
    else if (currentscreen == SCREEN_DATETIME)
    { // Screen datetime values
        _display.clearDisplay();
        _display.drawBitmap(0, 0, bitmap_clock, 32, 32, 1);
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);   
    
        if (openknx.time.isValid())
        {
            auto localTime = openknx.time.getLocalTime();
            char bufferdate[11] = {0};
            char buffertime[6] = {0};
            char bufferup[32] = {0};
            sprintf(bufferdate, "%02d.%02d.%04d", (int)localTime.day, (int)localTime.month, (int)localTime.year);
            sprintf(buffertime, "%02d:%02d", (int)localTime.hour, (int)localTime.minute);
            std::string uptimeStr = openknx.logger.buildUptime();
            sprintf(bufferup, "%s", uptimeStr.c_str());
            _display.setCursor(38, 0);
            _display.print("T: ");
            _display.print(buffertime);
            _display.setCursor(38, 10);
            _display.print("D: ");
            _display.print(bufferdate);
            _display.setCursor(38, 20);
            _display.print("UP: ");
            _display.print(bufferup);
        }
        else
        {
            _display.setCursor(38, 12);
            _display.print("No valid time");
        }
        _display.display();
    }
    else if (currentscreen == SUBSCREEN_CONNECTIONS)
    { // Screen information
        showConnectionScreen(currentconnectionscreen);
    }
}

void FrontPanelModule::startUpScreen()
{
    if (startUpFrame < FRAME_COUNT && delayCheck(_lastStartUpScreen, FRAME_DELAY))
    {
        _display.clearDisplay();
        _display.drawBitmap(0, 0, animation_openknx[startUpFrame], 32, 32, 1);
        _display.setTextSize(2);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 9);
        _display.print("OpenKNX");
        _display.display();
        _lastStartUpScreen = millis();
        startUpFrame++;
    }
    // Deactivate startup screen after last frame
    if (startUpFrame == FRAME_COUNT)
    {
        startupscreen = false;
        _lastButtonPressed = millis();
        _runScreenUpdate = true;
    }
}

void FrontPanelModule::showConnectionScreen(uint8_t index)
{
    _display.clearDisplay();
    _display.drawBitmap(0, 0, bitmap_lightbulb_on_outline, 32, 32, 1);
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(38, 5);
    _display.print("Kanal");
    _display.setCursor(40, 16);
    _display.print(openknxLEDModule.getChannelName(index).c_str());
    _display.print(openknxLEDModule.getChannelIndex(index) + 1);
    std::vector<uint8_t> ports = openknxLEDModule.getChannelHWPort(index);
    uint8_t numberOfPorts = ports.size();
    if (numberOfPorts == 1)
    {
        _display.setCursor(78, 2);
        _display.print("EK");
        _display.print(openknxLEDModule.getChannelIndex(index) + 1);
        _display.print(" - ");
        _display.print(HWPortsMapping[ports[0]]);
    }
    else if (numberOfPorts == 2)
    {
        _display.setCursor(78, 2);
        _display.print("WW");
        _display.print(openknxLEDModule.getChannelIndex(index) + 1);
        _display.print(" - ");
        _display.print(HWPortsMapping[ports[0]]);
        _display.setCursor(78, 13);
        _display.print("KW");
        _display.print(openknxLEDModule.getChannelIndex(index) + 1);
        _display.print(" - ");
        _display.print(HWPortsMapping[ports[1]]);
    }
    else if (numberOfPorts == 3)
    {
        _display.setCursor(78, 2);
        _display.print("R");
        _display.print(openknxLEDModule.getChannelIndex(index) + 1);
        _display.print(" - ");
        _display.print(HWPortsMapping[ports[0]]);
        _display.setCursor(78, 13);
        _display.print("G");
        _display.print(openknxLEDModule.getChannelIndex(index) + 1);
        _display.print(" - ");
        _display.print(HWPortsMapping[ports[1]]);
        _display.setCursor(78, 24);
        _display.print("B");
        _display.print(openknxLEDModule.getChannelIndex(index) + 1);
        _display.print(" - ");
        _display.print(HWPortsMapping[ports[2]]);
    }
    _display.display();
}

void FrontPanelModule::showWarningScreen()
{
        _display.clearDisplay();
        // Icon blinken lassen
        if (millis() - _warnLastBlinkTime >= BLINK_INTERVAL) {
            _warnBlinkOn = !_warnBlinkOn;
            _warnLastBlinkTime = millis();
        }
        if (_warnBlinkOn) {
            _display.drawBitmap(0, 0, bitmap_alert_outline, 32, 32, 1);
        }
        _display.setTextSize(1);
        _display.setTextColor(SSD1306_WHITE);
        _display.setCursor(38, 0);
        _display.print("Temp: ");
        _display.print(openknxMeasuringModule.getTempI2cConnectionState() ? "OK.." : "Error");
        _display.setCursor(38, 10);
        _display.print("Power: ");
        _display.print(openknxMeasuringModule.getInaI2cConnectionState() ? "OK.." : "Error");
        _display.setCursor(38, 20);
        _display.print("Dimmer: ");
        _display.print(openknxLEDModule.getPcaI2cConnectionState() ? "OK.." : "Error");
        _display.display();
}

void FrontPanelModule::toggleLedChannel(uint8_t index)
{
    logDebugP("Toggle Channel: %i", index);
    std::vector<uint8_t> ports = openknxLEDModule.getChannelHWPort(index);
    for (uint8_t i = 0; i < ports.size(); i++)
    {
        openknxLEDModule.toggleChannelHWPort(ports[i]);
    }
}

bool FrontPanelModule::initI2cConnectionLcd()
{
    if (!_display.begin(SSD1306_SWITCHCAPVCC, I2C_SSD1306_DEVICE_ADDRESS))
    {
        logErrorP("ERROR: initialization for SSD1306 failed...");
        return false;
    }
    // Clear the buffer.
    _display.clearDisplay();
    _display.display();
    logInfoP("Init oled I2C connection for SSD1306 sucessful");
    openknx.console.writeDiagenoseKo("OK OLED INIT");
    return true;
}

FrontPanelModule openknxFrontPanelModule;