#include "DimChannel_RGB.h"

DimChannel_RGB::DimChannel_RGB(uint8_t index) : DimChannel(index) {
  _index = index;
}

DimChannel_RGB::~DimChannel_RGB() {}

const std::string DimChannel_RGB::name()
{
    return "RGB";
}

void DimChannel_RGB::setup(uint8_t* hwchannel)
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
    m_curve = ParamRGB_DimCurve;                               // 0=A, 1=B, 2=C, 3=D, 4=E

    // setup hw channels
    hwchannels[m_hwchannel_r] = new HWChannel(m_hwchannel_r);
    hwchannels[m_hwchannel_g] = new HWChannel(m_hwchannel_g);
    hwchannels[m_hwchannel_b] = new HWChannel(m_hwchannel_b);
    hwchannels[m_hwchannel_r]->setup(m_hwchannel_r, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_g]->setup(m_hwchannel_g, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_b]->setup(m_hwchannel_b, m_curve, m_durationabsolut, m_durationrelativ);

    logDebugP("------------------ DEBUG -------------------");
    logDebugP("Channel Index: %i", _channelIndex);
    logDebugP("KO Switch: %i", calcKoNumber(RGB_KoSwitch));
    logDebugP("KO Color RGB: %i", calcKoNumber(RGB_KoColorRGB));
    logDebugP("KO Color HSV: %i", calcKoNumber(RGB_KoColorHSV));
    logDebugP("KO Dim Absolut H: %i", calcKoNumber(RGB_KoDimAbsoluteShadeH));
    logDebugP("KO Dim Absolut S: %i", calcKoNumber(RGB_KoDimAbsoluteSaturationS));
    logDebugP("KO Dim Absolut V: %i", calcKoNumber(RGB_KoDimAbsoluteBrightnessV));
    logDebugP("KO Dim Absolut R: %i", calcKoNumber(RGB_KoDimAbsoluteR));
    logDebugP("KO Dim Absolut G: %i", calcKoNumber(RGB_KoDimAbsoluteG));
    logDebugP("KO Dim Absolut B: %i", calcKoNumber(RGB_KoDimAbsoluteB));
    logDebugP("KO Dim Relativ H: %i", calcKoNumber(RGB_KoDimRelativShadeH));
    logDebugP("KO Dim Relativ S: %i", calcKoNumber(RGB_KoDimRelativSaturationS));
    logDebugP("KO Dim Relativ V: %i", calcKoNumber(RGB_KoDimRelativBrightnessV));
    logDebugP("KO Dim Relativ R: %i", calcKoNumber(RGB_KoDimRelativR));
    logDebugP("KO Dim Relativ G: %i", calcKoNumber(RGB_KoDimRelativG));
    logDebugP("KO Dim Relativ B: %i", calcKoNumber(RGB_KoDimRelativB));
    logDebugP("KO Status OnOff: %i", calcKoNumber(RGB_KoStatusOnOff));
    logDebugP("KO Status Color RGB: %i", calcKoNumber(RGB_KoStatusColorRGB));
    logDebugP("KO Status Color HSV: %i", calcKoNumber(RGB_KoStatusColorHSV));
    logDebugP("KO Status H: %i", calcKoNumber(RGB_KoStatusShadeH));
    logDebugP("KO Status S: %i", calcKoNumber(RGB_KoStatusSaturationS));
    logDebugP("KO Status V: %i", calcKoNumber(RGB_KoStatusBrightnessV));
    logDebugP("KO Status R: %i", calcKoNumber(RGB_KoStatusColorR));
    logDebugP("KO Status G: %i", calcKoNumber(RGB_KoStatusColorG));
    logDebugP("KO Status B: %i", calcKoNumber(RGB_KoStatusColorB));
    //logDebugP("KO Scene: %i", calcKoNumber(RGB_KoScene));
    logDebugP("HW Port R: %i", m_hwchannel_r);
    logDebugP("HW Port G: %i", m_hwchannel_g);
    logDebugP("HW Port B: %i", m_hwchannel_b);
    logDebugP("PT UseDayValue: %i", m_usedayvalue);
    logDebugP("PT DayColor: #%.2X%.2X%.2X", m_dayvalue[0], m_dayvalue[1], m_dayvalue[2]);
    logDebugP("PT UseNightValue: %i", m_usenightvalue);
    logDebugP("PT NightColor: #%.2X%.2X%.2X", m_nightvalue[0], m_nightvalue[1], m_nightvalue[2]);
    logDebugP("PT DurationRelativ: %i", m_durationrelativ);
    logDebugP("PT DurationAbsolut: %i", m_durationabsolut);
    logDebugP("PT Curve: %i", m_curve);
    logDebugP("--------------------------------------------");
}

