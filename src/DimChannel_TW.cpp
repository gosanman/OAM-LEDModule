#include "DimChannel_TW.h"

DimChannel_TW::DimChannel_TW(uint8_t index) : DimChannel(index)
{
    _index = index;
}

DimChannel_TW::~DimChannel_TW() {}

const std::string DimChannel_TW::name()
{
    return "TW";
}

void DimChannel_TW::setup(uint8_t *hwchannel)
{
    // Parameter
    m_hwchannel_ww = hwchannel[0];
    m_hwchannel_cw = hwchannel[1];

    m_colortempww = ParamTW_ColorTempWW;
    m_colortempcw = ParamTW_ColorTempKW;
    m_usedayvalue = ParamTW_UseOnValue;
    m_daybrightness = round(ParamTW_OnBrightness * 2.55);
    m_daycolortemp = ParamTW_OnColorTemp;
    m_usenightvalue = ParamTW_UseNightValue;
    m_nightbrightness = round(ParamTW_NightBrightness * 2.55);
    m_nightcolortemp = ParamTW_NightColorTemp;
    m_durationrelativ = LEDHelper::getTimeWithPattern(ParamTW_RelativDimTime, ParamTW_RelativDimBase);
    m_durationabsolut = LEDHelper::getTimeWithPattern(ParamTW_OnOffTime, ParamTW_OnOffBase);
    m_curve = ParamTW_DimCurve; // 0=A, 1=B, 2=C, 3=D, 4=E

    logDebugP("CH: %i, | HW WW: %i, CW: %i, | CT WW: %i, CW: %i, | Use Day: %i, B: %i, K: %i, | Use Night: %i, B: %i, K: %i, | Dur Rel: %i, Abs: %i, Curve: %i, | HCL Act: %i, Ch: %i, St: %i",
              _index, m_hwchannel_ww, m_hwchannel_cw, m_colortempww, m_colortempcw, m_usedayvalue, m_daybrightness, m_daycolortemp,
              m_usenightvalue, m_nightbrightness, m_nightcolortemp, m_durationrelativ, m_durationabsolut, m_curve, ParamTW_hclActive,
              ParamTW_hclChannel, ParamTW_hclStart);
}

void DimChannel_TW::processInputKo(GroupObject &ko)
{
    int channelKo = 0;
    channelKo = (ko.asap() - TW_KoOffset) % TW_KoBlockSize;
    logDebugP("Got SHORT KO %i", channelKo);

    switch (channelKo)
    {
    // Schalten
    case TW_KoSwitch:
        koHandleSwitch(ko);
        break;
    // Dimmen Absolut Brightness
    case TW_KoDimAbsoluteBrightness:
        koHandleDimmAbsBrightness(ko);
        break;
    // Dimmen Absolut ColorTemp
    case TW_KoDimAbsoluteColorTemp:
        koHandleDimmAbsColorTemp(ko);
        break;
    // Dimmen Relative Brightness
    case TW_KoDimRelativBrightness:
        koHandleDimmRelBrightness(ko);
        break;
    // Dimmen Relative ColorTemp
    case TW_KoDimRelativColorTemp:
        koHandleDimmRelColorTemp(ko);
        break;
    // Szenensteuerung
    case TW_KoSceneNumber:
        koHandleScene(ko);
        break;
    }
}

void DimChannel_TW::koHandleSwitch(GroupObject &ko)
{
    bool value = ko.value(DPT_Switch);
    if (value)
    { // on
        switchOnHelper();
        logDebugP(isNight ? "Switch On Night - with value: %i - Kelvin: %i - Brightness: %i" : "Switch On Day - with value: %i - Kelvin: %i - Brightness: %i", value, _currentValueTW[1], _newValueTW[0]);
        startTask(DimTaskTW::TW_DIM_SOFT_ON);
    }
    else
    { // off
        switchOffHelper();
        logDebugP(isNight ? "Switch Off Night - with value: %i" : "Switch Off Day - with value: %i", value);
        startTask(DimTaskTW::TW_DIM_SOFT_OFF);
    }
}

void DimChannel_TW::koHandleDimmAbsBrightness(GroupObject &ko)
{
    _newValueTW[0] = ko.value(DPT_Percent_U8);
    logDebugP("Dim Absolute Brightness - Kelvin: %i - Brightness: %i", _currentValueTW[1], _newValueTW[0]);
    startTask(DimTaskTW::TW_DIM_B_SET);
}

