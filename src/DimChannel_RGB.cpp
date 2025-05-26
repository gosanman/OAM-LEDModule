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
    m_durationrelativ = getTimeWithPattern(ParamRGB_RelativDimTime, ParamRGB_RelativDimBase);
    m_durationabsolut = getTimeWithPattern(ParamRGB_OnOffTime, ParamRGB_OnOffBase);
    m_curve = ParamRGB_DimCurve; // 0=A, 1=B, 2=C, 3=D, 4=E
    m_gammacorrection = ParamRGB_GammaCorrection;

    // set default values for gamma correction
    if (m_gammacorrection >= 1.0f && m_gammacorrection <= 3)
    {
        if (m_gammacorrection != 2.8f)
            calcGammaTable(m_gammacorrection);
    }

    logDebugP("CH: %i, | HW R: %i, G: %i, B: %i, | Use Day: %i, C: #%.2X%.2X%.2X, Use Night: %i, C: #%.2X%.2X%.2X, | Dur Rel: %i, Abs: %i, Curve: %i, Gamma: %.1f, | HCL Act: %i, Ch: %i, St: %i",
              _index, m_hwchannel_r, m_hwchannel_g, m_hwchannel_b, m_usedayvalue, m_dayvalue[0], m_dayvalue[1], m_dayvalue[2],
              m_usenightvalue, m_nightvalue[0], m_nightvalue[1], m_nightvalue[2], m_durationrelativ, m_durationabsolut,
              m_curve, m_gammacorrection, ParamRGB_hclActive, ParamRGB_hclChannel, ParamRGB_hclStart);
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
        //rgbToHSV(_newValueRGB[0], _newValueRGB[1], _newValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
        _currentTask = DimTaskRGB::RGB_DIM_SOFT_ON;
    }
    else
    { // off
        switchOffHelper();
        logDebugP(isNight ? "Switch Off Night - with value %i" : "Switch Off Day - with value %i", value);
        _currentTask = DimTaskRGB::RGB_DIM_SOFT_OFF;
    }
}

void DimChannel_RGB::koHandleDimmAbsColorRGB(GroupObject &ko)
{
    uint32_t rgb = ko.value(DPT_Colour_RGB);
    _newValueRGB[0] = (rgb >> 16) & 0xFF;
    _newValueRGB[1] = (rgb >> 8) & 0xFF;
    _newValueRGB[2] = rgb & 0xFF;
    //rgbToHSV(_newValueRGB[0], _newValueRGB[1], _newValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
    logDebugP("Dim Absolute RGB: %X", rgb);
    _currentTask = DimTaskRGB::RGB_DIM_RGB_SET;
}

void DimChannel_RGB::koHandleDimmAbsColorHSV(GroupObject &ko)
{
    uint32_t hsv = ko.value(DPT_Colour_RGB);
    _currentValueHSV[0] = (hsv >> 16) & 0xFF;
    _currentValueHSV[1] = (hsv >> 8) & 0xFF;
    _currentValueHSV[2] = hsv & 0xFF;
    hsvToRGB(_currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2], _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
    logDebugP("Dim Absolute HSV: %X", hsv);
    _currentTask = DimTaskRGB::RGB_DIM_RGB_SET;
}

void DimChannel_RGB::koHandleDimmAbsRGB(GroupObject &ko, uint8_t index)
{
    _newValueRGB[index] = ko.value(DPT_Scaling);
    _newValueRGB[index] = round(_newValueRGB[index] * 2.55);
    //rgbToHSV(_newValueRGB[0], _newValueRGB[1], _newValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);    
    logDebugP("Dim Absolute RGB index: %i withe value: %i", index, _newValueRGB[index]);
    _currentTask = DimTaskRGB::RGB_DIM_RGB_SET;
}

void DimChannel_RGB::koHandleDimmAbsHSV(GroupObject &ko, uint8_t index)
{
    if (index == 0) { // H
        _currentValueHSV[index] = ko.value(DPT_Angle);
    } else { // S or V
        _currentValueHSV[index] = ko.value(DPT_Scaling);
    }
    hsvToRGB(_currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2], _newValueRGB[0], _newValueRGB[1], _newValueRGB[2]);
    logDebugP("Dim Absolute HSV index: %i withe value: %i", index, _currentValueHSV[index]);
    _currentTask = DimTaskRGB::RGB_DIM_RGB_SET;
}

void DimChannel_RGB::koHandleDimmRelH(GroupObject &ko)
{
}

void DimChannel_RGB::koHandleDimmRelS(GroupObject &ko)
{
}

void DimChannel_RGB::koHandleDimmRelV(GroupObject &ko)
{
}

