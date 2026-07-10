#include "DimChannel_RGB.h"

DimChannel_RGB::DimChannel_RGB(uint8_t index) : DimChannel(index)
{
    _index = index;
}

DimChannel_RGB::~DimChannel_RGB() {}

const std::string DimChannel_RGB::name()
{
    return "RGB";
}

void DimChannel_RGB::setup(uint8_t *hwchannel)
{
    // Parameter
    m_hwchannel_r = hwchannel[0];
    m_hwchannel_g = hwchannel[1];
    m_hwchannel_b = hwchannel[2];

    m_usedayvalue = ParamRGB_UseOnColor;
    m_dayvalue = ParamRGB_OnColor;
    m_usenightvalue = ParamRGB_UseNightColor;
    m_nightvalue = ParamRGB_NightColor;
    m_durationrelativ = LEDHelper::getTimeWithPattern(ParamRGB_RelativDimTime, ParamRGB_RelativDimBase);
    m_durationabsolut = LEDHelper::getTimeWithPattern(ParamRGB_OnOffTime, ParamRGB_OnOffBase);
    m_curve = ParamRGB_DimCurve; // 0=A, 1=B, 2=C, 3=D, 4=E
    m_gammacorrection = ParamRGB_GammaCorrection;

    logDebugP("CH: %i, | HW R: %i, G: %i, B: %i, | Use Day: %i, C: #%.2X%.2X%.2X, Use Night: %i, C: #%.2X%.2X%.2X, | Dur Rel: %i, Abs: %i, Curve: %i, Gamma: %.1f, | HCL Act: %i, Ch: %i, St: %i",
              _index, m_hwchannel_r, m_hwchannel_g, m_hwchannel_b, m_usedayvalue, m_dayvalue[0], m_dayvalue[1], m_dayvalue[2],
              m_usenightvalue, m_nightvalue[0], m_nightvalue[1], m_nightvalue[2], m_durationrelativ, m_durationabsolut,
              m_curve, m_gammacorrection, ParamRGB_hclActive, ParamRGB_hclChannel, ParamRGB_hclStart);

    // Gamma-Tabelle je Kanal berechnen (ungültige Werte -> Default 2.8)
    float gamma = (m_gammacorrection >= 1.0f && m_gammacorrection <= 3.0f) ? m_gammacorrection : 2.8f;
    LEDHelper::calcGammaTable(gamma, m_gammaT);
    logDebugP("Gamma correction table for channel with value: %.1f", gamma);
}

void DimChannel_RGB::processInputKo(GroupObject &ko)
{
    int channelKo = 0;
    channelKo = (ko.asap() - RGB_KoOffset) % RGB_KoBlockSize;
    logDebugP("Got SHORT KO %i", channelKo);

    switch (channelKo)
    {
    // Schalten
    case RGB_KoSwitch:
        koHandleSwitch(ko);
        break;
    // Dimmen Absolute RGB
    case RGB_KoColorRGB:
        koHandleDimmAbsColorRGB(ko);
        break;
    // Dimmen Absolute HSV
    case RGB_KoColorHSV:
        koHandleDimmAbsColorHSV(ko);
        break;
    // Dimmen Absolute H
    case RGB_KoDimAbsoluteShadeH:
        koHandleDimmAbsHSV(ko, 0);
        break;
    // Dimmen Absolute S
    case RGB_KoDimAbsoluteSaturationS:
        koHandleDimmAbsHSV(ko, 1);
        break;
    // Dimmen Absolute V
    case RGB_KoDimAbsoluteBrightnessV:
        koHandleDimmAbsHSV(ko, 2);
        break;
    // Dimmen Absolute R
    case RGB_KoDimAbsoluteR:
        koHandleDimmAbsRGB(ko, 0);
        break;
    // Dimmen Absolute G
    case RGB_KoDimAbsoluteG:
        koHandleDimmAbsRGB(ko, 1);
        break;
    // Dimmen Absolute B
    case RGB_KoDimAbsoluteB:
        koHandleDimmAbsRGB(ko, 2);
        break;
    // Dimmen Relative H
    case RGB_KoDimRelativShadeH:
        koHandleDimmRelH(ko);
        break;
    // Dimmen Relative S
    case RGB_KoDimRelativSaturationS:
        koHandleDimmRelS(ko);
        break;
    // Dimmen Relative V
    case RGB_KoDimRelativBrightnessV:
        koHandleDimmRelV(ko);
        break;
    // Dimmen Relative R
    case RGB_KoDimRelativR:
        koHandleDimmRelRGB(ko, 0);
        break;
    // Dimmen Relative G
    case RGB_KoDimRelativG:
        koHandleDimmRelRGB(ko, 1);
        break;
    // Dimmen Relative B
    case RGB_KoDimRelativB:
        koHandleDimmRelRGB(ko, 2);
        break;
    // Szenensteuerung
    case RGB_KoSceneNumber:
        koHandleScene(ko);
        break;
    }
}

