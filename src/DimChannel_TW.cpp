#include "DimChannel_TW.h"

DimChannel_TW::DimChannel_TW(uint8_t index) : DimChannel(index) {
  _index = index;
}

DimChannel_TW::~DimChannel_TW() {}

const std::string DimChannel_TW::name()
{
    return "DimChannel_TW";
}

void DimChannel_TW::setup(uint8_t* hwchannel, uint16_t startKO) {
    // Parameter
    m_hwchannel_ww = hwchannel[0];
    m_hwchannel_cw = hwchannel[1];

    m_colortempww = ParamAPP_PT_TWColorTempWW;
    m_colortempcw = ParamAPP_PT_TWColorTempKW;
    m_useoncolortemp = ParamAPP_PT_TWUseOnColorTemp;
    m_onbrightness = round(ParamAPP_PT_TWOnBrightness * 2.55);
    m_oncolortemp = ParamAPP_PT_TWOnColorTemp;
    m_durationrelativ = ParamAPP_PT_TWRelativDimTime * 1000;
    m_durationabsolut = ParamAPP_PT_TWOnOffTime * 100;
    m_curve = ParamAPP_PT_TWDimCurve;                               // 0=A, 1=B, 2=C, 3=D

    _lastbrightness = 0;
    _lastcolortemp = 0;
  
    // calculate KO Objects and save this
    calc_ko_switch = startKO + KO_OFFSET_TW_SWITCH;
    calc_ko_dimabsolut = startKO + KO_OFFSET_TW_DIMABSOLUT;
    calc_ko_dimabsolutkelvin = startKO + KO_OFFSET_TW_DIMABSOLUTKELVIN;
    calc_ko_dimrelativ = startKO + KO_OFFSET_TW_DIMRELATIV;
    calc_ko_dimrelativkelvin = startKO + KO_OFFSET_TW_DIMRELATIVKELVIN;
    calc_ko_statusonoff = startKO + KO_OFFSET_TW_STATUSONOFF;
    calc_ko_statusbrightness = startKO + KO_OFFSET_TW_STATUSBRIGHTNESS;
    calc_ko_statuskelvin = startKO + KO_OFFSET_TW_STATUSKELVIN;

    // setup hw channels
    hwchannels[m_hwchannel_ww] = new HWChannel(m_hwchannel_ww);
    hwchannels[m_hwchannel_cw] = new HWChannel(m_hwchannel_cw);
    hwchannels[m_hwchannel_ww]->setup(m_hwchannel_ww, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_cw]->setup(m_hwchannel_cw, m_curve, m_durationabsolut, m_durationrelativ);

    logDebugP("------------------ DEBUG -------------------");
    logDebugP("KO Switch: %i", calc_ko_switch);
    logDebugP("KO Dim Absolute: %i", calc_ko_dimabsolut);
    logDebugP("KO Dim Absolut Kelvin: %i", calc_ko_dimabsolutkelvin);
    logDebugP("KO Dim Relativ: %i", calc_ko_dimrelativ);
    logDebugP("KO Dim Relativ Kelvin: %i", calc_ko_dimrelativkelvin);
    logDebugP("KO Status OnOff: %i", calc_ko_statusonoff);
    logDebugP("KO Status Brightness: %i", calc_ko_statusbrightness);
    logDebugP("KO Status Kelvin: %i", calc_ko_statuskelvin);
    logDebugP("HW Port WW: %i", m_hwchannel_ww);
    logDebugP("HW Port CW: %i", m_hwchannel_cw);
    logDebugP("PT ColorTemp WW: %i", m_colortempww);
    logDebugP("PT ColorTemp CW: %i", m_colortempcw);
    logDebugP("PT UseOnColorTemp: %i", m_useoncolortemp);
    logDebugP("PT OnBrightness: %i", m_onbrightness);
    logDebugP("PT OnColorTemp: %i", m_oncolortemp);
    logDebugP("PT DurationRelativ WW: %i", m_durationrelativ);
    logDebugP("PT DurationAbsolut WW: %i", m_durationabsolut);
    logDebugP("PT DurationRelativ CW: %i", m_durationrelativ);
    logDebugP("PT DurationAbsolut CW: %i", m_durationabsolut);
    logDebugP("PT Curve WW: %i", m_curve);
    logDebugP("PT Curve CW: %i", m_curve);
    logDebugP("--------------------------------------------");
}

