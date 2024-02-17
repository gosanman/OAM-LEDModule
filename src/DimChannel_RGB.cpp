#include "DimChannel_RGB.h"

DimChannel_RGB::DimChannel_RGB(uint8_t index) {
  _channelIndex = index;
}

DimChannel_RGB::~DimChannel_RGB() {}

const std::string DimChannel_RGB::name()
{
    return "DimChannel_RGB";
}

void DimChannel_RGB::setup(int8_t hwchannel_r, int8_t hwchannel_g, int8_t hwchannel_b, uint16_t startKO) {
    // Parameter
    m_hwchannel_r = hwchannel_r;
    m_hwchannel_g = hwchannel_g;
    m_hwchannel_b = hwchannel_b;

    m_oncolor = ParamAPP_PT_RGBOnColor;
    m_useoncolor = 0;
    //m_useoncolor = ParamAPP_PT_RGBUseOnColor;
    m_durationrelativ = ParamAPP_PT_RGBRelativDimTime * 1000;
    m_durationabsolut = ParamAPP_PT_RGBOnOffTime * 100;
    m_curve = ParamAPP_PT_RGBDimCurve;                               // 0=A, 1=B, 2=C, 3=D

    // calculate KO Objects and save this
    calc_ko_switch = startKO + KO_OFFSET_RGB_SWITCH;
    calc_ko_colorrgb = startKO + KO_OFFSET_RGB_COLORRGB;
    calc_ko_colorhsv = startKO + KO_OFFSET_RGB_COLORHSV;
    calc_ko_dimabsolutshadeh = startKO + KO_OFFSET_RGB_DIMABSOLUTSHADEH;
    calc_ko_dimabsolutsaturations = startKO + KO_OFFSET_RGB_DIMABSOLUTSATURATIONS;
    calc_ko_dimabsolutbrightnessv = startKO + KO_OFFSET_RGB_DIMABSOLUTBRIGHTNESSV;
    calc_ko_dimrelativshadeh = startKO + KO_OFFSET_RGB_DIMRELATIVSHADEH;
    calc_ko_dimrelativsaturations = startKO + KO_OFFSET_RGB_DIMRELATIVSATURATIONS;
    calc_ko_dimrelativbrightnessv = startKO + KO_OFFSET_RGB_DIMRELATIVBRIGHTNESSV;
    calc_ko_statusonoff = startKO + KO_OFFSET_RGB_STATUSONOFF;
    calc_ko_statuscolorrgb = startKO + KO_OFFSET_RGB_STATUSCOLORRGB;
    calc_ko_statuscolorhsv = startKO + KO_OFFSET_RGB_STATUSCOLORHSV;
    calc_ko_statusshadeh = startKO + KO_OFFSET_RGB_STATUSSHADEH;
    calc_ko_statussaturations = startKO + KO_OFFSET_RGB_STATUSSATURATIONS;
    calc_ko_statusbrightnessv = startKO + KO_OFFSET_RGB_STATUSBRIGHTNESSV;

    // setup hw channels
    hwchannels[m_hwchannel_r] = new HWChannel(m_hwchannel_r);
    hwchannels[m_hwchannel_g] = new HWChannel(m_hwchannel_g);
    hwchannels[m_hwchannel_b] = new HWChannel(m_hwchannel_b);
    hwchannels[m_hwchannel_r]->setup(m_hwchannel_r, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_g]->setup(m_hwchannel_g, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_b]->setup(m_hwchannel_b, m_curve, m_durationabsolut, m_durationrelativ);

    logInfoP("------------------ DEBUG -------------------");
    logInfoP("KO Switch: %i", calc_ko_switch);
    logInfoP("KO Color RGB: %i", calc_ko_colorrgb);
    logInfoP("KO Color HSV: %i", calc_ko_colorhsv);
    logInfoP("KO Dim Absolut H: %i", calc_ko_dimabsolutshadeh);
    logInfoP("KO Dim Absolut S: %i", calc_ko_dimabsolutsaturations);
    logInfoP("KO Dim Absolut V: %i", calc_ko_dimabsolutbrightnessv);
    logInfoP("KO Dim Relativ H: %i", calc_ko_dimrelativshadeh);
    logInfoP("KO Dim Relativ S: %i", calc_ko_dimrelativsaturations);
    logInfoP("KO Dim Relativ V: %i", calc_ko_dimrelativbrightnessv);
    logInfoP("KO Status OnOff: %i", calc_ko_statusonoff);
    logInfoP("KO Status Color RGB: %i", calc_ko_statuscolorrgb);
    logInfoP("KO Status Color HSV: %i", calc_ko_statuscolorhsv);
    logInfoP("KO Status H: %i", calc_ko_statusshadeh);
    logInfoP("KO Status S: %i", calc_ko_statussaturations);
    logInfoP("KO Status V: %i", calc_ko_statusbrightnessv);
    logInfoP("HW Port R: %i", m_hwchannel_r);
    logInfoP("HW Port G: %i", m_hwchannel_g);
    logInfoP("HW Port B: %i", m_hwchannel_b);
    logInfoP("PT UseOnColor: %i", m_useoncolor);
    logInfoP("PT OnColor: #%.2X%.2X%.2X", m_oncolor[0], m_oncolor[1], m_oncolor[2]);
    logInfoP("PT DurationRelativ: %i", m_durationrelativ);
    logInfoP("PT DurationAbsolut: %i", m_durationabsolut);
    logInfoP("PT Curve: %i", m_curve);
    logInfoP("--------------------------------------------");
}