void DimChannel_TW::koHandleDimmAbsColorTemp(GroupObject &ko)
{
    _newValueTW[1] = ko.value(Dpt(7, 600));
    if (_currentValueTW[0] == 0) {
        logDebugP("Dim Absolute Kelvin - Kelvin: %i - Brightness: %i (Brightness is 0, we only save Kelvin)", _newValueTW[1], _currentValueTW[0]);
        _currentValueTW[1] = _newValueTW[1]; // Save kelvin but do not change brightness
        (isNight ? _lastNightValue[1] : _lastDayValue[1]) = _newValueTW[1]; // Save kelvin for later use
        updateDimValue(); // Update KO state for Kelvin
    } else {
        logDebugP("Dim Absolute Kelvin - Kelvin: %i - Brightness: %i", _newValueTW[1], _currentValueTW[0]);
        startTask(DimTaskTW::TW_DIM_K_SET);
    }
}

void DimChannel_TW::koHandleDimmRelBrightness(GroupObject &ko)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ Brightness - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ Brightness - Stop");
        startTask(DimTaskTW::TW_DIM_STOP);
    } else if (direction == 1) {
        logDebugP("Dim Relativ Brightness - DimUp");
        startTask(DimTaskTW::TW_DIM_B_UP);
    } else if (direction == 0) {
        logDebugP("Dim Relativ Brightness - DimDown");
        startTask(DimTaskTW::TW_DIM_B_DOWN);
    }
}

void DimChannel_TW::koHandleDimmRelColorTemp(GroupObject &ko)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ Kelvin - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ Kelvin - Stop");
        startTask(DimTaskTW::TW_DIM_STOP);
    } else if (direction == 1) {
        logDebugP("Dim Relativ Kelvin - DimUp");
        startTask(DimTaskTW::TW_DIM_K_UP);
    } else if (direction == 0) {
        logDebugP("Dim Relativ Kelvin - DimDown");
        startTask(DimTaskTW::TW_DIM_K_DOWN);
    }
}

void DimChannel_TW::koHandleScene(GroupObject &ko)
{
    uint8_t scene = ko.value(DPT_SceneNumber);
    scene++; // increase value by one
    logDebugP("Scene - Number: %i", scene);
    for (uint8_t i = 0; i < MAXCHANNELSCENE; i++)
    {
        uint8_t sceneparam = ((int8_t)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneNumberA + i)))));
        if (scene == sceneparam)
        {
            uint8_t action = ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneActionA + i)))));
            switch (action)
            {
            case SC_TW_None:
                // do nothing
                break;
            case SC_TW_OnValueDayNight:
                switchOnHelper();
                startTask(DimTaskTW::TW_DIM_SOFT_ON);
                break;
            case SC_TW_SetBrightness:
                _newValueTW[0] = round(((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneBrightnessA + i))))) * 2.55);
                startTask(DimTaskTW::TW_DIM_B_SET);
                break;
            case SC_TW_SetColorTemp:
                _newValueTW[1] = ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneKelvinA + (i * 2))))));
                startTask(DimTaskTW::TW_DIM_K_SET);
                break;
            case SC_TW_SetBoth:
                _newValueTW[0] = round(((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneBrightnessA + i))))) * 2.55);
                _currentValueTW[1] = ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneKelvinA + (i * 2))))));
                startTask(DimTaskTW::TW_DIM_SOFT_ON);
                break;
            case SC_EK_Off:
                switchOffHelper();
                startTask(DimTaskTW::TW_DIM_SOFT_OFF);
                break;
            }
        }
    }
}

void DimChannel_TW::switchOnHelper()
{
    if (isNight) {
        if (m_usenightvalue) {
            _newValueTW[0] = m_nightbrightness;
            _currentValueTW[1] = m_nightcolortemp;
        } else {
            _newValueTW[0] = _lastNightValue[0];
            _currentValueTW[1] = _lastNightValue[1];
        }
    } else {
        if (m_usedayvalue) {
            _newValueTW[0] = m_daybrightness;
            _currentValueTW[1] = m_daycolortemp;
        } else {
            _newValueTW[0] = _lastDayValue[0];
            _currentValueTW[1] = _lastDayValue[1];
        }
    }
}

void DimChannel_TW::switchOffHelper()
{
    // aktuellen An-Wert nur sichern, wenn der Kanal an ist und nicht bereits
    // ausgeschaltet wird - sonst überschreibt wiederholtes AUS den letzten Wert mit 0
    if (_currentTask == DimTaskTW::TW_DIM_SOFT_OFF || _currentValueTW[0] == 0)
        return;
    if (isNight) {
        _lastNightValue[0] = _currentValueTW[0];
        _lastNightValue[1] = _currentValueTW[1];
    } else {
         _lastDayValue[0] = _currentValueTW[0];
         _lastDayValue[1] = _currentValueTW[1];
    }
}

void DimChannel_TW::setDayNight(bool value)
{
    isNight = value;
}