void DimChannel_RGB::koHandleSwitch(GroupObject &ko)
{
    bool value = ko.value(DPT_Switch);
    if (value)
    { // on
        switchOnHelper();
        logDebugP(isNight ? "Switch On Night - with value %i Color: #%.2X%.2X%.2X" : "Switch On Day - with value %i Color: #%.2X%.2X%.2X", value, _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
        startTask(DimTaskRGB::RGB_DIM_SOFT_ON);
    }
    else
    { // off
        switchOffHelper();
        logDebugP(isNight ? "Switch Off Night - with value %i" : "Switch Off Day - with value %i", value);
        startTask(DimTaskRGB::RGB_DIM_SOFT_OFF);
    }
}

void DimChannel_RGB::koHandleDimmAbsColorRGB(GroupObject &ko)
{
    uint32_t rgb = ko.value(DPT_Colour_RGB);
    _newValueRGB[0] = (rgb >> 16) & 0xFF;
    _newValueRGB[1] = (rgb >> 8) & 0xFF;
    _newValueRGB[2] = rgb & 0xFF;
    logDebugP("Dim Absolute RGB: %X", rgb);
    startTask(DimTaskRGB::RGB_DIM_RGB_SET);
}

void DimChannel_RGB::koHandleDimmAbsColorHSV(GroupObject &ko)
{
    uint32_t hsv = ko.value(DPT_Colour_RGB);
    _currentValueHSV[0] = (hsv >> 16) & 0xFF;
    _currentValueHSV[1] = (hsv >> 8) & 0xFF;
    _currentValueHSV[2] = hsv & 0xFF;
    LEDHelper::hsvToRGB(_currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2], _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
    logDebugP("Dim Absolute HSV: %X", hsv);
    startTask(DimTaskRGB::RGB_DIM_RGB_SET);
}

void DimChannel_RGB::koHandleDimmAbsRGB(GroupObject &ko, uint8_t index)
{
    _newValueRGB[index] = ko.value(DPT_Scaling);
    _newValueRGB[index] = round(_newValueRGB[index] * 2.55);
    logDebugP("Dim Absolute RGB index: %i withe value: %i", index, _newValueRGB[index]);
    startTask(DimTaskRGB::RGB_DIM_RGB_SET);
}

void DimChannel_RGB::koHandleDimmAbsHSV(GroupObject &ko, uint8_t index)
{
    if (index == 0) { // H
        _currentValueHSV[index] = ko.value(DPT_Angle);   // KNX-Einheit: 0-360
    } else { // S or V
        _currentValueHSV[index] = ko.value(DPT_Scaling); // KNX-Einheit: 0-100
    }
    // _currentValueHSV in KNX-Einheiten (H 0-360, S/V 0-100), hsvToRGB erwartet je 0-255
    // -> hier umrechnen (analog zum *2.55 im R/G/B-Pfad)
    uint8_t h255 = (uint8_t)round(_currentValueHSV[0] * 255.0 / 360.0);
    uint8_t s255 = (uint8_t)round(_currentValueHSV[1] * 2.55);
    uint8_t v255 = (uint8_t)round(_currentValueHSV[2] * 2.55);
    LEDHelper::hsvToRGB(h255, s255, v255, _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
    logDebugP("Dim Absolute HSV index: %i withe value: %i", index, _currentValueHSV[index]);
    startTask(DimTaskRGB::RGB_DIM_RGB_SET);
}

void DimChannel_RGB::koHandleDimmRelH(GroupObject &ko)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ H - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ H - Stop");
        startTask(DimTaskRGB::RGB_DIM_STOP);
    } else if (direction == 1) {
        logDebugP("Dim Relativ H - Up");
    } else if (direction == 0) {
        logDebugP("Dim Relativ H - Down");
    }
}

void DimChannel_RGB::koHandleDimmRelS(GroupObject &ko)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ S - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ S - Stop");
        startTask(DimTaskRGB::RGB_DIM_STOP);
    } else if (direction == 1) {
        logDebugP("Dim Relativ S - Up");
    } else if (direction == 0) {
        logDebugP("Dim Relativ S - Down");
    }
}