void DimChannel_RGB::processInputKoRGB(GroupObject &ko) {
    uint16_t asap = ko.asap();
    if (asap == calc_ko_switch) {
        bool value = ko.value(DPT_Switch);
        if (value) { // on
            uint8_t on_r, on_g, on_b;

            if (m_useoncolor == 1) { // true use oncolor
                m_oncolorvalue = m_oncolor[0];
                m_oncolorvalue |= m_oncolor[1] << 8;
                m_oncolorvalue |= m_oncolor[2] << 16;

                on_r = m_oncolor[0];
                on_g = m_oncolor[1];
                on_b = m_oncolor[2];

                _currentrgb[0] = on_r;
                _currentrgb[1] = on_g;
                _currentrgb[2] = on_b;
            }
            else { //false use last color
                on_r = _currentrgb[0];
                on_g = _currentrgb[1];
                on_b = _currentrgb[2];
            }
            logDebugP("Got Switch: %i Color: %X", value, _currentrgb);
               
            uint8_t h, s, v;
            rgbToHSV(on_r, on_g, on_b, h, s, v);
            _currenthsv[0] = h;
            _currenthsv[1] = s;
            _currenthsv[2] = v;

            hwchannels[m_hwchannel_r]->taskNewValue(on_r);
            hwchannels[m_hwchannel_g]->taskNewValue(on_g);
            hwchannels[m_hwchannel_b]->taskNewValue(on_b);
        }
        else { // off
            logDebugP("Got Switch: %i", value);

            hwchannels[m_hwchannel_r]->taskSoftOff();
            hwchannels[m_hwchannel_g]->taskSoftOff();
            hwchannels[m_hwchannel_b]->taskSoftOff();
        }
    }
    else if (asap == calc_ko_colorrgb) {
        uint32_t rgb = ko.value(DPT_Colour_RGB);
        logDebugP("Got RGB Color: %X", rgb);

        _currentrgb[0] = (rgb >> 16) & 0xFF;
        _currentrgb[1] = (rgb >> 8) & 0xFF;
        _currentrgb[2] = rgb & 0xFF;

        uint8_t h, s, v;
        rgbToHSV(_currentrgb[0], _currentrgb[1], _currentrgb[2], h, s, v);

        _currenthsv[0] = h;
        _currenthsv[1] = s;
        _currenthsv[2] = v;

        hwchannels[m_hwchannel_r]->taskNewValue(_currentrgb[0]);
        hwchannels[m_hwchannel_g]->taskNewValue(_currentrgb[1]);
        hwchannels[m_hwchannel_b]->taskNewValue(_currentrgb[2]);
    }
    else if (asap == calc_ko_colorhsv) {
        uint32_t hsv = ko.value(DPT_Colour_RGB);
        logDebugP("Got HSV Color: %X", hsv);

        _currenthsv[0] = (hsv >> 16) & 0xFF;
        _currenthsv[1] = (hsv >> 8) & 0xFF;
        _currenthsv[2] = hsv & 0xFF;

        uint8_t r, g, b;
        hsvToRGB(_currenthsv[0], _currenthsv[1], _currenthsv[2], r, g, b);

        _currentrgb[0] = r;
        _currentrgb[1] = g;
        _currentrgb[2] = b;

        hwchannels[m_hwchannel_r]->taskNewValue(r);
        hwchannels[m_hwchannel_g]->taskNewValue(g);
        hwchannels[m_hwchannel_b]->taskNewValue(b);
    }
    else if (asap == calc_ko_dimabsolutshadeh) {
        uint8_t h = ko.value(DPT_Angle);
        logDebugP("Got Shade H: %i", h);
        
        _currenthsv[0] = h;

        uint8_t r, g, b;
        hsvToRGB(_currenthsv[0], _currenthsv[1], _currenthsv[2], r, g, b); 

        _currentrgb[0] = r;
        _currentrgb[1] = g;
        _currentrgb[2] = b;

        hwchannels[m_hwchannel_r]->taskNewValue(r);
        hwchannels[m_hwchannel_g]->taskNewValue(g);
        hwchannels[m_hwchannel_b]->taskNewValue(b);
    }
    else if (asap == calc_ko_dimabsolutsaturations) {
        uint8_t s = ko.value(DPT_Scaling);
        logDebugP("Got Saturation S: %i", s);

        _currenthsv[1] = s;

        uint8_t r, g, b;
        hsvToRGB(_currenthsv[0], _currenthsv[1], _currenthsv[2], r, g, b); 

        _currentrgb[0] = r;
        _currentrgb[1] = g;
        _currentrgb[2] = b;

        hwchannels[m_hwchannel_r]->taskNewValue(r);
        hwchannels[m_hwchannel_g]->taskNewValue(g);
        hwchannels[m_hwchannel_b]->taskNewValue(b);
    }
    else if (asap == calc_ko_dimabsolutbrightnessv) {
        uint8_t v = ko.value(DPT_Scaling);
        logDebugP("Got Brightness V: %i", v);

        _currenthsv[2] = v;

        uint8_t r, g, b;
        hsvToRGB(_currenthsv[0], _currenthsv[1], _currenthsv[2], r, g, b); 

        _currentrgb[0] = r;
        _currentrgb[1] = g;
        _currentrgb[2] = b;

        hwchannels[m_hwchannel_r]->taskNewValue(r);
        hwchannels[m_hwchannel_g]->taskNewValue(g);
        hwchannels[m_hwchannel_b]->taskNewValue(b);
    }
    else if (asap == calc_ko_dimrelativshadeh) {
        uint8_t direction = ko.value(Dpt(3,7,0));
        uint8_t step = ko.value(Dpt(3,7,1));
        // logInfoP("Dim Relativ - Direction: %i, Step: %i", direction, step);
        //direction true = dim up, false = dim down, step = 0 then stop
        if (step == 0) {
            hwchannels[m_hwchannel_r]->taskStop();
            hwchannels[m_hwchannel_g]->taskStop();
            hwchannels[m_hwchannel_b]->taskStop();
        }
        else if (direction == 1) {
            // logInfoP("Dim Relativ - DimUp");
            // code passt noch nicht
        }
        else if (direction == 0) {
            // logInfoP("Dim Relativ - DimDown");
            // code passt noch nicht
        }
    }
}