std::vector<uint8_t> DimChannel_TW::getHWPorts()
{
    std::vector<uint8_t> ports;
    ports.push_back(m_hwchannel_ww);
    ports.push_back(m_hwchannel_cw);
    return ports;
}

uint8_t DimChannel_TW::getChannelIndex()
{
    return _index;
}

uint8_t DimChannel_TW::getChannelType()
{
    return ChannelType::TW; // 2 = TW
}

void DimChannel_TW::setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness)
{
    if (ParamTW_hclActive != 1 || channel != ParamTW_hclChannel)
        return;
    if (ParamTW_hclStart == PT_hclStart_during && _isOn) { // HCL active when channel is on
        logDebugP("HCL active - Channel: %i Kelvin: %i Brightness: %i", channel, kelvin, brightness);
        if (ParamTW_hclCheckTemperature == 1 && ParamTW_hclCheckBrightness == 1) {
            _currentValueTW[0] = round(brightness * 2.55); // HCL-Helligkeit 0-100 -> interne 0-255
            _newValueTW[1] = kelvin;
            startTask(DimTaskTW::TW_DIM_K_SET);
        } else if (ParamTW_hclCheckTemperature == 1 && ParamTW_hclCheckBrightness == 0) {
            _newValueTW[1] = kelvin;
            startTask(DimTaskTW::TW_DIM_K_SET);
        } else if (ParamTW_hclCheckTemperature == 0 && ParamTW_hclCheckBrightness == 1) {
            _newValueTW[0] = round(brightness * 2.55); // HCL-Helligkeit 0-100 -> interne 0-255
            startTask(DimTaskTW::TW_DIM_B_SET);
        }
    } else {
        _currentHclValue[0] = brightness;
        _currentHclValue[1] = kelvin;
        logDebugP("HCL will only apply if channel on, save for later use");
    }
}

void DimChannel_TW::task()
{
    dimmerTask();
}

uint16_t DimChannel_TW::calcKoNumber(int koNum)
{
    return koNum + (TW_KoBlockSize * _channelIndex) + TW_KoOffset;
}

void DimChannel_TW::sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool alwayssend)
{
    GroupObject &ko = knx.getGroupObject(calcKoNumber(koNr));
    if (ko.valueNoSendCompare(value, type)) {
        ko.objectWritten();
    } else if (alwayssend == true) {
        ko.objectWritten();
    }
}

void DimChannel_TW::updateDimValue()
{
    _isOn = _currentValueTW[0] > 0;
    logDebugP("Send DimValue to KO - OnOff: %i B: %i K: %i", _isOn, _currentValueTW[0], _currentValueTW[1]);
    sendKoStateOnChange(TW_KoStatusOnOff, _isOn, DPT_Switch, false);
    sendKoStateOnChange(TW_KoStatusBrightness, _currentValueTW[0], DPT_Percent_U8, false);
    sendKoStateOnChange(TW_KoStatusColorTemp, _currentValueTW[1], Dpt(7, 600), false);
}

//----------------------------- TW Dimmer Task ------------------------------

// Neue Aufgabe starten: _busy zuruecksetzen, damit handleDimGeneric _time
// fuer die neue Rampe frisch berechnet, auch beim Unterbrechen einer Rampe.
void DimChannel_TW::startTask(uint8_t task)
{
    _currentTask = task;
    _busy = false;
}

void DimChannel_TW::dimmerTask()
{
    _currentMillis = millis();
    switch (_currentTask)
    {
    case DimTaskTW::TW_DIM_STOP:
        handleDimStop();
        break;
    case DimTaskTW::TW_DIM_SOFT_ON:
        handleDimSoftOn();
        break;
    case DimTaskTW::TW_DIM_SOFT_OFF:
        handleDimSoftOff();
        break;
    case DimTaskTW::TW_DIM_B_SET:
        handleDimSetBrightness();
        break;
    case DimTaskTW::TW_DIM_K_SET:
        handleDimSetColorTemp();
        break;
    case DimTaskTW::TW_DIM_B_UP:
        handleDimBrightnessUp();
        break;
    case DimTaskTW::TW_DIM_B_DOWN:
        handleDimBrightnessDown();
        break;
    case DimTaskTW::TW_DIM_K_UP:
        handleDimColorTempUp();
        break;
    case DimTaskTW::TW_DIM_K_DOWN:
        handleDimColorTempDown();
        break;
    case DimTaskTW::TW_DIM_IDLE:
    default:
        break;
    }
}