void DimChannel_RGB::koHandleDimmRelRGB(GroupObject &ko, uint8_t index)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ ColorIndex: %i - Direction: %i, Step: %i", index, direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ ColorIndex: %i - Stop", index);
        _currentTask = DimTaskRGB::RGB_DIM_STOP;
    } else if (direction == 1) {
        logDebugP("Dim Relativ ColorIndex: %i - Up", index);
        _newValueRGB[index] = _valueMax;
        _currentTask = DimTaskRGB::RGB_DIM_RGB_REL;
    } else if (direction == 0) {
        logDebugP("Dim Relativ ColorIndex: %i - Down", index);
        _newValueRGB[index] = _valueMin;
        _currentTask = DimTaskRGB::RGB_DIM_RGB_REL;    
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
                _currentTask = DimTaskRGB::RGB_DIM_SOFT_ON;
                break;
            case SC_RGB_SetColor:
                uint8_t *colorvalue;
                colorvalue = knx.paramData((RGB_ParamBlockOffset + RGB_ParamBlockSize * channelIndex() + RGB_SceneColorA + (i * 3)));
                setNewValueRGB(colorvalue);
                rgbToHSV(_newValueRGB[0], _newValueRGB[1], _newValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
                _currentTask = DimTaskRGB::RGB_DIM_RGB_SET;
                break;
            case SC_RGB_Off:
                switchOffHelper();
                _currentTask = DimTaskRGB::RGB_DIM_SOFT_OFF;
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
    rgbToHSV(_currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
    uint32_t rgb = (_currentValueRGB[0] << 16) | (_currentValueRGB[1] << 8) | _currentValueRGB[2];
    uint32_t hsv = ((uint8_t)round((double)_currentValueHSV[0] * 255.0 / 360.0) << 16) | (_currentValueHSV[1] << 8) | _currentValueHSV[2];
    bool isOn = (_currentValueRGB[0] > 0 || _currentValueRGB[1] > 0 || _currentValueRGB[2] > 0);
    logDebugP("Send DimValue to KO - RGB: #%.2X%.2X%.2X, HSV: %i, %i, %i", _currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2],
              _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
    sendKoStateOnChange(RGB_KoStatusOnOff, isOn, DPT_Switch, false);
    sendKoStateOnChange(RGB_KoStatusColorRGB, rgb, DPT_Colour_RGB, false);
    sendKoStateOnChange(RGB_KoStatusColorHSV, hsv, DPT_Colour_RGB, false);
    sendKoStateOnChange(RGB_KoStatusShadeH, _currentValueHSV[0], DPT_Angle, false);
    sendKoStateOnChange(RGB_KoStatusSaturationS, _currentValueHSV[1], DPT_Scaling, false);
    sendKoStateOnChange(RGB_KoStatusBrightnessV, _currentValueHSV[2], DPT_Scaling, false);
    sendKoStateOnChange(RGB_KoStatusColorR, (uint8_t)_currentValueRGB[0], DPT_Percent_U8, false);
    sendKoStateOnChange(RGB_KoStatusColorG, (uint8_t)_currentValueRGB[1], DPT_Percent_U8, false);
    sendKoStateOnChange(RGB_KoStatusColorB, (uint8_t)_currentValueRGB[2], DPT_Percent_U8, false);
}

uint32_t DimChannel_RGB::getTimeWithPattern(uint16_t time, uint8_t base)
{
    if (base == TIMEBASE_HOURS && time > 1000)
    {
        time = 1000; // Begrenzung auf maximal 1000 Stunden
    }

    switch (base)
    {
    case TIMEBASE_TENTH_SECONDS:
        return time * 100;
    case TIMEBASE_SECONDS:
        return time * 1000;
    case TIMEBASE_MINUTES:
        return time * 60000;
    case TIMEBASE_HOURS:
        return time * 3600000;
    default:
        return 0;
    }
}

//----------------------Color Converter ------------------------------
// extended lib from https://github.com/ratkins/RGBConverter
// WTFPL license

void DimChannel_RGB::hsvToRGB(uint8_t in_h, uint8_t in_s, uint8_t in_v, uint8_t &out_r, uint8_t &out_g, uint8_t &out_b)
{
    float h = in_h / 255.0;
    float s = in_s / 255.0;
    float v = in_v / 255.0;

    double rt = 0;
    double gt = 0;
    double bt = 0;

    int i = int(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch (i % 6)
    {
    case 0:
        rt = v, gt = t, bt = p;
        break;
    case 1:
        rt = q, gt = v, bt = p;
        break;
    case 2:
        rt = p, gt = v, bt = t;
        break;
    case 3:
        rt = p, gt = q, bt = v;
        break;
    case 4:
        rt = t, gt = p, bt = v;
        break;
    case 5:
        rt = v, gt = p, bt = q;
        break;
    }
    out_r = rt * 255;
    out_g = gt * 255;
    out_b = bt * 255;
}

void DimChannel_RGB::rgbToHSV(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint16_t &out_h, uint16_t &out_s, uint16_t &out_v)
{
    double rd = (double)in_r / 255;
    double gd = (double)in_g / 255;
    double bd = (double)in_b / 255;
    double max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
    double h = 0, s, v = max;

    double d = max - min;
    s = max == 0 ? 0 : d / max;

    if (max != min)
    {
        if (max == rd)
        {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        }
        else if (max == gd)
        {
            h = (bd - rd) / d + 2;
        }
        else if (max == bd)
        {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }
    /*
    h = h * 360;
    if (h >= 0 && h <= 255) {
        out_h = static_cast<uint8_t>(h);
    } else if (h > 255 && h <= 360) {
        double converted_h = ((h - 256.0) / 104.0) * 255.0;
        converted_h = constrain(converted_h, 0.0, 255.0);
        out_h = static_cast<uint8_t>(round(converted_h));
    }
    */
    out_h = h * 360;
    out_s = s * 100;
    out_v = v * 100;
}

double DimChannel_RGB::threeway_max(double a, double b, double c)
{
    return max(a, max(b, c));
}

double DimChannel_RGB::threeway_min(double a, double b, double c)
{
    return min(a, min(b, c));
}

//----------------------Color Converter ------------------------------

//----------------------Gamma Converter ------------------------------

// gamma 2.8 lookup table used for color correction
uint8_t DimChannel_RGB::gammaT[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};

// re-calculates & fills gamma table
void DimChannel_RGB::calcGammaTable(float gamma)
{
    for (size_t i = 0; i < 256; i++)
    {
        gammaT[i] = (int)(powf((float)i / 255.0f, gamma) * 255.0f + 0.5f);
    }
    logDebugP("Finish recalculate gamma correction table with value: %.1f", m_gammacorrection);
}

//----------------------Gamma Converter ------------------------------

//----------------------------- TW Dimmer Task ------------------------------

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
    // logDebugP("Send DimValue to HW - R: %i G: % B: %i", _currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2]);
    LEDModule::_instance->setHwChannelValue(m_hwchannel_r, gammaT[_currentValueRGB[0]], m_curve);
    LEDModule::_instance->setHwChannelValue(m_hwchannel_g, gammaT[_currentValueRGB[1]], m_curve);
    LEDModule::_instance->setHwChannelValue(m_hwchannel_b, gammaT[_currentValueRGB[2]], m_curve);
}

void DimChannel_RGB::handleDimGeneric(uint8_t *currentValues, uint8_t *targetValues, uint8_t minValue, uint8_t maxValue, bool isAbsolute) 
{
    bool allTargetsReached = true;
    bool channelBusy[3] = {false, false, false};

    for (uint8_t i = 0; i < 3; i++) {
        if(currentValues[i] != targetValues[i]) {
            allTargetsReached = false;
            channelBusy[i] = true;
        }
    }
    if (allTargetsReached) {
        _currentTask = DimTaskRGB::RGB_DIM_STOP;
        return;
    }
    if (!_busy) {
        uint32_t duration = isAbsolute ? m_durationabsolut : m_durationrelativ;
        uint16_t maxDelta = 0;
        for (uint8_t i = 0; i < 3; i++) {
            if (channelBusy[i]) {
                uint16_t delta = abs((int)targetValues[i] - (int)currentValues[i]);
                maxDelta = max(maxDelta, delta);
            }
        }
        _time = (word)(duration / maxDelta);
        _busy = true;
    }
    if (_currentMillis - _lastTaskExecution >= _time) {
        bool valueChanged = false;
        for (uint8_t i = 0; i < 3; i++) {
            if (channelBusy[i]) {
                if (currentValues[i] < targetValues[i] && currentValues[i] < maxValue) {
                    currentValues[i]++;
                    valueChanged = true;
                } else if (currentValues[i] > targetValues[i] && currentValues[i] > minValue) {
                    currentValues[i]--;
                    valueChanged = true;
                }
            }
        }
        if (valueChanged) {
            sendDimValue();
            _lastTaskExecution = millis();
        } else {
            _currentTask = DimTaskRGB::RGB_DIM_STOP;
        }
    }
}

void DimChannel_RGB::handleDimStop()
{
    _busy = false;
    _currentTask = DimTaskRGB::RGB_DIM_IDLE;
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