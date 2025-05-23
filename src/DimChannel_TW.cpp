#include "DimChannel_TW.h"

DimChannel_TW::DimChannel_TW(uint8_t index) : DimChannel(index)
{
    _index = index;
}

DimChannel_TW::~DimChannel_TW() {}

const std::string DimChannel_TW::name()
{
    return "TW";
}

void DimChannel_TW::setup(uint8_t *hwchannel)
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
    m_curve = ParamTW_DimCurve; // 0=A, 1=B, 2=C, 3=D, 4=E

    // setup hw channels
    hwchannels[m_hwchannel_ww] = new HWChannel(m_hwchannel_ww);
    hwchannels[m_hwchannel_cw] = new HWChannel(m_hwchannel_cw);
    hwchannels[m_hwchannel_ww]->setup(m_hwchannel_ww, m_curve, m_durationabsolut, m_durationrelativ);
    hwchannels[m_hwchannel_cw]->setup(m_hwchannel_cw, m_curve, m_durationabsolut, m_durationrelativ);

    logDebugP("CH: %i, | HW WW: %i, CW: %i, | CT WW: %i, CW: %i, | Use Day: %i, B: %i, K: %i, | Use Night: %i, B: %i, K: %i, | Dur Rel: %i, Abs: %i, Curve: %i, | HCL Act: %i, Ch: %i, St: %i",
              _index, m_hwchannel_ww, m_hwchannel_cw, m_colortempww, m_colortempcw, m_usedayvalue, m_daybrightness, m_daycolortemp,
              m_usenightvalue, m_nightbrightness, m_nightcolortemp, m_durationrelativ, m_durationabsolut, m_curve, ParamTW_hclActive,
              ParamTW_hclChannel, ParamTW_hclStart);
}

void DimChannel_TW::processInputKo(GroupObject &ko)
{
    int channelKo = 0;
    channelKo = (ko.asap() - TW_KoOffset) % TW_KoBlockSize;
    logDebugP("Got SHORT KO %i", channelKo);

    switch (channelKo)
    {
    // Schalten
    case TW_KoSwitch:
        koHandleSwitch(ko);
        break;
    // Dimmen Absolut Brightness
    case TW_KoDimAbsoluteBrightness:
        koHandleDimmAbsBrightness(ko);
        break;
    // Dimmen Absolut ColorTemp
    case TW_KoDimAbsoluteColorTemp:
        koHandleDimmAbsColorTemp(ko);
        break;
    // Dimmen Relative Brightness
    case TW_KoDimRelativBrightness:
        koHandleDimmRelBrightness(ko);
        break;
    // Dimmen Relative ColorTemp
    case TW_KoDimRelativColorTemp:
        koHandleDimmRelColorTemp(ko);
        break;
    // Szenensteuerung
    case TW_KoSceneNumber:
        koHandleScene(ko);
        break;
    }
}

