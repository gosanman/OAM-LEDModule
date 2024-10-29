#include "DimChannel_TW.h"

DimChannel_TW::DimChannel_TW(uint8_t index) : DimChannel(index) {
  _index = index;
}

DimChannel_TW::~DimChannel_TW() {}

const std::string DimChannel_TW::name()
{
    return "TW";
}

void DimChannel_TW::setup(uint8_t* hwchannel) 
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
    m_durationrelativ = getTimeWithPattern(ParamTW_RelativDimTime, ParamTW_RelativDimBase);
    m_durationabsolut = getTimeWithPattern(ParamTW_OnOffTime, ParamTW_OnOffBase);
    m_curve = ParamTW_DimCurve;                               // 0=A, 1=B, 2=C, 3=D, 4=E

    // setup hw channels
    hwchannels[m_hwchannel_ww] = new HWChannel(m_hwchannel_ww);
    hwchannels[m_hwchannel_cw] = new HWChannel(m_hwchannel_cw);
    hwchannels[m_hwchannel_ww]->setup(m_hwchannel_ww, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_cw]->setup(m_hwchannel_cw, m_curve, m_durationabsolut, m_durationrelativ);

    logDebugP("------------------ DEBUG -------------------");
    logDebugP("Channel Index: %i", _channelIndex);
    logDebugP("KO Switch: %i", calcKoNumber(TW_KoSwitch));
    logDebugP("KO Dim Absolute Brightness: %i", calcKoNumber(TW_KoDimAbsoluteBrightness));
    logDebugP("KO Dim Absolute Kelvin: %i", calcKoNumber(TW_KoDimAbsoluteColorTemp));
    logDebugP("KO Dim Relativ Brightness: %i", calcKoNumber(TW_KoDimRelativBrightness));
    logDebugP("KO Dim Relativ KW: %i", calcKoNumber(TW_KoDimRelativKW));
    logDebugP("KO Status OnOff: %i", calcKoNumber(TW_KoStatusOnOff));
    logDebugP("KO Status Brightness: %i", calcKoNumber(TW_KoStatusBrightness));
    logDebugP("KO Status Kelvin: %i", calcKoNumber(TW_KoStatusColorTemp));
    logDebugP("KO Status KW: %i", calcKoNumber(TW_KoStatusKW));
    logDebugP("KO Scene: %i", calcKoNumber(TW_KoSceneNumber));
    logDebugP("HW Port WW: %i", m_hwchannel_ww);
    logDebugP("HW Port CW: %i", m_hwchannel_cw);
    logDebugP("PT ColorTemp WW: %i K", m_colortempww);
    logDebugP("PT ColorTemp CW: %i K", m_colortempcw);
    logDebugP("PT UseDayValue: %i", m_usedayvalue);
    logDebugP("PT DayBrightness: %i", m_daybrightness);
    logDebugP("PT DayColorTemp: %i K", m_daycolortemp);
    logDebugP("PT UseNightValue: %i", m_usenightvalue);
    logDebugP("PT NightBrightness: %i", m_nightbrightness);
    logDebugP("PT NightColorTemp: %i K", m_nightcolortemp);
    logDebugP("PT DurationRelativ: %i", m_durationrelativ);
    logDebugP("PT DurationAbsolut: %i", m_durationabsolut);
    logDebugP("PT Curve: %i", m_curve);
    logDebugP("--------------------------------------------");
}

void DimChannel_TW::processInputKo(GroupObject &ko) 
{
    int channelKo = 0;
    channelKo = (ko.asap() - TW_KoOffset) % TW_KoBlockSize;
    logDebugP("Got SHORT KO %i", channelKo);

    switch (channelKo)
    {
    //Schalten
    case TW_KoSwitch:
        koHandleSwitch(ko);
        break;

    //Dimmen Absolut Brightness
    case TW_KoDimAbsoluteBrightness:
        koHandleDimmAbsBrightness(ko);
        break;

    //Dimmen Absolut ColorTemp
    case TW_KoDimAbsoluteColorTemp:
        koHandleDimmAbsColorTemp(ko);
        break;

    //Dimmen Relative Brightness
    case TW_KoDimRelativBrightness:
        koHandleDimmRelBrightness(ko);
        break;

    //Dimmen Relative KW
    case TW_KoDimRelativKW:
        koHandleDimmRelKw(ko);
        break;

    //Szenensteuerung
    case TW_KoSceneNumber:
        koHandleScene(ko);
        break;
    }
}

