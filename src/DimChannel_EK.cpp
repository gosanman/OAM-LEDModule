#include "DimChannel_EK.h"

DimChannel_EK::DimChannel_EK(uint8_t index) : DimChannel(index)
{
    _index = index;
}

DimChannel_EK::~DimChannel_EK() {}

const std::string DimChannel_EK::name()
{
    return "EK";
}

void DimChannel_EK::setup(uint8_t *hwchannel)
{
    // Parameter
    m_hwchannel = hwchannel[0];

    m_usedayvalue = ParamEK_UseOnValue;
    m_dayvalue = round(ParamEK_OnBrightness * 2.55);
    m_usenightvalue = ParamEK_UseNightValue;
    m_nightvalue = round(ParamEK_NightBrightness * 2.55);
    m_durationrelativ = LEDHelper::getTimeWithPattern(ParamEK_RelativDimTime, ParamEK_RelativDimBase);
    m_durationabsolut = LEDHelper::getTimeWithPattern(ParamEK_OnOffTime, ParamEK_OnOffBase);
    m_curve = ParamEK_DimCurve; // 0=A, 1=B, 2=C, 3=D, 4=E

    logDebugP("CH: %i, | HW: %i, | Use Day: %i, B: %i, | Use Night: %i, B: %i, Dur Rel: %i, Abs: %i, Curve: %i, | HCL Act: %i, Ch: %i, St: %i",
              _index, m_hwchannel, m_usedayvalue, m_dayvalue, m_usenightvalue, m_nightvalue, m_durationrelativ,
              m_durationabsolut, m_curve, ParamEK_hclActive, ParamEK_hclChannel, ParamEK_hclStart);
}

void DimChannel_EK::processInputKo(GroupObject &ko)
{
    int channelKo = 0;
    channelKo = (ko.asap() - EK_KoOffset) % EK_KoBlockSize;
    logDebugP("Got SHORT KO %i", channelKo);

    switch (channelKo)
    {
    // Schalten
    case EK_KoSwitch:
        koHandleSwitch(ko);
        break;
    // Dimmen Absolut Brightness
    case EK_KoDimAbsolute:
        koHandleDimmAbs(ko);
        break;
    // Dimmen Relativ Brightness
    case EK_KoDimRelativ:
        koHandleDimmRel(ko);
        break;
    // Szenensteuerung
    case EK_KoSceneNumber:
        koHandleScene(ko);
        break;
    }
}

void DimChannel_EK::koHandleSwitch(GroupObject &ko)
{
    bool value = ko.value(DPT_Switch);
    if (value)
    { // on
        switchOnHelper();
        logDebugP(isNight ? "Switch On Night - with value %i" : "Switch On Day - with value %i", _currentValueEK);
        startTask(DimTaskEK::EK_DIM_SOFT_ON);
    }
    else
    { // off
        switchOffHelper();
        logDebugP(isNight ? "Switch Off Night - with value %i" : "Switch Off Day - with value %i", _currentValueEK);
        startTask(DimTaskEK::EK_DIM_SOFT_OFF);
    }
}

void DimChannel_EK::koHandleDimmAbs(GroupObject &ko)
{
    _newValueEK = ko.value(DPT_Percent_U8);
    logDebugP("Dim Absolut - Brightness: %i", _newValueEK);
    startTask(DimTaskEK::EK_DIM_B_SET);
}

void DimChannel_EK::koHandleDimmRel(GroupObject &ko)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0) {
        logDebugP("Dim Relativ - Stop");
        startTask(DimTaskEK::EK_DIM_STOP);
    } else if (direction == 1) {
        logDebugP("Dim Relativ - DimUp");
        startTask(DimTaskEK::EK_DIM_B_UP);
    } else if (direction == 0) {
        logDebugP("Dim Relativ - DimDown");
        startTask(DimTaskEK::EK_DIM_B_DOWN);
    }
}

void DimChannel_EK::koHandleScene(GroupObject &ko)
{
    uint8_t scene = ko.value(DPT_SceneNumber);
    scene++; // increase value by one
    logDebugP("Scene - Number: %i", scene);
    for (uint8_t i = 0; i < MAXCHANNELSCENE; i++)
    {
        uint8_t sceneparam = ((int8_t)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_SceneNumberA + i)))));
        if (scene == sceneparam)
        {
            uint8_t action = ((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_SceneActionA + i)))));
            switch (action)
            {
            case SC_EK_None:
                // do nothing
                break;
            case SC_EK_OnValueDayNight:
                switchOnHelper();
                startTask(DimTaskEK::EK_DIM_SOFT_ON);
                break;
            case SC_EK_SetBrightness:
                _newValueEK = round(((uint)((knx.paramByte((EK_ParamBlockOffset + EK_ParamBlockSize * channelIndex() + EK_SceneBrightnessA + i))))) * 2.55);
                startTask(DimTaskEK::EK_DIM_B_SET);
                break;
            case SC_EK_Off:
                switchOffHelper();
                startTask(DimTaskEK::EK_DIM_SOFT_OFF);
                break;
            }
        }
    }
}

void DimChannel_EK::switchOnHelper()
{
    if (isNight) {
        _newValueEK = m_usenightvalue ? m_nightvalue : _lastNightValue;
    } else {
        _newValueEK = m_usedayvalue ? m_dayvalue : _lastDayValue;
    }
}

void DimChannel_EK::switchOffHelper()
{
    if (isNight) {
        _lastNightValue = _currentValueEK;
    } else {
        _lastDayValue = _currentValueEK;
    }
}