void DimChannel_TW::processInputKo(GroupObject &ko) {
    uint16_t asap = ko.asap();
    if (asap == calc_ko_switch)
    {
        bool value = ko.value(DPT_Switch);
        if (_lastbrightness == 0) { _lastbrightness = m_onbrightness; }
        if (_lastcolortemp == 0)  { _lastcolortemp = m_oncolortemp;   }
        logDebugP("Switch - Value: %i - Kelvin: %i - Brightness: %i", value, _lastcolortemp, _lastbrightness);
        if (value) { // on
            uint8_t percent = prozToDim(_lastbrightness, 3);
            uint8_t percentWW = (percent*(m_colortempcw - _lastcolortemp))/(m_colortempcw - m_colortempww);
            uint8_t percentCW = (percent*(_lastcolortemp - m_colortempww))/(m_colortempcw - m_colortempww);
            logDebugP("protzToDim Correction Value - WW: %i CW: %i", percentWW, percentCW);
            hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
            hwchannels[m_hwchannel_cw]->taskNewValue(percentCW);
        }
        else { // off
            _lastbrightness = 0;
            hwchannels[m_hwchannel_ww]->taskSoftOff();
            hwchannels[m_hwchannel_cw]->taskSoftOff();   
        }
    }
    else if (asap == calc_ko_dimabsolut) {
        uint8_t brightness = ko.value(DPT_Percent_U8);
        _lastbrightness = brightness;
        if (_lastcolortemp == 0) { _lastcolortemp = m_oncolortemp; }
        logDebugP("Dim Absolute - Kelvin: %i - Brightness: %i", _lastcolortemp, _lastbrightness);
        uint8_t percent = prozToDim(_lastbrightness, 3);
        uint8_t percentWW = (percent*(m_colortempcw - _lastcolortemp))/(m_colortempcw - m_colortempww);
        uint8_t percentCW = (percent*(_lastcolortemp - m_colortempww))/(m_colortempcw - m_colortempww);
        logDebugP("protzToDim Correction Value - WW: %i CW %i", percentWW, percentCW); 
        hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
        hwchannels[m_hwchannel_cw]->taskNewValue(percentCW);
    }
    else if (asap == calc_ko_dimabsolutkelvin) {
        uint16_t kelvin = ko.value(Dpt(7, 600));
        _lastcolortemp = kelvin;
        if (_lastbrightness == 0) { _lastbrightness = m_onbrightness; }
        logDebugP("Dim Kelvin - Kelvin: %i - Brightness: %i", _lastcolortemp, _lastbrightness);
        uint8_t percent = prozToDim(_lastbrightness, 3);
        uint8_t percentWW = (percent*(m_colortempcw - _lastcolortemp))/(m_colortempcw - m_colortempww);
        uint8_t percentCW = (percent*(_lastcolortemp - m_colortempww))/(m_colortempcw - m_colortempww);
        logDebugP("protzToDim Correction Value - WW: %i CW %i", percentWW, percentCW);
        hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
        hwchannels[m_hwchannel_cw]->taskNewValue(percentCW); 
    }
    else if (asap == calc_ko_dimrelativ) {
        uint8_t direction = ko.value(Dpt(3,7,0));
        uint8_t step = ko.value(Dpt(3,7,1));
        logDebugP("Dim Relativ - Direction: %i, Step: %i", direction, step);
        //direction true = dim up, false = dim down, step = 0 then stop
        if (step == 0) {
            hwchannels[m_hwchannel_ww]->taskStop();
            hwchannels[m_hwchannel_cw]->taskStop();
        }
        else if (direction == 1) {
            logDebugP("Dim Relativ - DimUp");
            // code passt noch nicht
        }
        else if (direction == 0) {
            logDebugP("Dim Relativ - DimDown");
            // code passt noch nicht
        }
    }
}

void DimChannel_TW::setDayNight(bool value) {
    isNight = value;
}

void DimChannel_TW::task() {
    hwchannels[m_hwchannel_ww]->task();
    hwchannels[m_hwchannel_cw]->task();
    //run ko update every 500ms
    _currentTaskRun = millis();
    if (_currentTaskRun - _lastTaskRun >= 500) {
        updateDimValue();
        _lastTaskRun = millis();
    }
}

void DimChannel_TW::updateDimValue() {
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
}

uint16_t DimChannel_TW::prozToDim(uint8_t value, uint8_t curve) {
    return proztable[value][curve];
}  