void DimChannel_TW::koHandleSwitch(GroupObject &ko)
{
    bool value = ko.value(DPT_Switch);
    if (value)
    { // on
        if (isNight)
        {
            if (m_usenightvalue)
            {
                _currentValueTW[0] = m_nightbrightness;
                _currentValueTW[1] = m_nightcolortemp;
            }
            else
            {
                _currentValueTW[0] = _lastNightValue[0];
                _currentValueTW[1] = _lastNightValue[1];
            }
        }
        else
        {
            if (m_usedayvalue)
            {
                _currentValueTW[0] = m_daybrightness;
                _currentValueTW[1] = m_daycolortemp;
            }
            else
            {
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
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ Brightness - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0)
    {
        logDebugP("Dim Relativ Brightness - Stop");
        _currentTask = DimTaskTW::TW_DIM_STOP;
    }
    else if (direction == 1)
    {
        logDebugP("Dim Relativ Brightness - DimUp");
        _currentTask = DimTaskTW::TW_DIM_B_UP;
    }
    else if (direction == 0)
    {
        logDebugP("Dim Relativ Brightness - DimDown");
        _currentTask = DimTaskTW::TW_DIM_B_DOWN;
    }
}

void DimChannel_TW::koHandleDimmRelColorTemp(GroupObject &ko)
{
    uint8_t direction = ko.value(Dpt(3, 7, 0));
    uint8_t step = ko.value(Dpt(3, 7, 1));
    logDebugP("Dim Relativ Kelvin - Direction: %i, Step: %i", direction, step);
    // direction true = dim up, false = dim down, step = 0 then stop
    if (step == 0)
    {
        logDebugP("Dim Relativ Kelvin - Stop");
        _currentTask = DimTaskTW::TW_DIM_STOP;
    }
    else if (direction == 1)
    {
        logDebugP("Dim Relativ Kelvin - DimUp");
        _currentTask = DimTaskTW::TW_DIM_K_UP;
    }
    else if (direction == 0)
    {
        logDebugP("Dim Relativ Kelvin - DimDown");
        _currentTask = DimTaskTW::TW_DIM_K_DOWN;
    }
}

void DimChannel_TW::koHandleScene(GroupObject &ko)
{
    uint8_t scene = ko.value(DPT_SceneNumber);
    scene++; // increase value by one
    logDebugP("Scene - Number: %i", scene);
    for (uint8_t i = 0; i < MAXCHANNELSCENE; i++)
    {
        uint8_t sceneparam = ((int8_t)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneNumberA + i)))));
        if (scene == sceneparam)
        {
            uint8_t action = ((uint)((knx.paramByte((TW_ParamBlockOffset + TW_ParamBlockSize * channelIndex() + TW_SceneActionA + i)))));
            switch (action)
            {
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

void DimChannel_TW::setDayNight(bool value)
{
    isNight = value;
}

std::vector<uint8_t> DimChannel_TW::getHWPorts()
{
    std::vector<uint8_t> ports;
    ports.push_back(m_hwchannel_ww);
    ports.push_back(m_hwchannel_cw);
    return ports;
}

uint8_t DimChannel_TW::getChannelIndex()
{
    return _index;
}

uint8_t DimChannel_TW::getChannelType()
{
    return ChannelType::TW; // 2 = TW
}

void DimChannel_TW::setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness)
{
    if (ParamTW_hclActive != 1 || channel != ParamTW_hclChannel)
        return;
    if (brightness != 0) _currentValueTW[0] = brightness;
    if (kelvin != 0) _currentValueTW[1] = kelvin;
}

void DimChannel_TW::task()
{
    hwchannels[m_hwchannel_ww]->task();
    hwchannels[m_hwchannel_cw]->task();
    dimmerTask();
    // run ko update every 100ms
    _currentUpdateRun = millis();
    if (_currentUpdateRun - _lastUpdatekRun >= 100)
    {
        updateDimValue();
        _lastUpdatekRun = millis();
    }
}

uint16_t DimChannel_TW::calcKoNumber(int koNum)
{
    return koNum + (TW_KoBlockSize * _channelIndex) + TW_KoOffset;
}

void DimChannel_TW::sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool alwayssend)
{
    GroupObject &ko = knx.getGroupObject(calcKoNumber(koNr));
    if (ko.valueNoSendCompare(value, type))
    {
        ko.objectWritten();
    }
    else if (alwayssend == true)
    {
        ko.objectWritten();
    }
}

void DimChannel_TW::sendDimValue()
{
    percent = prozToDim(_currentValueTW[0], 3);
    percentWW = (percent * (m_colortempcw - _currentValueTW[1])) / (m_colortempcw - m_colortempww);
    percentCW = (percent * (_currentValueTW[1] - m_colortempww)) / (m_colortempcw - m_colortempww);
    // logDebugP("protzToDim Correction Value - WW: %i CW: %i", percentWW, percentCW);
    hwchannels[m_hwchannel_ww]->taskNewValue(percentWW);
    hwchannels[m_hwchannel_cw]->taskNewValue(percentCW);
}