void DimChannel_RGB::task() {
    hwchannels[m_hwchannel_r]->task();
    hwchannels[m_hwchannel_g]->task();
    hwchannels[m_hwchannel_b]->task();
    //run ko update every 500ms
    _currentTaskRun = millis();
    if (_currentTaskRun - _lastTaskRun >= 500) {
        if (hwchannels[m_hwchannel_r]->isBusy() || hwchannels[m_hwchannel_g]->isBusy() || hwchannels[m_hwchannel_b]->isBusy()) { return; }
        if (!hwchannels[m_hwchannel_r]->updateAvailable() || !hwchannels[m_hwchannel_g]->updateAvailable() || !hwchannels[m_hwchannel_b]->updateAvailable()) { return; }

        hwchannels[m_hwchannel_r]->resetUpdateFlag();
        hwchannels[m_hwchannel_g]->resetUpdateFlag();
        hwchannels[m_hwchannel_b]->resetUpdateFlag();
        byte value_r = hwchannels[m_hwchannel_r]->getCurrentValue();
        byte value_g = hwchannels[m_hwchannel_g]->getCurrentValue();
        byte value_b = hwchannels[m_hwchannel_b]->getCurrentValue();

        if (value_r != _lasthwvalue[0] || value_g != _lasthwvalue[1] || value_b != _lasthwvalue[2]) {
            if (value_r != 0 || value_g != 0 || value_b != 0) {
                knx.getGroupObject(calc_ko_statusonoff).value((bool)1, DPT_Switch);
            } else {
                knx.getGroupObject(calc_ko_statusonoff).value((bool)0, DPT_Switch);
            }

            _currentvalue_rgb = (_currentrgb[0] << 16) | (_currentrgb[1] << 8) | _currentrgb[2];
            _currentvalue_hsv = (_currenthsv[0] << 16) | (_currenthsv[1] << 8) | _currenthsv[2];

            _currenth = _currenthsv[0];
            _currents = _currenthsv[1];
            _currentv = _currenthsv[2];

            knx.getGroupObject(calc_ko_statuscolorrgb).value(_currentvalue_rgb, DPT_Colour_RGB);
            knx.getGroupObject(calc_ko_statuscolorhsv).value(_currentvalue_hsv, DPT_Colour_RGB);
            knx.getGroupObject(calc_ko_statusshadeh).value(_currenth, DPT_Angle);
            knx.getGroupObject(calc_ko_statussaturations).value(_currents, DPT_Scaling);
            knx.getGroupObject(calc_ko_statusbrightnessv).value(_currentv, DPT_Scaling);

            _lasthwvalue[0] = value_r;
            _lasthwvalue[1] = value_g;
            _lasthwvalue[2] = value_b;
        }
    _lastTaskRun = millis();
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