void DimChannel_TW::koHandleSwitch(GroupObject &ko) 
{
    bool value = ko.value(DPT_Switch);
    if (value) //on
    {
        if (isNight) 
        {
            if (m_usenightvalue) {
                _currentValueTW[0] = m_nightbrightness;
                _currentValueTW[1] = m_nightcolortemp;
            } else {
                _currentValueTW[0] = _lastNightValue[0];
                _currentValueTW[1] = _lastNightValue[1];                
            }   
        } else {
            if (m_usedayvalue) {
                _currentValueTW[0] = m_daybrightness;
                _currentValueTW[1] = m_daycolortemp;                
            } else {
                _currentValueTW[0] = _lastDayValue[0];
                _currentValueTW[1] = _lastDayValue[1];                  
            }
        }
        logDebugP(isNight ? "Switch Night - with value: %i - Kelvin: %i - Brightness: %i" : "Switch Day - with value: %i - Kelvin: %i - Brightness: %i", value, _currentValueTW[1], _currentValueTW[0]);
        sendDimValue();
    }
    else 
    { // off
        hwchannels[m_hwchannel_ww]->taskSoftOff();
        hwchannels[m_hwchannel_cw]->taskSoftOff();   
    }
}

void DimChannel_TW::koHandleDimmAbsBrightness(GroupObject &ko) 
{
    uint8_t brightness = ko.value(DPT_Percent_U8);
    //double originalValue = (brightness / 100.0) * 255.0;
    //_currentValueTW[0] = (uint8_t)round(originalValue);
    //_currentValueTW[0] = round(brightness * 2.55);
    _currentValueTW[0] = brightness;
    logDebugP("Dim Absolute - Kelvin: %i - Brightness: %i", _currentValueTW[1], _currentValueTW[0]);
    sendDimValue();
}

void DimChannel_TW::koHandleDimmAbsColorTemp(GroupObject &ko) 
{
    uint16_t kelvin = ko.value(Dpt(7, 600));
    _currentValueTW[1] = kelvin;
    logDebugP("Dim Kelvin - Kelvin: %i - Brightness: %i", _currentValueTW[1], _currentValueTW[0]);
    sendDimValue();
}

void DimChannel_TW::koHandleDimmRelBrightness(GroupObject &ko) 
{
    uint8_t direction = ko.value(Dpt(3,7,0));
    uint8_t step = ko.value(Dpt(3,7,1));
    logDebugP("Dim Relativ - Direction: %i, Step: %i", direction, step);
    //direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        hwchannels[m_hwchannel_ww]->taskStop();
        hwchannels[m_hwchannel_cw]->taskStop();
        // To-Do
    }
    else if (direction == 1) {
        logDebugP("Dim Relativ - DimUp");
        // TO-DO
    }
    else if (direction == 0) {
        logDebugP("Dim Relativ - DimDown");
        // To-Do
    }
}

void DimChannel_TW::koHandleDimmRelKw(GroupObject &ko) 
{
    uint8_t direction = ko.value(Dpt(3,7,0));
    uint8_t step = ko.value(Dpt(3,7,1));
    logDebugP("Dim Relativ - Direction: %i, Step: %i", direction, step);
    //direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        hwchannels[m_hwchannel_cw]->taskStop();
        // To-DO - Update für Kelvin muss noch berechnet werden
    }
    else if (direction == 1) {
        logDebugP("Dim Relativ - DimUp");
        hwchannels[m_hwchannel_cw]->taskDimUp();
    }
    else if (direction == 0) {
        logDebugP("Dim Relativ - DimDown");
        hwchannels[m_hwchannel_cw]->taskDimDown();
    }
}