void DimChannel_RGB::processInputKo(GroupObject &ko) 
{
    int channelKo = 0;
    channelKo = (ko.asap() - RGB_KoOffset) % RGB_KoBlockSize;
    logDebugP("Got SHORT KO %i", channelKo);

    switch (channelKo)
    {
    //Schalten
    case RGB_KoSwitch:
        koHandleSwitch(ko);
        break;

    //Dimmen Absolute RGB
    case RGB_KoColorRGB:
        koHandleDimmAbsColorRGB(ko);
        break;

    //Dimmen Absolute HSV
    case RGB_KoColorHSV:
        koHandleDimmAbsColorHSV(ko);
        break;

    //Dimmen Absolute H
    case RGB_KoDimAbsoluteShadeH:
        koHandleDimmAbsHSV(ko, 0);
        break;

    //Dimmen Absolute S
    case RGB_KoDimAbsoluteSaturationS:
        koHandleDimmAbsHSV(ko, 1);
        break;

    //Dimmen Absolute V
    case RGB_KoDimAbsoluteBrightnessV:
        koHandleDimmAbsHSV(ko, 2);
        break;

    //Dimmen Absolute R
    case RGB_KoDimAbsoluteR:
        koHandleDimmAbsRGB(ko, 0);
        break;

    //Dimmen Absolute G
    case RGB_KoDimAbsoluteG:
        koHandleDimmAbsRGB(ko, 1);
        break;

    //Dimmen Absolute B
    case RGB_KoDimAbsoluteB:
        koHandleDimmAbsRGB(ko, 2);
        break;

    //Dimmen Relative H
    case RGB_KoDimRelativShadeH:
        koHandleDimmRelH(ko);
        break;

    //Dimmen Relative S
    case RGB_KoDimRelativSaturationS:
        koHandleDimmRelS(ko);
        break;

    //Dimmen Relative V
    case RGB_KoDimRelativBrightnessV:
        koHandleDimmRelV(ko);
        break;

    //Dimmen Relative R
    case RGB_KoDimRelativR:
        koHandleDimmRelR(ko);
        break;

    //Dimmen Relative G
    case RGB_KoDimRelativG:
        koHandleDimmRelG(ko);
        break;

    //Dimmen Relative B
    case RGB_KoDimRelativB:
        koHandleDimmRelB(ko);
        break;

    //Szenensteuerung
    //case RGB_KoScene:
    //    koHandleScene(ko);
    //    break;
    }
}


void DimChannel_RGB::koHandleSwitch(GroupObject &ko) 
{
    bool value = ko.value(DPT_Switch);
    if (value) //on
    {
        if (isNight) {
            if (m_usenightvalue) {
                _currentValueRGB[0] = m_nightvalue[0];
                _currentValueRGB[1] |= m_nightvalue[1] << 8;
                _currentValueRGB[2] |= m_nightvalue[2] << 16;
            } else {
                _currentValueRGB[0] = _lastNightValue[0];
                _currentValueRGB[1] |= _lastNightValue[1] << 8;
                _currentValueRGB[2] |= _lastNightValue[2] << 16;
            }
        } else {
            if (m_usedayvalue) {
                _currentValueRGB[0] = m_dayvalue[0];
                _currentValueRGB[1] |= m_dayvalue[1] << 8;
                _currentValueRGB[2] |= m_dayvalue[2] << 16;
            } else {
                _currentValueRGB[0] = _lastDayValue[0];
                _currentValueRGB[1] |= _lastDayValue[1] << 8;
                _currentValueRGB[2] |= _lastDayValue[2] << 16;
            }
        }
        logDebugP(isNight ? "Switch Night - with value %i Color: #%.2X%.2X%.2X" : "Switch Day - with value%i Color: #%.2X%.2X%.2X", value, _currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2]);
        rgbToHSV(_currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
        sendDimValue();
    }
    else 
    { // off
        hwchannels[m_hwchannel_r]->taskSoftOff();
        hwchannels[m_hwchannel_g]->taskSoftOff();
        hwchannels[m_hwchannel_b]->taskSoftOff();
    }
}