void DimChannel_TW::sendDimValue()
{
    /*
    // Mired-basierte Methode
    const float M_warm = 1000000.0f / m_colortempww;
    const float M_cool = 1000000.0f / m_colortempcw;

    uint8_t percentWW = 0;
    uint8_t percentCW = 0;

    if (_currentValueTW[1] <= m_colortempww) {
        percentWW = _currentValueTW[0];
        percentCW = 0;
    } else if (_currentValueTW[1] >= m_colortempcw) {
        percentWW = 0;
        percentCW = _currentValueTW[0];
    } else {
        float M_desired = 1000000.0f / (float)_currentValueTW[1];
        float r = (M_desired - M_cool) / (M_warm - M_cool);
        percentWW = (int)round(r * _currentValueTW[0]);
        percentCW = (int)round((1.0f - r) * _currentValueTW[0]);
    }
    */
    // Lineare Interpolation
    float t;
    if (m_colortempcw == m_colortempww) // WW und KW identisch konfiguriert -> Division durch 0 vermeiden
        t = 0.0f;
    else
        t = (float)(_currentValueTW[1] - m_colortempww) / (m_colortempcw - m_colortempww);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    uint8_t percentWW = (uint8_t)((1.0f - t) * _currentValueTW[0]);
    uint8_t percentCW = (uint8_t)(t * _currentValueTW[0]);
    // logDebugP("Send DimValue to HW - WW: %i CW: %i", percentWW, percentCW);
    //LEDModule::_instance->setHwChannelValue(m_hwchannel_ww, percentWW, m_curve);
    //LEDModule::_instance->setHwChannelValue(m_hwchannel_cw, percentCW, m_curve);

    // for TW LEDs we use a special PWM dimming method to shift the PWM frequency
    // this should reduce the visible flickering of the LEDs and current peaks
    uint16_t ticksWW = curves[percentWW][m_curve];
    uint16_t ticksCW = curves[percentCW][m_curve];
    uint16_t startWW = 0;
    uint16_t endWW = startWW + ticksWW;
    uint16_t startCW = endWW % 4096;
    uint16_t endCW   = (startCW + ticksCW) % 4096;

    LEDModule::_instance->setHwChannelValuePWM(m_hwchannel_ww, startWW, endWW ,m_curve);
    LEDModule::_instance->setHwChannelValuePWM(m_hwchannel_cw, startCW, endCW, m_curve);
}

void DimChannel_TW::resend()
{
    sendDimValue();
}

void DimChannel_TW::handleDimGeneric(uint16_t &currentValue, uint16_t targetValue, uint16_t minValue, uint16_t maxValue, bool isAbsolute)
{
    if (currentValue == targetValue) {
        startTask(DimTaskTW::TW_DIM_STOP);
        return;
    }
    if (!_busy) {
        uint32_t duration = isAbsolute ? m_durationabsolut : m_durationrelativ;
        uint16_t delta = abs((int)targetValue - (int)currentValue);
        _time = duration / delta;
    }
    if (_currentMillis - _lastTaskExecution >= _time) {
        if (currentValue < targetValue && currentValue < maxValue) {
            currentValue++;
        } else if (currentValue > targetValue && currentValue > minValue) {
            currentValue--;
        } else {
            startTask(DimTaskTW::TW_DIM_STOP);
            return;
        }
        _busy = true;
        sendDimValue();
        _lastTaskExecution = millis();
    }
}

void DimChannel_TW::handleDimStop()
{
    _busy = false;
    _currentTask = DimTaskTW::TW_DIM_IDLE;
    updateDimValue();
}

void DimChannel_TW::handleDimSoftOn() {
    handleDimGeneric(_currentValueTW[0], _newValueTW[0], _valueMinBrightness, _valueMaxBrightness, true);
}

void DimChannel_TW::handleDimSoftOff() {
    handleDimGeneric(_currentValueTW[0], _valueMinBrightness, _valueMinBrightness, _valueMaxBrightness, true);
}

void DimChannel_TW::handleDimSetBrightness() {
    handleDimGeneric(_currentValueTW[0], _newValueTW[0], _valueMinBrightness, _valueMaxBrightness, true);
}

void DimChannel_TW::handleDimSetColorTemp() {
    handleDimGeneric(_currentValueTW[1], _newValueTW[1], m_colortempww, m_colortempcw, true);
}

void DimChannel_TW::handleDimBrightnessUp() {
    handleDimGeneric(_currentValueTW[0], _valueMaxBrightness, _valueMinBrightness, _valueMaxBrightness, false);
}

void DimChannel_TW::handleDimBrightnessDown() {
    handleDimGeneric(_currentValueTW[0], _valueMinBrightness, _valueMinBrightness, _valueMaxBrightness, false);
}

void DimChannel_TW::handleDimColorTempUp() {
    handleDimGeneric(_currentValueTW[1], m_colortempcw, m_colortempww, m_colortempcw, false);
}

void DimChannel_TW::handleDimColorTempDown() {
    handleDimGeneric(_currentValueTW[1], m_colortempww, m_colortempww, m_colortempcw, false);
}
