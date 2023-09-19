#include <DimChannel_TW.h>

DimChannel_TW::DimChannel_TW(uint8_t index) {
  _channelIndex = index;
}

DimChannel_TW::~DimChannel_TW() {}

const std::string DimChannel_TW::name()
{
    return "DimChannel_TW";
}

void DimChannel_TW::setup(int8_t hwchannel_ww, int8_t hwchannel_cw, uint16_t startKO) {
    // Parameter
    m_hwchannel_ww = hwchannel_ww;
    m_hwchannel_cw = hwchannel_cw;

    m_colortempww = ParamAPP_PT_TWColorTempWW;
    m_colortempcw = ParamAPP_PT_TWColorTempKW;
    m_onbrightness = round(ParamAPP_PT_TWOnBrightness * 2.55);
    m_oncolortemp = ParamAPP_PT_TWOnColorTemp;
    m_durationrelativ = ParamAPP_PT_TWRelativDimTime * 1000;
    m_durationabsolut = ParamAPP_PT_TWOnOffTime * 100;
    m_curve = ParamAPP_PT_TWDimCurve;                               // 0=A, 1=B, 2=C, 3=D

    _lastbrightness = 0;
    _lastcolortemp = 0;
  
    // calculate KO Objects and save this
    calc_ko_switch = startKO + KO_OFFSET_TW_SWITCH;
    calc_ko_dimabsolute = startKO + KO_OFFSET_TW_DIMABSOLUT;
    calc_ko_dimkelvin = startKO + KO_OFFSET_TW_DIMKELVIN;
    calc_ko_dimrelativ = startKO + KO_OFFSET_TW_DIMRELATIV;
    calc_ko_statusonoff = startKO + KO_OFFSET_TW_STATUSONOFF;
    calc_ko_statusbrightness = startKO + KO_OFFSET_TW_STATUSBRIGHTNESS;
    calc_ko_statuskelvin = startKO + KO_OFFSET_TW_STATUSKELVIN;

    // setup hw channels
    hwchannels[m_hwchannel_ww] = new HWChannel(m_hwchannel_ww);
    hwchannels[m_hwchannel_cw] = new HWChannel(m_hwchannel_cw);
    hwchannels[m_hwchannel_ww]->setup(m_hwchannel_ww, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_cw]->setup(m_hwchannel_cw, m_curve, m_durationabsolut, m_durationrelativ);

    logInfoP("------------------ DEBUG -------------------");
    logInfoP("KO Switch: %i", calc_ko_switch);
    logInfoP("KO Dim Absolute: %i", calc_ko_dimabsolute);
    logInfoP("KO Dim Kelvin: %i", calc_ko_dimkelvin);
    logInfoP("KO Dim Relativ: %i", calc_ko_dimrelativ);
    logInfoP("KO Status OnOff: %i", calc_ko_statusonoff);
    logInfoP("KO Status Brightness: %i", calc_ko_statusbrightness);
    logInfoP("KO Status Kelvin: %i", calc_ko_statuskelvin);
    logInfoP("HW Port WW: %i", m_hwchannel_ww);
    logInfoP("HW Port CW: %i", m_hwchannel_cw);
    logInfoP("PT ColorTemp WW: %i", m_colortempww);
    logInfoP("PT ColorTemp CW: %i", m_colortempcw);
    logInfoP("PT OnBrightness: %i", m_onbrightness);
    logInfoP("PT OnColorTemp: %i", m_oncolortemp);
    logInfoP("PT DurationRelativ WW: %i", m_durationrelativ);
    logInfoP("PT DurationAbsolut WW: %i", m_durationabsolut);
    logInfoP("PT DurationRelativ CW: %i", m_durationrelativ);
    logInfoP("PT DurationAbsolut CW: %i", m_durationabsolut);
    logInfoP("PT Curve WW: %i", m_curve);
    logInfoP("PT Curve CW: %i", m_curve);
    logInfoP("--------------------------------------------");
}