void DimChannel_EK::setDayNight(bool value)
{
    isNight = value;
}

std::vector<uint8_t> DimChannel_EK::getHWPorts()
{
    std::vector<uint8_t> ports;
    ports.push_back(m_hwchannel);
    return ports;
}

uint8_t DimChannel_EK::getChannelIndex()
{
    return _index;
}

uint8_t DimChannel_EK::getChannelType()
{
    return ChannelType::EK; // 1 = EK
}

void DimChannel_EK::setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness)
{
    if (ParamEK_hclActive != 1 || channel != ParamEK_hclChannel)
        return;
    if (ParamEK_hclStart == PT_hclStart_during) { // HCL active when channel is on
        if (ParamEK_hclCheckBrightness == 1 && _isOn) {
            logDebugP("HCL active - Channel: %i Kelvin: %i Brightness: %i", channel, kelvin, brightness);
            _newValueEK = round((uint)(brightness * 2.55));
            startTask(DimTaskEK::EK_DIM_B_SET);
        } else {
            _currentHclValue[0] = brightness;
            _currentHclValue[1] = kelvin;
            logDebugP("HCL will only apply if channel on, save for later use");
        }
    }
}

void DimChannel_EK::task()
{
    dimmerTask();
}

uint16_t DimChannel_EK::calcKoNumber(int koNum)
{
    return koNum + (EK_KoBlockSize * _channelIndex) + EK_KoOffset;
}

void DimChannel_EK::sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool alwayssend)
{
    GroupObject &ko = knx.getGroupObject(calcKoNumber(koNr));
    if (ko.valueNoSendCompare(value, type)) {
        ko.objectWritten();
    } else if (alwayssend == true) {
        ko.objectWritten();
    }
}

void DimChannel_EK::updateDimValue()
{
    _isOn = _currentValueEK > 0;
    logDebugP("Send DimValue to KO - OnOff: %i B: %i", _isOn, _currentValueEK);
    sendKoStateOnChange(EK_KoStatusOnOff, _isOn, DPT_Switch, false);
    sendKoStateOnChange(EK_KoStatusBrightness, _currentValueEK, DPT_Percent_U8, true);
}

//----------------------------- TW Dimmer Task ------------------------------

// Neue Aufgabe starten: _busy zuruecksetzen, damit handleDimGeneric _time
// fuer die neue Rampe (neues Ziel/Dauer) frisch berechnet, auch wenn eine
// laufende Rampe unterbrochen wird.
void DimChannel_EK::startTask(uint8_t task)
{
    _currentTask = task;
    _busy = false;
}

void DimChannel_EK::dimmerTask()
{
    _currentMillis = millis();
    switch (_currentTask)
    {
    case DimTaskEK::EK_DIM_STOP:
        handleDimStop();
        break;
    case DimTaskEK::EK_DIM_SOFT_ON:
        handleDimSoftOn();
        break;
    case DimTaskEK::EK_DIM_SOFT_OFF:
        handleDimSoftOff();
        break;
    case DimTaskEK::EK_DIM_B_SET:
        handleDimSetBrightness();
        break;
    case DimTaskEK::EK_DIM_B_UP:
        handleDimBrightnessUp();
        break;
    case DimTaskEK::EK_DIM_B_DOWN:
        handleDimBrightnessDown();
        break;
    case DimTaskEK::EK_DIM_IDLE:
    default:
        break;
    }
}

void DimChannel_EK::sendDimValue()
{
    LEDModule::_instance->setHwChannelValue(m_hwchannel, _currentValueEK, m_curve);
}

void DimChannel_EK::handleDimGeneric(uint8_t &currentValue, uint8_t targetValue, uint8_t minValue, uint8_t maxValue, bool isAbsolute)
{
    if (currentValue == targetValue) {
        startTask(DimTaskEK::EK_DIM_STOP);
        return;
    }
    if (!_busy) {
        uint32_t duration = isAbsolute ? m_durationabsolut : m_durationrelativ;
        uint16_t delta = abs((int)targetValue - (int)currentValue);
        _time = duration / delta;
    }
    if (_currentMillis - _lastTaskExecution >= _time) {
        if (currentValue < targetValue && currentValue < maxValue) {
            currentValue++;
        } else if (currentValue > targetValue && currentValue > minValue) {
            currentValue--;
        } else {
            startTask(DimTaskEK::EK_DIM_STOP);
            return;
        }
        _busy = true;
        sendDimValue();
        _lastTaskExecution = millis();
    }
}

void DimChannel_EK::handleDimStop()
{
    _busy = false;
    _currentTask = DimTaskEK::EK_DIM_IDLE;
    updateDimValue();
}

void DimChannel_EK::handleDimSoftOn() {
    handleDimGeneric(_currentValueEK, _newValueEK, _valueMinBrightness, _valueMaxBrightness, true);
}

void DimChannel_EK::handleDimSoftOff() {
    handleDimGeneric(_currentValueEK, _valueMinBrightness, _valueMinBrightness, _valueMaxBrightness, true);
}

void DimChannel_EK::handleDimSetBrightness() {
    handleDimGeneric(_currentValueEK, _newValueEK, _valueMinBrightness, _valueMaxBrightness, true);
}

void DimChannel_EK::handleDimBrightnessUp() {
    handleDimGeneric(_currentValueEK, _valueMaxBrightness, _valueMinBrightness, _valueMaxBrightness, false);
}

void DimChannel_EK::handleDimBrightnessDown() {
    handleDimGeneric(_currentValueEK, _valueMinBrightness, _valueMinBrightness, _valueMaxBrightness, false);
}