void DimChannel_TW::setDimValue()
{
    percent = prozToDim(_currentValueTW[0], 3);
    percentWW = (percent * (m_colortempcw - _currentValueTW[1])) / (m_colortempcw - m_colortempww);
    percentCW = (percent * (_currentValueTW[1] - m_colortempww)) / (m_colortempcw - m_colortempww);
    // logDebugP("protzToDim Correction Value - WW: %i CW: %i", percentWW, percentCW);
    hwchannels[m_hwchannel_ww]->taskSetValue(percentWW);
    hwchannels[m_hwchannel_cw]->taskSetValue(percentCW);
}

void DimChannel_TW::updateDimValue()
{
    if (_busy == true)
    {
        return;
    }
    if (hwchannels[m_hwchannel_ww]->isBusy() || hwchannels[m_hwchannel_cw]->isBusy())
    {
        return;
    }
    if (hwchannels[m_hwchannel_ww]->updateAvailable() || hwchannels[m_hwchannel_cw]->updateAvailable())
    {
        hwchannels[m_hwchannel_ww]->resetUpdateFlag();
        hwchannels[m_hwchannel_cw]->resetUpdateFlag();
        uint8_t ww = hwchannels[m_hwchannel_ww]->getCurrentValue();
        uint8_t cw = hwchannels[m_hwchannel_cw]->getCurrentValue();

        if (isNight)
        {
            _lastNightValue[0] = _currentValueTW[0];
            _lastNightValue[1] = _currentValueTW[1];
        }
        else
        {
            _lastDayValue[0] = _currentValueTW[0];
            _lastDayValue[1] = _currentValueTW[1];
        }
        logDebugP("Send DimValue to KO");
        if (ww != 0 || cw != 0)
        {
            sendKoStateOnChange(TW_KoStatusOnOff, (bool)1, DPT_Switch, false);
            sendKoStateOnChange(TW_KoStatusBrightness, _currentValueTW[0], DPT_Percent_U8, true);
            sendKoStateOnChange(TW_KoStatusColorTemp, _currentValueTW[1], Dpt(7, 600), true);
        }
        else
        {
            sendKoStateOnChange(TW_KoStatusOnOff, (bool)0, DPT_Switch, false);
            sendKoStateOnChange(TW_KoStatusBrightness, (uint8_t)0, DPT_Percent_U8, false);
            sendKoStateOnChange(TW_KoStatusColorTemp, _currentValueTW[1], Dpt(7, 600), false);
        }
    }
}

uint16_t DimChannel_TW::prozToDim(uint8_t value, uint8_t curve)
{
    return proztable[value][curve];
}

uint32_t DimChannel_TW::getTimeWithPattern(uint16_t time, uint8_t base)
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

//----------------------------- TW Dimmer Task ------------------------------

void DimChannel_TW::dimmerTask() {
    _currentMillis = millis();
    switch (_currentTask) {
        case DimTaskTW::TW_DIM_STOP:
            handleDimStop();
            break;
        case DimTaskTW::TW_DIM_B_UP:
            handleDimBrightnessUp();
            break;
        case DimTaskTW::TW_DIM_B_DOWN:
            handleDimBrightnessDown();
            break;
        case DimTaskTW::TW_DIM_K_UP:
            handleDimColorTempUp();
            break;
        case DimTaskTW::TW_DIM_K_DOWN:
            handleDimColorTempDown();
            break;
        case DimTaskTW::TW_DIM_IDLE:
        default:
            break;
    }
}

void DimChannel_TW::handleDimStop() {
    _busy = false;
    _updateAvailable = true;
    _updateCounter = 0;
    _currentTask = DimTaskTW::TW_DIM_IDLE;
}

void DimChannel_TW::handleDimBrightnessUp() {
    if (_currentValueTW[0] < _valueMaxBrightness) {
        if (!_busy) {
            _delayRelative = (word)(m_durationrelativ / (_valueMaxBrightness - _currentValueTW[0]));
        }
        if (_currentMillis - _lastTaskExecution >= _delayRelative) {
            _currentValueTW[0]++;
            _busy = true;
            setDimValue();
            _lastTaskExecution = millis();
        }
    } else {
        _currentTask = DimTaskTW::TW_DIM_STOP;
    }
}