void DimChannel_RGB::koHandleDimmRelV(GroupObject &ko)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ V - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ V - Stop");
        startTask(DimTaskRGB::RGB_DIM_STOP);
    } else if (direction == 1) {
        logDebugP("Dim Relativ V - Up");
    } else if (direction == 0) {
        logDebugP("Dim Relativ V - Down");
    }
}

void DimChannel_RGB::koHandleDimmRelRGB(GroupObject &ko, uint8_t index)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ ColorIndex: %i - Direction: %i, Step: %i", index, direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ ColorIndex: %i - Stop", index);
        startTask(DimTaskRGB::RGB_DIM_STOP);
    } else {
        // nur den adressierten Kanal bewegen: die anderen beiden auf ihren
        // aktuellen Wert als Ziel setzen, sonst zieht handleDimGeneric (das
        // alle drei proportional dimmt) sie Richtung veralteter _newValueRGB
        _newValueRGB[0] = _currentValueRGB[0];
        _newValueRGB[1] = _currentValueRGB[1];
        _newValueRGB[2] = _currentValueRGB[2];
        _newValueRGB[index] = (direction == 1) ? _valueMax : _valueMin;
        logDebugP("Dim Relativ ColorIndex: %i - %s", index, (direction == 1) ? "Up" : "Down");
        startTask(DimTaskRGB::RGB_DIM_RGB_REL);
    }
}

void DimChannel_RGB::koHandleScene(GroupObject &ko)
{
    uint8_t scene = ko.value(DPT_SceneNumber);
    scene++; // increase value by one
    logDebugP("Scene - Number: %i", scene);
    for (uint8_t i = 0; i < MAXCHANNELSCENE; i++)
    {
        uint8_t sceneparam = ((int8_t)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_SceneNumberA + i)))));
        if (scene == sceneparam)
        {
            uint8_t action = ((uint)((knx.paramByte((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_SceneActionA + i)))));
            switch (action)
            {
            case SC_RGB_None:
                // do nothing
                break;
            case SC_RGB_OnValueDayNight:
                switchOnHelper();
                startTask(DimTaskRGB::RGB_DIM_SOFT_ON);
                break;
            case SC_RGB_SetColor:
                uint8_t *colorvalue;
                colorvalue = knx.paramData((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_SceneColorA + (i * 3)));
                setNewValueRGB(colorvalue);
                startTask(DimTaskRGB::RGB_DIM_RGB_SET);
                break;
            case SC_RGB_Off:
                switchOffHelper();
                startTask(DimTaskRGB::RGB_DIM_SOFT_OFF);
                break;
            }
        }
    }
}

void DimChannel_RGB::setNewValueRGB(uint8_t *value)
{
    _newValueRGB[0] = value[0];
    _newValueRGB[1] = value[1];
    _newValueRGB[2] = value[2];
}

void DimChannel_RGB::switchOnHelper()
{
    if (isNight) {
        if (m_usenightvalue) {
            setNewValueRGB(m_nightvalue);
        } else{
            setNewValueRGB(_lastNightValue);
        }
    } else {
        if (m_usedayvalue) {
            setNewValueRGB(m_dayvalue);
        } else{
            setNewValueRGB(_lastDayValue);
        }
    }
}

void DimChannel_RGB::switchOffHelper()
{
    // aktuelle An-Farbe nur sichern, wenn der Kanal an ist und nicht bereits
    // ausgeschaltet wird - sonst überschreibt wiederholtes AUS die letzte Farbe mit Schwarz
    if (_currentTask == DimTaskRGB::RGB_DIM_SOFT_OFF ||
        (_currentValueRGB[0] == 0 && _currentValueRGB[1] == 0 && _currentValueRGB[2] == 0))
        return;
    if (isNight) {
        _lastNightValue[0] = _currentValueRGB[0];
        _lastNightValue[1] = _currentValueRGB[1];
        _lastNightValue[2] = _currentValueRGB[2];
    } else {
        _lastDayValue[0] = _currentValueRGB[0];
        _lastDayValue[1] = _currentValueRGB[1];
        _lastDayValue[2] = _currentValueRGB[2];
    }

}

void DimChannel_RGB::setDayNight(bool value)
{
    isNight = value;
}

