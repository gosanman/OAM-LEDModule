#include "DimChannel_EK.h"

DimChannel_EK::DimChannel_EK(uint8_t index) : DimChannel(index) {
  _index = index;
}

DimChannel_EK::~DimChannel_EK() {}

const std::string DimChannel_EK::name()
{
    return "EK";
}

void DimChannel_EK::setup(uint8_t* hwchannel) 
{
    // Parameter
    m_hwchannel = hwchannel[0];

    m_usedayvalue = ParamEK_UseOnValue;
    m_dayvalue = round(ParamEK_OnBrightness * 2.55);
    m_usenightvalue = ParamEK_UseNightValue;
    m_nightvalue = round(ParamEK_NightBrightness * 2.55);
    m_durationrelativ = getTimeWithPattern(ParamEK_RelativDimTime, ParamEK_RelativDimBase);
    m_durationabsolut = getTimeWithPattern(ParamEK_OnOffTime, ParamEK_OnOffBase);
    m_curve = ParamEK_DimCurve;                               // 0=A, 1=B, 2=C, 3=D, 4=E

    // setup hw channels
    hwchannels[m_hwchannel] = new HWChannel(m_hwchannel);
    hwchannels[m_hwchannel]->setup(m_hwchannel, m_curve, m_durationabsolut, m_durationrelativ);
 
    logDebugP("------------------ DEBUG -------------------");
    logDebugP("Channel Index: %i", _channelIndex);
    logDebugP("KO Switch: %i", calcKoNumber(EK_KoSwitch));
    logDebugP("KO Dim Absolute: %i", calcKoNumber(EK_KoDimAbsolute));
    logDebugP("KO Dim Relativ: %i", calcKoNumber(EK_KoDimRelativ));
    logDebugP("KO Status OnOff: %i", calcKoNumber(EK_KoStatusOnOff));
    logDebugP("KO Status Brightness: %i", calcKoNumber(EK_KoStatusBrightness));
    logDebugP("KO Scene: %i", calcKoNumber(EK_KoSceneNumber));
    logDebugP("HW Port: %i", m_hwchannel);
    logDebugP("PT UseDayValue: %i", m_usedayvalue);
    logDebugP("PT DayBrightness: %i", m_dayvalue);
    logDebugP("PT UseNightValue: %i", m_usenightvalue);
    logDebugP("PT NightBrightness: %i", m_nightvalue);
    logDebugP("PT DurationRelativ: %i", m_durationrelativ);
    logDebugP("PT DurationAbsolut: %i", m_durationabsolut);
    logDebugP("PT Curve: %i", m_curve);
    logDebugP("--------------------------------------------");
}

void DimChannel_EK::processInputKo(GroupObject &ko) 
{
    int channelKo = 0;
    channelKo = (ko.asap() - EK_KoOffset) % EK_KoBlockSize;
    logDebugP("Got SHORT KO %i", channelKo);

    switch (channelKo)
    {
    //Schalten
    case EK_KoSwitch:
        koHandleSwitch(ko);
        break;
    
    //Dimmen Absolut Brightness
    case EK_KoDimAbsolute:
        koHandleDimmAbs(ko);
        break;
    
    //Dimmen Relativ Brightness
    case EK_KoDimRelativ:
        koHandleDimmRel(ko);
        break;
    
    //Szenensteuerung
    case EK_KoSceneNumber:
        koHandleScene(ko);
        break;
    }
}

void DimChannel_EK::koHandleSwitch(GroupObject &ko) 
{
    bool value = ko.value(DPT_Switch);
    if (value) //on
    {
        _currentValueEK = isNight ? (m_usenightvalue ? m_nightvalue : _lastNightValue)
                                  : (m_usedayvalue ? m_dayvalue : _lastDayValue);
        logDebugP(isNight ? "Switch Night - with value %i" : "Switch Day - with value %i", _currentValueEK);
        sendDimValue();
    }
    else //off
    {
        hwchannels[m_hwchannel]->taskSoftOff();
    }
}

