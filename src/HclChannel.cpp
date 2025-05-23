#include "HclChannel.h"

const std::string HclChannel::logPrefix()
{
    std::string name = "HCL<";
    name += std::to_string(_index + 1);
    name += ">";
    return name;
}

const uint8_t HclChannel::channelIndex()
{
    return _index;
}

void HclChannel::setup(uint8_t index)
{
    _index = index;
    _type = ParamHCL_Type;
    _isConfigured = _type != PT_hclType_none;
    if (_isConfigured)
    {
        if (_type == PT_hclType_sun)
            logDebugP("Konfiguriert nach Sonnenstand, Parameter: %i/%i K - %i/%i %%", ParamHCL_colorTempMin, ParamHCL_colorTempMax, ParamHCL_briMin, ParamHCL_briMax);
        else if (_type == PT_hclType_time)
            logDebugP("Konfiguriert nach Zeittabelle");
    }
    else
        logDebugP("Nicht Konfiguriert");

    //_lastCheck = _index * 3000;
}

void HclChannel::loop()
{
    if (!_isConfigured || !openknx.sun.isSunCalculatioValid())
        return;

    OpenKNX::TimeOnly _sunRise = openknx.sun.sunRiseLocalTime();
    OpenKNX::TimeOnly _sunSet = openknx.sun.sunSetLocalTime();

    logDebugP("Aktuelle Zeit: %i:%i:%i", openknx.time.getLocalTime().hour, openknx.time.getLocalTime().minute, openknx.time.getLocalTime().second);

    uint16_t minT = ParamHCL_colorTempMin;
    uint8_t minB = ParamHCL_briMin;

    if (openknx.time.getLocalTime().hour < _sunRise.hour || (openknx.time.getLocalTime().hour == _sunRise.hour && openknx.time.getLocalTime().minute < _sunRise.minute))
    {
        logDebugP("Vor Sonnenaufgang %i K (%i:%i)", minT, _sunRise.hour, _sunRise.minute);
        if (ParamHCL_checkTemperature)
            KoHCL_StatusColorTemp.value(minT, Dpt(7, 600));
        if (ParamHCL_checkBrightness)
            KoHCL_StatusBrightness.value(minB, DPT_Scaling);
    }
    else if (openknx.time.getLocalTime().hour > _sunSet.hour || (openknx.time.getLocalTime().hour == _sunSet.hour && openknx.time.getLocalTime().minute > _sunSet.minute))
    {
        logDebugP("Nach Sonnenuntergang %i K (%i:%i)", minT, _sunSet.hour, _sunSet.minute);
        if (ParamHCL_checkTemperature)
            KoHCL_StatusColorTemp.value(minT, Dpt(7, 600));
        if (ParamHCL_checkBrightness)
            KoHCL_StatusBrightness.value(minB, DPT_Scaling);
    }
    else
    {
        logDebugP("Dazwischen %i:%i - jetzt - %i:%i", _sunRise.hour, _sunRise.minute, _sunSet.hour, _sunSet.minute);
        uint16_t startMin = _sunRise.hour * 60 + _sunRise.minute;
        uint16_t stopMin = _sunSet.hour * 60 + _sunSet.minute;

        if (ParamHCL_offsetRiseType == PT_hclOffset_plus)
            startMin += ParamHCL_offsetRiseMin;
        else if (ParamHCL_offsetRiseType == PT_hclOffset_minus)
            startMin -= ParamHCL_offsetRiseMin;

        if (ParamHCL_offsetSetType == PT_hclOffset_plus)
            stopMin += ParamHCL_offsetSetMin;
        else if (ParamHCL_offsetSetType == PT_hclOffset_minus)
            stopMin -= ParamHCL_offsetSetMin;

        uint16_t currentMin = openknx.time.getLocalTime().hour * 60 + openknx.time.getLocalTime().minute;
        // logDebugP("start %i | stop %i | curr %i", startMin, stopMin, currentMin);
        uint16_t response = 0;
        uint16_t maxT = ParamHCL_colorTempMax;
        uint8_t maxB = ParamHCL_briMax;

        if (ParamHCL_checkTemperature)
        {
            response = getValueFromSun(currentMin - startMin, stopMin - startMin, minT, maxT);
            logDebugP("response: %i K", response);
            KoHCL_StatusColorTemp.value(response, Dpt(7, 600));
        }
        if (ParamHCL_checkBrightness)
        {
            response = getValueFromSun(currentMin - startMin, stopMin - startMin, minB, maxB);
            logDebugP("response: %i %", response);
            KoHCL_StatusBrightness.value(response, DPT_Scaling);
        }
    }
}

uint16_t HclChannel::getValueFromSun(uint16_t minCurr, uint16_t minDiff, uint16_t minK, uint16_t maxK)
{
    float xAchse = (minCurr * 3.14159) / minDiff;
    float yAchse = sin(xAchse);
    return (maxK - minK) * yAchse + minK;
}