void DimChannel_RGB::koHandleDimmAbsColorRGB(GroupObject &ko) 
{
    uint32_t rgb = ko.value(DPT_Colour_RGB);
    logDebugP("Got RGB Color: %X", rgb);

    _currentValueRGB[0] = (rgb >> 16) & 0xFF;
    _currentValueRGB[1] = (rgb >> 8) & 0xFF;
    _currentValueRGB[2] = rgb & 0xFF;

    rgbToHSV(_currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
    sendDimValue();
}

void DimChannel_RGB::koHandleDimmAbsColorHSV(GroupObject &ko) 
{
    uint32_t hsv = ko.value(DPT_Colour_RGB);
    logDebugP("Got HSV Color: %X", hsv);

    _currentValueHSV[0] = (hsv >> 16) & 0xFF;
    _currentValueHSV[1] = (hsv >> 8) & 0xFF;
    _currentValueHSV[2] = hsv & 0xFF;

    hsvToRGB(_currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2], _currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2]);
    sendDimValue();
}

void DimChannel_RGB::koHandleDimmAbsRGB(GroupObject &ko, uint8_t index)
{
    _currentValueRGB[index] = ko.value(DPT_Scaling);
    logDebugP("Got RGB index: %i withe value: %i", index, _currentValueRGB[index]);

    rgbToHSV(_currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);
    sendDimValue();
}

void DimChannel_RGB::koHandleDimmAbsHSV(GroupObject &ko, uint8_t index)
{
    if (index == 0) {
        _currentValueHSV[index] = ko.value(DPT_Angle);
    } else {
        _currentValueHSV[index] = ko.value(DPT_Scaling);
    }
    logDebugP("Got HSV index: %i withe value: %i", index, _currentValueHSV[index]);

    hsvToRGB(_currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2], _currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2]);
    sendDimValue();
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

void DimChannel_RGB::koHandleDimmRelR(GroupObject &ko) 
{
    uint8_t direction = ko.value(Dpt(3,7,0));
    uint8_t step = ko.value(Dpt(3,7,1));
    logDebugP("Dim Relativ R - Direction: %i, Step: %i", direction, step);
    //direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        hwchannels[m_hwchannel_r]->taskStop();
    }
    else if (direction == 1) {
        hwchannels[m_hwchannel_r]->taskDimUp();
    }
    else if (direction == 0) {
        hwchannels[m_hwchannel_r]->taskDimDown();
    }
}

void DimChannel_RGB::koHandleDimmRelG(GroupObject &ko) 
{
    uint8_t direction = ko.value(Dpt(3,7,0));
    uint8_t step = ko.value(Dpt(3,7,1));
    logDebugP("Dim Relativ G - Direction: %i, Step: %i", direction, step);
    //direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        hwchannels[m_hwchannel_g]->taskStop();
    }
    else if (direction == 1) {
        hwchannels[m_hwchannel_g]->taskDimUp();
    }
    else if (direction == 0) {
        hwchannels[m_hwchannel_g]->taskDimDown();
    }
}

void DimChannel_RGB::koHandleDimmRelB(GroupObject &ko) 
{
    uint8_t direction = ko.value(Dpt(3,7,0));
    uint8_t step = ko.value(Dpt(3,7,1));
    logDebugP("Dim Relativ B - Direction: %i, Step: %i", direction, step);
    //direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        hwchannels[m_hwchannel_b]->taskStop();
    }
    else if (direction == 1) {
        hwchannels[m_hwchannel_b]->taskDimUp();
    }
    else if (direction == 0) {
        hwchannels[m_hwchannel_b]->taskDimDown();
    }
}