void DimChannel_TW::koHandleScene(GroupObject &ko) {
    uint8_t scene = ko.value(DPT_SceneNumber);
    scene++; //increase value by one
    logDebugP("Scene - Number: %i", scene);
    for (uint8_t i = 0; i < MAXCHANNELSCENE; i++) {
        uint8_t sceneparam = ((int8_t)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneNumberA + i)))));
        if (scene == sceneparam) {
            uint8_t action = ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneActionA + i)))));
            switch (action) {
                case SC_TW_None:
                     // do nothing
                    break;
                case SC_TW_OnValueDayNight:
                    sendDimValue();
                    break;                   
                case SC_TW_SetBrightness:
                    _currentValueTW[0] = round(((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneBrightnessA + i))))) * 2.55);
                    sendDimValue();
                    break;
                case SC_TW_SetColorTemp:
                    _currentValueTW[1] = ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneKelvinA + (i * 2))))));
                    sendDimValue();
                    break;
                case SC_TW_SetBoth:
                    _currentValueTW[0] = round(((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneBrightnessA + i))))) * 2.55);
                    _currentValueTW[1] = ((uint)((knx.paramWord((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneKelvinA + (i * 2))))));
                    sendDimValue();
                    break;
                case SC_EK_Off:
                    hwchannels[m_hwchannel_ww]->taskSoftOff();
                    hwchannels[m_hwchannel_cw]->taskSoftOff(); 
                    break;
            }
        }
    }
}

void DimChannel_TW::setDayNight(bool value) {
    isNight = value;
}

void DimChannel_TW::task() {
    hwchannels[m_hwchannel_ww]->task();
    hwchannels[m_hwchannel_cw]->task();
    //run ko update every 100ms
    _currentUpdateRun = millis();
    if (_currentUpdateRun - _lastUpdatekRun >= 100) {
        updateDimValue();
        _lastUpdatekRun = millis();
    }
}

uint16_t DimChannel_TW::calcKoNumber(int koNum)
{
    return koNum + (TW_KoBlockSize * _channelIndex) + TW_KoOffset;
}

void DimChannel_TW::sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type)
{
    GroupObject &ko = knx.getGroupObject(calcKoNumber(koNr));
    if(ko.valueNoSendCompare(value, type))
        ko.objectWritten();
}

void DimChannel_TW::sendDimValue()
{
    uint8_t percent = prozToDim(_currentValueTW[0], 3);
    uint8_t percentWW = (percent*(m_colortempcw - _currentValueTW[1]))/(m_colortempcw - m_colortempww);
    uint8_t percentCW = (percent*(_currentValueTW[1] - m_colortempww))/(m_colortempcw - m_colortempww);
    logDebugP("protzToDim Correction Value - WW: %i CW: %i", percentWW, percentCW);
    hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
    hwchannels[m_hwchannel_cw]->taskNewValue(percentCW);
}

void DimChannel_TW::updateDimValue() {
    if (hwchannels[m_hwchannel_ww]->isBusy() || hwchannels[m_hwchannel_cw]->isBusy()) { return; }
    if (hwchannels[m_hwchannel_ww]->updateAvailable() || hwchannels[m_hwchannel_cw]->updateAvailable()) 
    {
        hwchannels[m_hwchannel_ww]->resetUpdateFlag();
        hwchannels[m_hwchannel_cw]->resetUpdateFlag();
        uint8_t ww = hwchannels[m_hwchannel_ww]->getCurrentValue();
        uint8_t cw = hwchannels[m_hwchannel_cw]->getCurrentValue();

        if (ww != 0 || cw != 0) {
            sendKoStateOnChange(TW_KoStatusOnOff, (bool)1, DPT_Switch);
            //To-Do - Berechnung von Kelvin und Brightness aus WW sowie CM
        } else {
            sendKoStateOnChange(TW_KoStatusOnOff, (bool)0, DPT_Switch);
        }

        if (isNight) {
            _lastNightValue[0] = _currentValueTW[0];  
            _lastNightValue[1] = _currentValueTW[1];
        } else {
            _lastDayValue[0] = _currentValueTW[0];
            _lastDayValue[1] = _currentValueTW[1];
        }

        //sendKoStateOnChange(TW_KoStatusBrightness, (uint8_t)round(_currentValueTW[0] / 2.55), DPT_Scaling);
        sendKoStateOnChange(TW_KoStatusBrightness, _currentValueTW[0], DPT_Percent_U8);
        sendKoStateOnChange(TW_KoStatusColorTemp, _currentValueTW[1], Dpt(7, 600));
        //sendKoStateOnChange(TW_KoStatusKW, (uint8_t)round(cw / 2.55), DPT_Scaling);
        sendKoStateOnChange(TW_KoStatusKW, cw, DPT_Percent_U8);
    }
}

uint16_t DimChannel_TW::prozToDim(uint8_t value, uint8_t curve) {
    return proztable[value][curve];
} 

uint32_t DimChannel_TW::getTimeWithPattern(uint16_t time, uint8_t base)
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