void DimChannel_EK::koHandleDimmAbs(GroupObject &ko) 
{
    uint8_t brightness = ko.value(DPT_Percent_U8);
    //uint8_t brightness = ko.value(DPT_Scaling);
    //_currentValueEK = round(brightness * 2.55);
    _currentValueEK = brightness;
    logDebugP("Dim Absolut with value %i", brightness);
    sendDimValue();
}

void DimChannel_EK::koHandleDimmRel(GroupObject &ko) 
{
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

void DimChannel_EK::koHandleScene(GroupObject &ko) {
    uint8_t scene = ko.value(DPT_SceneNumber);
    scene++; //increase value by one
    logDebugP("Scene - Number: %i", scene);
    for (uint8_t i = 0; i < MAXCHANNELSCENE; i++) {
        uint8_t sceneparam = ((int8_t)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_SceneNumberA + i)))));
        if (scene == sceneparam) {
            uint8_t action = ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_SceneActionA + i)))));
            switch (action) {
                case SC_EK_None: 
                    // do nothing
                    break;
                case SC_EK_OnValueDayNight:
                    sendDimValue();
                    break;
                case SC_EK_SetBrightness:
                    _currentValueEK = round(((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_SceneBrightnessA + i))))) * 2.55);
                    sendDimValue();
                    break;
                case SC_EK_Off:
                    hwchannels[m_hwchannel]->taskSoftOff();
                    break;
            }
        }
    }
}

void DimChannel_EK::setDayNight(bool value) {
    isNight = value;
}

void DimChannel_EK::task() 
{
    hwchannels[m_hwchannel]->task();
    //run ko update every 100ms
    _currentUpdateRun = millis();
    if (_currentUpdateRun - _lastUpdatekRun >= 100) {
        updateDimValue();
        _lastUpdatekRun = millis();
    }
}

uint16_t DimChannel_EK::calcKoNumber(int koNum)
{
    return koNum + (EK_KoBlockSize * _channelIndex) + EK_KoOffset;
}

void DimChannel_EK::sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool alwayssend)
{
    GroupObject &ko = knx.getGroupObject(calcKoNumber(koNr));
    if(ko.valueNoSendCompare(value, type)) {
        ko.objectWritten();
    } else if (alwayssend == true) {
        ko.objectWritten();
    }
}

void DimChannel_EK::sendDimValue()
{
    hwchannels[m_hwchannel]->taskNewValue(_currentValueEK);
}

void DimChannel_EK::updateDimValue() 
{
    if (hwchannels[m_hwchannel]->isBusy()) { return; }
    if (hwchannels[m_hwchannel]->updateAvailable()) 
    { 
        hwchannels[m_hwchannel]->resetUpdateFlag();
        uint8_t ek = hwchannels[m_hwchannel]->getCurrentValue();

        (isNight ? _lastNightValue : _lastDayValue) = _currentValueEK;

        if (ek != 0) { 
            sendKoStateOnChange(EK_KoStatusOnOff, (bool)1, DPT_Switch, false);
            sendKoStateOnChange(EK_KoStatusBrightness, _currentValueEK, DPT_Percent_U8, true);

        } else {
            sendKoStateOnChange(EK_KoStatusOnOff, (bool)0, DPT_Switch, false);
            sendKoStateOnChange(EK_KoStatusBrightness, _currentValueEK, DPT_Percent_U8, false);
        }
    }
}

uint32_t DimChannel_EK::getTimeWithPattern(uint16_t time, uint8_t base) 
{
    if (base == TIMEBASE_HOURS && time > 1000) {
        time = 1000; // Begrenzung auf maximal 1000 Stunden
    }

    switch (base) {
        case TIMEBASE_TENTH_SECONDS: return time * 100;
        case TIMEBASE_SECONDS:       return time * 1000;
        case TIMEBASE_MINUTES:       return time * 60000;
        case TIMEBASE_HOURS:         return time * 3600000;
        default:                     return 0;
    }
}