void DimChannel_TW::processInputKoTW(GroupObject &ko) {
    uint16_t asap = ko.asap();
    if (asap == calc_ko_switch)
    {
        bool value = ko.value(DPT_Switch);
        if (_lastbrightness == 0) { _lastbrightness = m_onbrightness; }
        if (_lastcolortemp == 0)  { _lastcolortemp = m_oncolortemp;   }
        logInfoP("Switch - Value: %i - Kelvin: %i - Brightness: %i", value, _lastcolortemp, _lastbrightness);
        if (value) { // on
            uint8_t percent = prozToDim(_lastbrightness, 3);
            uint8_t percentWW = (percent*(m_colortempcw - _lastcolortemp))/(m_colortempcw - m_colortempww);
            uint8_t percentCW = (percent*(_lastcolortemp - m_colortempww))/(m_colortempcw - m_colortempww);
            logInfoP("protzToDim Correction Value - WW: %i CW: %i", percentWW, percentCW);
            hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
            hwchannels[m_hwchannel_cw]->taskNewValue(percentCW);
        }
        else { // off
            _lastbrightness = 0;
            hwchannels[m_hwchannel_ww]->taskSoftOff();
            hwchannels[m_hwchannel_cw]->taskSoftOff();   
        }
    }
    else if (asap == calc_ko_dimabsolute) {
        uint8_t brightness = ko.value(DPT_Percent_U8);
        _lastbrightness = brightness;
        if (_lastcolortemp == 0) { _lastcolortemp = m_oncolortemp; }
        logInfoP("Dim Absolute - Kelvin: %i - Brightness: %i", _lastcolortemp, _lastbrightness);
        uint8_t percent = prozToDim(_lastbrightness, 3);
        uint8_t percentWW = (percent*(m_colortempcw - _lastcolortemp))/(m_colortempcw - m_colortempww);
        uint8_t percentCW = (percent*(_lastcolortemp - m_colortempww))/(m_colortempcw - m_colortempww);
        logInfoP("protzToDim Correction Value - WW: %i CW %i", percentWW, percentCW); 
        hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
        hwchannels[m_hwchannel_cw]->taskNewValue(percentCW);
    }
    else if (asap == calc_ko_dimkelvin) {
        uint16_t kelvin = ko.value(Dpt(7, 600));
        _lastcolortemp = kelvin;
        if (_lastbrightness == 0) { _lastbrightness = m_onbrightness; }
        logInfoP("Dim Kelvin - Kelvin: %i - Brightness: %i", _lastcolortemp, _lastbrightness);
        uint8_t percent = prozToDim(_lastbrightness, 3);
        uint8_t percentWW = (percent*(m_colortempcw - _lastcolortemp))/(m_colortempcw - m_colortempww);
        uint8_t percentCW = (percent*(_lastcolortemp - m_colortempww))/(m_colortempcw - m_colortempww);
        logInfoP("protzToDim Correction Value - WW: %i CW %i", percentWW, percentCW);
        hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
        hwchannels[m_hwchannel_cw]->taskNewValue(percentCW); 
    }
    else if (asap == calc_ko_dimrelativ) {
        uint8_t direction = ko.value(Dpt(3,7,0));
        uint8_t step = ko.value(Dpt(3,7,1));
        logInfoP("Dim Relativ - Direction: %i, Step: %i", direction, step);
        //direction true = dim up, false = dim down, step = 0 then stop
        if (step == 0) {
            logInfoP("Dim Relativ - Stop");
            // code passt noch nicht
        }
        else if (direction == 1) {
            logInfoP("Dim Relativ - DimUp");
            // code passt noch nicht
        }
        else if (direction == 0) {
            logInfoP("Dim Relativ - DimDown");
            // code passt noch nicht
        }
    }
}

void DimChannel_TW::task() {
    hwchannels[m_hwchannel_ww]->task();
    hwchannels[m_hwchannel_cw]->task();
    //run ko update every 500ms
    _currentTaskRun = millis();
    if (_currentTaskRun - _lastTaskRun >= 500) {
        if (hwchannels[m_hwchannel_ww]->isBusy() || hwchannels[m_hwchannel_cw]->isBusy()) { return; }
        if (!hwchannels[m_hwchannel_ww]->updateAvailable() || !hwchannels[m_hwchannel_cw]->updateAvailable()) { return; }

        hwchannels[m_hwchannel_ww]->resetUpdateFlag();
        hwchannels[m_hwchannel_cw]->resetUpdateFlag();
        byte value_ww = hwchannels[m_hwchannel_ww]->getCurrentValue();
        byte value_cw = hwchannels[m_hwchannel_cw]->getCurrentValue();

        if (value_ww != _lastvalue_ww || value_cw != _lastvalue_cw) {
            if (value_ww != 0 || value_cw != 0) {
                knx.getGroupObject(calc_ko_statusonoff).value((bool)1, DPT_Switch);
            } else {
                knx.getGroupObject(calc_ko_statusonoff).value((bool)0, DPT_Switch);
            }
            knx.getGroupObject(calc_ko_statuskelvin).value(_lastcolortemp, Dpt(7, 600));
            knx.getGroupObject(calc_ko_statusbrightness).value(_lastbrightness, DPT_Percent_U8);
            _lastvalue_ww = value_ww;
            _lastvalue_cw = value_cw;
        }
    _lastTaskRun = millis();
    }
}

uint16_t DimChannel_TW::prozToDim(uint8_t value, uint8_t curve) {
  return proztable[value][curve];
}  