void DimChannel_TW::handleDimBrightnessDown() {
    if (_currentValueTW[0] > _valueMinBrightness) {
        if (!_busy) {
            _delayRelative = (word)(m_durationrelativ / (_currentValueTW[0] - _valueMinBrightness));
        }
        if (_currentMillis - _lastTaskExecution >= _delayRelative) {
            _currentValueTW[0]--;
            _busy = true;
            setDimValue();
            _lastTaskExecution = millis();
        }
    } else {
        _currentTask = DimTaskTW::TW_DIM_STOP;
    }
}

void DimChannel_TW::handleDimColorTempUp() {
    if (_currentValueTW[1] < m_colortempcw) {
        if (!_busy) {
            _delayRelative = (word)(m_durationrelativ / (m_colortempcw - _currentValueTW[1]));
        }
        if (_currentMillis - _lastTaskExecution >= _delayRelative) {
            _currentValueTW[1]++;
            _busy = true;
            setDimValue();
            _lastTaskExecution = millis();
        }
    } else {
        _currentTask = DimTaskTW::TW_DIM_STOP;
    }
}

void DimChannel_TW::handleDimColorTempDown() {
    if (_currentValueTW[1] > m_colortempww) {
        if (!_busy) {
            _delayRelative = (word)(m_durationrelativ / (_currentValueTW[1] - m_colortempww));
        }
        if (_currentMillis - _lastTaskExecution >= _delayRelative) {
            _currentValueTW[1]--;
            _busy = true;
            setDimValue();
            _lastTaskExecution = millis();
        }
    } else {
        _currentTask = DimTaskTW::TW_DIM_STOP;
    }
}










/*

void DimChannel_TW::dimmerTask()
{
    _currentMillis = millis();
    switch (_currentTask)
    {
    // stop all activities
    case TW_DIM_STOP:
        _busy = false;
        _currentTask = DIM_IDLE;
        logDebugP("Dim Relativ - Stop");
        break;
    // increase value brightness
    case TW_DIM_B_UP:
        if (_currentValueTW[0] < _valueMaxBrightness)
        {
            if (!_busy)
            {
                _delayRelative = (word)(m_durationrelativ / (_valueMaxBrightness - _currentValueTW[0]));
            }
            if (_currentMillis - _lastTaskExecution >= _delayRelative)
            {
                _currentValueTW[0]++;
                _busy = true;
                setDimValue();
                _lastTaskExecution = millis();
            }
        }
        else
        {
            _currentTask = TW_DIM_STOP;
        }
        break;
    // decrease value brightness
    case TW_DIM_B_DOWN:
        if (_currentValueTW[0] > _valueMinBrightness)
        {
            if (!_busy)
            {
                _delayRelative = (word)(m_durationrelativ / (_currentValueTW[0] - _valueMinBrightness));
            }
            if (_currentMillis - _lastTaskExecution >= _delayRelative)
            {
                _currentValueTW[0]--;
                _busy = true;
                setDimValue();
                _lastTaskExecution = millis();
            }
        }
        else
        {
            _currentTask = TW_DIM_STOP;
        }
        break;
    // increase value kelvin
    case TW_DIM_K_UP:
        if (_currentValueTW[1] < m_colortempcw)
        {
            if (!_busy)
            {
                _delayRelative = (word)(m_durationrelativ / (m_colortempcw - _currentValueTW[1]));
            }
            if (_currentMillis - _lastTaskExecution >= _delayRelative)
            {
                _currentValueTW[1]++;
                _busy = true;
                setDimValue();
                _lastTaskExecution = millis();
            }
        }
        else
        {
            _currentTask = TW_DIM_STOP;
        }
        break;
    // decrease value kelvin
    case TW_DIM_K_DOWN:
        if (_currentValueTW[1] > m_colortempww)
        {
            if (!_busy)
            {
                _delayRelative = (word)(m_durationrelativ / (_currentValueTW[1] - m_colortempww));
            }
            if (_currentMillis - _lastTaskExecution >= _delayRelative)
            {
                _currentValueTW[1]--;
                _busy = true;
                setDimValue();
                _lastTaskExecution = millis();
            }
        }
        else
        {
            _currentTask = TW_DIM_STOP;
        }
        break;
    case DIM_IDLE:
    default:
        break;
    }
}

*/