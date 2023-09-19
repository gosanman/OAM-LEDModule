#include <DimChannel_EK.h>

DimChannel_EK::DimChannel_EK(uint8_t index) {
  _channelIndex = index;
}

DimChannel_EK::~DimChannel_EK() {}

const std::string DimChannel_EK::name()
{
    return "DimChannel_EK";
}

void DimChannel_EK::setup(int8_t hwchannel, uint16_t startKO) {
    // Parameter
    m_hwchannel = hwchannel;
    
    m_startko = startKO;
    m_onbrightness = round(ParamAPP_PT_EKOnBrightness * 2.55);
    m_durationrelativ = ParamAPP_PT_EKRelativDimTime * 1000;
    m_durationabsolut = ParamAPP_PT_EKOnOffTime * 100;
    m_curve = ParamAPP_PT_EKDimCurve;                               // 0=A, 1=B, 2=C, 3=D

    _lastbrightness = 0;

    // calculate KO Objects and save this
    calc_ko_switch = m_startko + KO_OFFSET_EK_SWITCH;
    calc_ko_dimabsolute = m_startko + KO_OFFSET_EK_DIMABSOLUT;
    calc_ko_dimrelativ = m_startko + KO_OFFSET_EK_DIMRELATIV;
    calc_ko_statusonoff = m_startko + KO_OFFSET_EK_STATUSONOFF;
    calc_ko_statusbrightness = m_startko + KO_OFFSET_EK_STATUSBRIGHTNESS;

    // setup hw channels
    hwchannels[m_hwchannel] = new HWChannel(m_hwchannel);
    hwchannels[m_hwchannel]->setup(m_hwchannel, m_curve, m_durationabsolut, m_durationrelativ);
 
    logInfoP("------------------ DEBUG -------------------");
    logInfoP("KO Switch: %i", calc_ko_switch);
    logInfoP("KO Dim Absolute: %i", calc_ko_dimabsolute);
    logInfoP("KO Dim Relativ: %i", calc_ko_dimrelativ);
    logInfoP("KO Status OnOff: %i", calc_ko_statusonoff);
    logInfoP("KO Status Brightness: %i", calc_ko_statusbrightness);
    logInfoP("HW Port: %i", m_hwchannel);
    logInfoP("PT OnBrightness: %i", m_onbrightness);
    logInfoP("PT DurationRelativ: %i", m_durationrelativ);
    logInfoP("PT DurationAbsolut: %i", m_durationabsolut);
    logInfoP("PT Curve: %i", m_curve);
    logInfoP("--------------------------------------------");
}

void DimChannel_EK::processInputKoEK(GroupObject &ko) {
    uint16_t asap = ko.asap();
    calc_ko_switch = m_startko + KO_OFFSET_EK_SWITCH;
    if (asap == calc_ko_switch)
    {
        bool value = ko.value(DPT_Switch);
        if (_lastbrightness == 0) { _setbrightness = m_onbrightness; } else {_setbrightness = _lastbrightness;}
        logInfoP("Switch - Value: %i", value);
        if (value) {
            hwchannels[m_hwchannel]->taskNewValue(_setbrightness);
        }
        else {
            hwchannels[m_hwchannel]->taskSoftOff();
        }
    }
    else if (asap == calc_ko_dimabsolute) {
        uint8_t brightness = ko.value(DPT_Percent_U8);
            _lastbrightness = brightness;
            hwchannels[m_hwchannel]->taskNewValue(brightness);
    }
    else if (asap == calc_ko_dimrelativ) {
        uint8_t direction = ko.value(Dpt(3,7,0));
        uint8_t step = ko.value(Dpt(3,7,1));
        logInfoP("Dim Relativ - Direction: %i, Step: %i", direction, step);
        //direction true = dim up, false = dim down, step = 0 then stop
        if (step == 0) {
            hwchannels[m_hwchannel]->taskStop();
        }
        else if (direction == 1) {
            hwchannels[m_hwchannel]->taskDimUp();
        }
        else if (direction == 0) {
            hwchannels[m_hwchannel]->taskDimDown();
        }
    }
}

void DimChannel_EK::task() {
    hwchannels[m_hwchannel]->task();
    //run ko update every 500ms
    _currentTaskRun = millis();
    if (_currentTaskRun - _lastTaskRun >= 500) {
        if (hwchannels[m_hwchannel]->isBusy()) { return; }
        if (!hwchannels[m_hwchannel]->updateAvailable()) { return; }
        
        hwchannels[m_hwchannel]->resetUpdateFlag();
        byte value = hwchannels[m_hwchannel]->getCurrentValue();

        if (value != _lastbrightness) {    
            if (value != 0) { 
                knx.getGroupObject(calc_ko_statusonoff).value((bool)1, DPT_Switch);
            } else {
                knx.getGroupObject(calc_ko_statusonoff).value((bool)0, DPT_Switch);
            }
            knx.getGroupObject(calc_ko_statusbrightness).value(value, DPT_Percent_U8);
            _lastbrightness = value;
        }
    _lastTaskRun = millis();
    }
}
