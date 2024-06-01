#include "DimChannel_EK.h"

DimChannel_EK::DimChannel_EK(uint8_t index) : DimChannel(index) {
  _index = index;
}

DimChannel_EK::~DimChannel_EK() {}

const std::string DimChannel_EK::name()
{
    return "DimChannel_EK";
}

void DimChannel_EK::setup(uint8_t* hwchannel, uint16_t startKO) {
    // Parameter
    m_hwchannel = hwchannel[0];
    
    m_startko = startKO;
    m_useonbrightness = ParamAPP_PT_EKUseOnBrightness;
    m_onbrightness = round(ParamAPP_PT_EKOnBrightness * 2.55);
    m_durationrelativ = ParamAPP_PT_EKRelativDimTime * 1000;
    m_durationabsolut = ParamAPP_PT_EKOnOffTime * 100;
    m_curve = ParamAPP_PT_EKDimCurve;                               // 0=A, 1=B, 2=C, 3=D

    // calculate KO Objects and save this
    calc_ko_switch = m_startko + KO_OFFSET_EK_SWITCH;
    calc_ko_dimabsolute = m_startko + KO_OFFSET_EK_DIMABSOLUT;
    calc_ko_dimrelativ = m_startko + KO_OFFSET_EK_DIMRELATIV;
    calc_ko_scene = m_startko + KO_OFFSET_EK_SCENE;
    calc_ko_statusonoff = m_startko + KO_OFFSET_EK_STATUSONOFF;
    calc_ko_statusbrightness = m_startko + KO_OFFSET_EK_STATUSBRIGHTNESS;

    // setup hw channels
    hwchannels[m_hwchannel] = new HWChannel(m_hwchannel);
    hwchannels[m_hwchannel]->setup(m_hwchannel, m_curve, m_durationabsolut, m_durationrelativ);
 
    logDebugP("------------------ DEBUG -------------------");
    logDebugP("KO Switch: %i", calc_ko_switch);
    logDebugP("KO Dim Absolute: %i", calc_ko_dimabsolute);
    logDebugP("KO Dim Relativ: %i", calc_ko_dimrelativ);
    logDebugP("KO Scene: %i", calc_ko_scene);
    logDebugP("KO Status OnOff: %i", calc_ko_statusonoff);
    logDebugP("KO Status Brightness: %i", calc_ko_statusbrightness);
    logDebugP("HW Port: %i", m_hwchannel);
    logDebugP("PT UseOnBrightness: %i", m_useonbrightness);
    logDebugP("PT OnBrightness: %i", m_onbrightness);
    logDebugP("PT DurationRelativ: %i", m_durationrelativ);
    logDebugP("PT DurationAbsolut: %i", m_durationabsolut);
    logDebugP("PT Curve: %i", m_curve);
    logDebugP("--------------------------------------------");
}

void DimChannel_EK::processInputKo(GroupObject &ko) {
    uint16_t asap = ko.asap();
    if (asap == calc_ko_switch)
    {
        bool value = ko.value(DPT_Switch);
        if (m_useonbrightness) { 
                _setbrightness = m_onbrightness;
            } 
            else {
                if (_lastbrightness == 0) { _setbrightness = 255; } else { _setbrightness = _lastbrightness; }
            }
        logDebugP("Switch - %i with value %i", value, _setbrightness);
        if (value) {
            hwchannels[m_hwchannel]->taskNewValue(_setbrightness);
        }
        else {
            hwchannels[m_hwchannel]->taskSoftOff();
        }
    }
    else if (asap == calc_ko_dimabsolute) {
        uint8_t brightness = ko.value(DPT_Percent_U8);
        logDebugP("Dim Absolut with value %i", brightness);
        hwchannels[m_hwchannel]->taskNewValue(brightness);
    }
    else if (asap == calc_ko_dimrelativ) {
        uint8_t direction = ko.value(Dpt(3,7,0));
        uint8_t step = ko.value(Dpt(3,7,1));
        logDebugP("Dim Relativ - Direction: %i, Step: %i", direction, step);
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
    else if (asap == calc_ko_scene) {
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
    }
}

void DimChannel_EK::setDayNight(bool value) {
    isNight = value;
}

void DimChannel_EK::task() {
    hwchannels[m_hwchannel]->task();
    //run ko update every 500ms
    _currentTaskRun = millis();
    if (_currentTaskRun - _lastTaskRun >= 500) {
        updateDimValue();
        _lastTaskRun = millis();
    }
}

void DimChannel_EK::updateDimValue() {
    if (hwchannels[m_hwchannel]->isBusy()) { return; }
    if (!hwchannels[m_hwchannel]->updateAvailable()) { return; }
        
    hwchannels[m_hwchannel]->resetUpdateFlag();
    byte value = hwchannels[m_hwchannel]->getCurrentValue();
    
    if (value != 0) { 
        GroupObject& ko = knx.getGroupObject(calc_ko_statusonoff);
        if(ko.valueNoSendCompare((bool)1, DPT_Switch))
            ko.objectWritten();
    } else {
        GroupObject& ko = knx.getGroupObject(calc_ko_statusonoff);
        if(ko.valueNoSendCompare((bool)0, DPT_Switch))
            ko.objectWritten();
    }
    
    GroupObject& ko = knx.getGroupObject(calc_ko_statusbrightness);
    if(ko.valueNoSendCompare(value, DPT_Percent_U8))
        ko.objectWritten();

    if (value > 0) { _lastbrightness = value; }
}