std::vector<uint8_t> DimChannel_RGB::getHWPorts()
{
    std::vector<uint8_t> ports;
    ports.push_back(m_hwchannel_r);
    ports.push_back(m_hwchannel_g);
    ports.push_back(m_hwchannel_b);
    return ports;
}

uint8_t DimChannel_RGB::getChannelIndex()
{
    return _index;
}

uint8_t DimChannel_RGB::getChannelType()
{
    return ChannelType::RGB; // 3 = RGB
}

void DimChannel_RGB::setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness)
{
    if (ParamRGB_hclActive != 1 || channel != ParamRGB_hclChannel)
        return;
    if (ParamRGB_hclStart == PT_hclStart_during && _isOn) { // HCL active when channel is on
        logDebugP("HCL active - Channel: %i Kelvin: %i Brightness: %i",channel, kelvin, brightness);
        if (ParamRGB_hclCheckTemperature == 1 && ParamRGB_hclCheckBrightness == 1) {
            LEDHelper::kelvinToRGB(kelvin, brightness, _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
            startTask(DimTaskRGB::RGB_DIM_RGB_SET);
        } else if (ParamRGB_hclCheckTemperature == 1 && ParamRGB_hclCheckBrightness == 0) {
            LEDHelper::kelvinToRGB(kelvin, 100, _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
            startTask(DimTaskRGB::RGB_DIM_RGB_SET);
        } else if (ParamRGB_hclCheckTemperature == 0 && ParamRGB_hclCheckBrightness == 1) {
            LEDHelper::adjustRGBBrightness(_currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2], brightness, _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
            startTask(DimTaskRGB::RGB_DIM_RGB_SET);
        }
    } else {
        _currentHclValue[0] = brightness;
        _currentHclValue[1] = kelvin;
        logDebugP("HCL will only apply if channel on, save for later use");      
    }
}

void DimChannel_RGB::task()
{
    dimmerTask();
}

uint16_t DimChannel_RGB::calcKoNumber(int koNum)
{
    return koNum + (RGB_KoBlockSize * _channelIndex) + RGB_KoOffset;
}

void DimChannel_RGB::sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool alwayssend)
{
    GroupObject &ko = knx.getGroupObject(calcKoNumber(koNr));
    if (ko.valueNoSendCompare(value, type)) {
        ko.objectWritten(); }
    else if (alwayssend == true) {
        ko.objectWritten();
    }
}

void DimChannel_RGB::updateDimValue()
{
    LEDHelper::rgbToHSV(_currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
    uint32_t rgb = (_currentValueRGB[0] << 16) | (_currentValueRGB[1] << 8) | _currentValueRGB[2];
    uint32_t hsv = ((uint8_t)round((double)_currentValueHSV[0] * 255.0 / 360.0) << 16) | (_currentValueHSV[1] << 8) | _currentValueHSV[2];
    _isOn = (_currentValueRGB[0] > 0 || _currentValueRGB[1] > 0 || _currentValueRGB[2] > 0);
    logDebugP("Send DimValue to KO - OnOff: %i RGB: #%.2X%.2X%.2X HSV: %i, %i, %i", _isOn, _currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2],
              _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
    sendKoStateOnChange(RGB_KoStatusOnOff, _isOn, DPT_Switch, false);
    sendKoStateOnChange(RGB_KoStatusColorRGB, rgb, DPT_Colour_RGB, false);
    sendKoStateOnChange(RGB_KoStatusColorHSV, hsv, DPT_Colour_RGB, false);
    sendKoStateOnChange(RGB_KoStatusShadeH, _currentValueHSV[0], DPT_Angle, false);
    sendKoStateOnChange(RGB_KoStatusSaturationS, _currentValueHSV[1], DPT_Scaling, false);
    sendKoStateOnChange(RGB_KoStatusBrightnessV, _currentValueHSV[2], DPT_Scaling, false);
    sendKoStateOnChange(RGB_KoStatusColorR, (uint8_t)_currentValueRGB[0], DPT_Percent_U8, false);
    sendKoStateOnChange(RGB_KoStatusColorG, (uint8_t)_currentValueRGB[1], DPT_Percent_U8, false);
    sendKoStateOnChange(RGB_KoStatusColorB, (uint8_t)_currentValueRGB[2], DPT_Percent_U8, false);
}

//----------------------------- TW Dimmer Task ------------------------------

// Neue Aufgabe starten: Gate-Flags zuruecksetzen, damit handleDimGeneric die
// proportionalen Increments und _time fuer die neue Rampe frisch berechnet,
// auch wenn eine laufende Rampe unterbrochen wird.
void DimChannel_RGB::startTask(uint8_t task)
{
    _currentTask = task;
    _dimmingInit = false;
    _busy = false;
}

void DimChannel_RGB::dimmerTask()
{
    _currentMillis = millis();
    switch (_currentTask)
    {
    case DimTaskRGB::RGB_DIM_STOP:
        handleDimStop();
        break;
    case DimTaskRGB::RGB_DIM_SOFT_ON:
        handleDimSoftOn();
        break;
    case DimTaskRGB::RGB_DIM_SOFT_OFF:
        handleDimSoftOff();
        break;
    case DimTaskRGB::RGB_DIM_RGB_SET:
        handleDimSetRGB();
        break;
    case DimTaskRGB::RGB_DIM_RGB_REL:
        handleDimRelRGB();
        break;   
    case DimTaskRGB::RGB_DIM_IDLE:
    default:
        break;
    }
}

void DimChannel_RGB::sendDimValue()
{
    LEDModule::_instance->setHwChannelValue(m_hwchannel_r, m_gammaT[_currentValueRGB[0]], m_curve);
    LEDModule::_instance->setHwChannelValue(m_hwchannel_g, m_gammaT[_currentValueRGB[1]], m_curve);
    LEDModule::_instance->setHwChannelValue(m_hwchannel_b, m_gammaT[_currentValueRGB[2]], m_curve);
}

void DimChannel_RGB::handleDimGeneric(uint8_t *currentValues, uint8_t *targetValues, uint8_t minValue, uint8_t maxValue, bool isAbsolute)
{
    if (!_dimmingInit) {
        uint16_t maxDelta = 0;
        // Schrittdifferenzen bestimmen
        for (uint8_t i = 0; i < 3; i++) {
            uint16_t delta = abs((int)targetValues[i] - (int)currentValues[i]);
            if (delta > maxDelta) maxDelta = delta;
        }
        // Schrittgrößen berechnen
        for (uint8_t i = 0; i < 3; i++) {
            uint16_t delta = abs((int)targetValues[i] - (int)currentValues[i]);
            _dimIncrement[i] = (maxDelta == 0) ? 0.0f : ((float)delta / (float)maxDelta);
            _dimAcc[i] = 0.0f;
        }
        uint32_t duration = isAbsolute ? m_durationabsolut : m_durationrelativ;
        _time = maxDelta ? (duration / maxDelta) : duration;
        _dimmingInit = true;
        _busy = true;
    }

    bool allTargetsReached = true;
    for (uint8_t i = 0; i < 3; i++) {
        if(currentValues[i] != targetValues[i]) {
            allTargetsReached = false;
            break;
        }
    } if (allTargetsReached) {
        startTask(DimTaskRGB::RGB_DIM_STOP);
        _dimmingInit = false;
        _busy = false;
        return;
    }

    if (_currentMillis - _lastTaskExecution >= _time) {
        bool valueChanged = false;
        for (uint8_t i = 0; i < 3; i++) {
            if (currentValues[i] != targetValues[i]) {
                _dimAcc[i] += _dimIncrement[i];
                if (_dimAcc[i] >= 1.0f) {
                    int8_t step = (currentValues[i] < targetValues[i]) ? 1 : -1;
                    currentValues[i] = std::max(std::min((int)currentValues[i] + step, (int)maxValue), (int)minValue);
                    _dimAcc[i] -= 1.0f;
                    valueChanged = true;
                }
            }
        }
        if (valueChanged) {
            sendDimValue();
            _lastTaskExecution = millis();
        }
    }
}

void DimChannel_RGB::handleDimStop()
{
    _busy = false;
    _currentTask = DimTaskRGB::RGB_DIM_IDLE;
    _dimmingInit = false;
    updateDimValue();
}

void DimChannel_RGB::handleDimSoftOn() {
    handleDimGeneric(_currentValueRGB, _newValueRGB, _valueMin, _valueMax, true);
}

void DimChannel_RGB::handleDimSoftOff() {
    handleDimGeneric(_currentValueRGB, _valueOff, _valueMin, _valueMax, true);
}

void DimChannel_RGB::handleDimSetRGB() {
    handleDimGeneric(_currentValueRGB, _newValueRGB, _valueMin, _valueMax, true);
}

void DimChannel_RGB::handleDimRelRGB() {
    handleDimGeneric(_currentValueRGB, _newValueRGB, _valueMin, _valueMax, false);
}