void DimChannel_RGB::koHandleScene(GroupObject &ko) {
    /*
    uint8_t value;
    uint8_t scene = ko.value(DPT_SceneNumber);
    scene++; //increase value by one
    logDebugP("Scene - Number: %i", scene);
    for (uint8_t i = 0; i < MAXCHANNELSCENE; i++) {
        uint8_t sceneparam = ParamAPP_PT_EKSzNum;
        if (scene == sceneparam) {
            uint8_t action = ParamAPP_PT_EKSzAction;
            switch (action) {
                case SC_EK_SetBrightness:
                    value = round(ParamAPP_PT_EKSzValue * 2.55);
                    hwchannels[m_hwchannel]->taskNewValue(value);
                    break;
                default:
                    // do nothing
                    break;
            }
        }
    }
    */
}

void DimChannel_RGB::setDayNight(bool value) {
    isNight = value;
}

void DimChannel_RGB::task() {
    hwchannels[m_hwchannel_r]->task();
    hwchannels[m_hwchannel_g]->task();
    hwchannels[m_hwchannel_b]->task();
    //run update check every 100ms
    _currentUpdateRun = millis();
    if (_currentUpdateRun - _lastUpdatekRun >= 100) {
        updateDimValue();
        _lastUpdatekRun = millis();
    }
}

uint16_t DimChannel_RGB::calcKoNumber(int koNum)
{
    return koNum + (RGB_KoBlockSize * _channelIndex) + RGB_KoOffset;
}

void DimChannel_RGB::sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type)
{
    GroupObject &ko = knx.getGroupObject(calcKoNumber(koNr));
    if(ko.valueNoSendCompare(value, type))
        ko.objectWritten();
}

void DimChannel_RGB::sendDimValue()
{
    hwchannels[m_hwchannel_r]->taskNewValue(_currentValueRGB[0]);
    hwchannels[m_hwchannel_g]->taskNewValue(_currentValueRGB[1]);
    hwchannels[m_hwchannel_b]->taskNewValue(_currentValueRGB[2]);
}

void DimChannel_RGB::updateDimValue() 
{
    if (hwchannels[m_hwchannel_r]->isBusy() || hwchannels[m_hwchannel_g]->isBusy() || hwchannels[m_hwchannel_b]->isBusy()) { return; }
    if (hwchannels[m_hwchannel_r]->updateAvailable() || hwchannels[m_hwchannel_g]->updateAvailable() || hwchannels[m_hwchannel_b]->updateAvailable())
    { 
        hwchannels[m_hwchannel_r]->resetUpdateFlag();
        hwchannels[m_hwchannel_g]->resetUpdateFlag();
        hwchannels[m_hwchannel_b]->resetUpdateFlag();
        uint8_t r = hwchannels[m_hwchannel_r]->getCurrentValue();
        uint8_t g = hwchannels[m_hwchannel_g]->getCurrentValue();
        uint8_t b = hwchannels[m_hwchannel_b]->getCurrentValue();

        if (r != 0 || g != 0 || b != 0) {
            sendKoStateOnChange(RGB_KoStatusOnOff, (bool)1, DPT_Switch);
            _currentValueRGB[0] = r;
            _currentValueRGB[1] = g;
            _currentValueRGB[2] = b;
        } else {
            sendKoStateOnChange(RGB_KoStatusOnOff, (bool)0, DPT_Switch);
        }

        rgbToHSV(_currentValueRGB[0], _currentValueRGB[1], _currentValueRGB[2], _currentValueHSV[0], _currentValueHSV[1], _currentValueHSV[2]);

        if (isNight) {
            _lastNightValue[0] = _currentValueRGB[0];  
            _lastNightValue[1] = _currentValueRGB[1];
            _lastNightValue[2] = _currentValueRGB[2];
        } else {
            _lastDayValue[0] = _currentValueRGB[0];
            _lastDayValue[1] = _currentValueRGB[1];
            _lastDayValue[2] = _currentValueRGB[2];
        }

        uint32_t rgb = (_currentValueRGB[0] << 16) | (_currentValueRGB[1] << 8) | _currentValueRGB[2];
        uint32_t hsv = (_currentValueHSV[0] << 16) | (_currentValueHSV[1] << 8) | _currentValueHSV[2];

        sendKoStateOnChange(RGB_KoStatusColorRGB, rgb, DPT_Colour_RGB);
        sendKoStateOnChange(RGB_KoStatusColorHSV, hsv, DPT_Colour_RGB);
        sendKoStateOnChange(RGB_KoStatusShadeH, _currentValueHSV[0], DPT_Angle);
        sendKoStateOnChange(RGB_KoStatusSaturationS, _currentValueHSV[1], DPT_Scaling);
        sendKoStateOnChange(RGB_KoStatusBrightnessV, _currentValueHSV[2], DPT_Scaling);
        sendKoStateOnChange(RGB_KoStatusColorR, _currentValueRGB[0], DPT_Percent_U8);
        sendKoStateOnChange(RGB_KoStatusColorG, _currentValueRGB[1], DPT_Percent_U8);
        sendKoStateOnChange(RGB_KoStatusColorB, _currentValueRGB[2], DPT_Percent_U8);
    }
}

uint32_t DimChannel_RGB::getTimeWithPattern(uint16_t time, uint8_t base) 
{
    switch (base)
    {
        case TIMEBASE_TENTH_SECONDS:
            return time * 100;
            break;
        case TIMEBASE_SECONDS:
            return time * 1000;
            break;
        case TIMEBASE_MINUTES:
            return time * 60000;
            break;
        case TIMEBASE_HOURS:
            // for hour, we can only cover 1193 hours in milliseconds, we allow just 1000 here
            if (time > 1000) 
                time = 1000;
            return time * 3600000;
            break;
        default:
            return 0;
            break;
    }
}

//----------------------Color Converter ------------------------------
//extended lib from https://github.com/ratkins/RGBConverter
//WTFPL license

void DimChannel_RGB::hsvToRGB(uint8_t in_h, uint8_t in_s, uint8_t in_v, uint8_t& out_r, uint8_t& out_g, uint8_t& out_b) {
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

    switch(i % 6){
        case 0: rt = v, gt = t, bt = p; break;
        case 1: rt = q, gt = v, bt = p; break;
        case 2: rt = p, gt = v, bt = t; break;
        case 3: rt = p, gt = q, bt = v; break;
        case 4: rt = t, gt = p, bt = v; break;
        case 5: rt = v, gt = p, bt = q; break;
    }
	out_r = rt * 255;
	out_g = gt * 255;
	out_b = bt * 255;

}

void DimChannel_RGB::rgbToHSV(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint8_t& out_h, uint8_t& out_s, uint8_t& out_v) {
    double rd = (double) in_r/255;
    double gd = (double) in_g/255;
    double bd = (double) in_b/255;
    double max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
    double h = 0, s, v = max;

    double d = max - min;
    s = max == 0 ? 0 : d / max;

    if (max != min) { 
        if (max == rd) {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        } else if (max == gd) {
            h = (bd - rd) / d + 2;
        } else if (max == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }
    h = h * 360;
    if (h >= 0 && h <= 255) {
        out_h = static_cast<uint8_t>(h);
    } else if (h > 255 && h <= 360) {
        double converted_h = ((h - 256.0) / 104.0) * 255.0;
        converted_h = constrain(converted_h, 0.0, 255.0);
        out_h = static_cast<uint8_t>(round(converted_h));
    }
    out_s = s * 100;
    out_v = v * 100;
}

double DimChannel_RGB::threeway_max(double a, double b, double c) {
    return max(a, max(b, c));
}

double DimChannel_RGB::threeway_min(double a, double b, double c) {
    return min(a, min(b, c));
}
//----------------------Color Converter ------------------------------