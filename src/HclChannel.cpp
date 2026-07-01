#include "HclChannel.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr uint16_t MINUTES_PER_DAY = 1440;
}

std::string HclChannel::logPrefix()
{
    std::string name = "HCL<";
    name += std::to_string(_index + 1);
    name += ">";
    return name;
}

uint8_t HclChannel::channelIndex()
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
            logDebugP("Konfiguriert nach Zeittabelle - Start: %i:%i hh:mm / Ende: %i:%i hh:mm, Parameter: %i/%i K - %i/%i %%", ParamHCL_startTimeHour, ParamHCL_startTimeMinute, ParamHCL_endTimeHour, ParamHCL_endTimeMinute, ParamHCL_colorTempMin, ParamHCL_colorTempMax, ParamHCL_briMin, ParamHCL_briMax);
    }
    else
    {
        logDebugP("Nicht konfiguriert");
    }
}

void HclChannel::loop(uint16_t &out_k, uint8_t &out_b)
{
    if (!_isConfigured || !openknx.sun.isSunCalculatioValid())
        return;

    uint16_t minT = ParamHCL_colorTempMin;
    uint16_t maxT = ParamHCL_colorTempMax;
    uint8_t minB = ParamHCL_briMin;
    uint8_t maxB = ParamHCL_briMax;
    uint16_t currentMin = openknx.time.getLocalTime().hour * 60 + openknx.time.getLocalTime().minute;

    uint16_t response_k = minT;
    uint8_t response_b = minB;

    if (_type == PT_hclType_sun)
    {
        OpenKNX::TimeOnly sunRise = openknx.sun.sunRiseLocalTime();
        OpenKNX::TimeOnly sunSet = openknx.sun.sunSetLocalTime();

        uint16_t startMin = applyOffset(sunRise.hour * 60 + sunRise.minute, ParamHCL_offsetRiseType, ParamHCL_offsetRiseMin);
        uint16_t stopMin = applyOffset(sunSet.hour * 60 + sunSet.minute, ParamHCL_offsetSetType, ParamHCL_offsetSetMin);

        uint16_t elapsedMin = 0;
        uint16_t totalMin = 0;
        if (inTimeWindow(currentMin, startMin, stopMin, elapsedMin, totalMin))
        {
            response_k = getCircadianValue(elapsedMin, totalMin, minT, maxT, 0.70f, 0.70f);
            response_b = static_cast<uint8_t>(getCircadianValue(elapsedMin, totalMin, minB, maxB, 0.90f, 1.45f));
            logDebugP("Sonnenprofil: elapsed=%i total=%i -> %i K / %i %%", elapsedMin, totalMin, response_k, response_b);
        }
        else
        {
            logDebugP("Ausserhalb Sonnenfenster (%i:%i -> %i:%i), nutze Minimum", startMin / 60, startMin % 60, stopMin / 60, stopMin % 60);
        }
    }
    else if (_type == PT_hclType_time)
    {
        uint16_t startMin = normalizeMinute(ParamHCL_startTimeHour * 60 + ParamHCL_startTimeMinute);
        uint16_t stopMin = normalizeMinute(ParamHCL_endTimeHour * 60 + ParamHCL_endTimeMinute);

        uint16_t elapsedMin = 0;
        uint16_t totalMin = 0;
        if (inTimeWindow(currentMin, startMin, stopMin, elapsedMin, totalMin))
        {
            response_k = getCircadianValue(elapsedMin, totalMin, minT, maxT, 0.70f, 0.70f);
            response_b = static_cast<uint8_t>(getCircadianValue(elapsedMin, totalMin, minB, maxB, 0.90f, 1.45f));
            logDebugP("Zeitprofil: elapsed=%i total=%i -> %i K / %i %%", elapsedMin, totalMin, response_k, response_b);
        }
        else
        {
            logDebugP("Ausserhalb Zeitspanne %i:%i - %i:%i", ParamHCL_startTimeHour, ParamHCL_startTimeMinute, ParamHCL_endTimeHour, ParamHCL_endTimeMinute);
        }
    }

    setStatus(response_k, response_b);
    out_k = response_k;
    out_b = response_b;
}

// Modern HCL profile:
// - Color temperature ramps quickly after start and stays cooler around noon.
// - Brightness fades earlier in the evening for residential comfort.
uint16_t HclChannel::getCircadianValue(uint16_t elapsedMin, uint16_t totalMin, uint16_t minVal, uint16_t maxVal, float riseExp, float setExp)
{
    if (totalMin == 0 || maxVal <= minVal)
        return minVal;

    double phase = std::clamp(static_cast<double>(elapsedMin) / static_cast<double>(totalMin), 0.0, 1.0);

    double profile = 0.0;
    if (phase <= 0.5)
    {
        double t = phase * 2.0;
        profile = std::pow(t, std::max(0.05f, riseExp));
    }
    else
    {
        double t = (1.0 - phase) * 2.0;
        profile = std::pow(t, std::max(0.05f, setExp));
    }

    double value = minVal + (maxVal - minVal) * std::clamp(profile, 0.0, 1.0);
    long rounded = std::lround(value);
    if (rounded < minVal)
        rounded = minVal;
    if (rounded > maxVal)
        rounded = maxVal;
    return static_cast<uint16_t>(rounded);
}

uint16_t HclChannel::normalizeMinute(int32_t minuteOfDay)
{
    int32_t mod = minuteOfDay % MINUTES_PER_DAY;
    if (mod < 0)
        mod += MINUTES_PER_DAY;
    return static_cast<uint16_t>(mod);
}

bool HclChannel::inTimeWindow(uint16_t currentMin, uint16_t startMin, uint16_t endMin, uint16_t &elapsedMin, uint16_t &totalMin)
{
    if (startMin == endMin)
    {
        elapsedMin = 0;
        totalMin = 0;
        return false;
    }

    if (startMin < endMin)
    {
        totalMin = endMin - startMin;
        if (currentMin < startMin || currentMin > endMin)
            return false;
        elapsedMin = currentMin - startMin;
        return true;
    }

    totalMin = (MINUTES_PER_DAY - startMin) + endMin;
    if (currentMin >= startMin)
        elapsedMin = currentMin - startMin;
    else
        elapsedMin = (MINUTES_PER_DAY - startMin) + currentMin;
    return true;
}

uint16_t HclChannel::applyOffset(uint16_t baseMinute, uint8_t offsetType, uint8_t offsetMin)
{
    int32_t shifted = baseMinute;
    if (offsetType == PT_hclOffset_plus)
        shifted += offsetMin;
    else if (offsetType == PT_hclOffset_minus)
        shifted -= offsetMin;
    return normalizeMinute(shifted);
}

void HclChannel::setStatus(uint16_t colorTemp, uint8_t brightness)
{
    KoHCL_StatusColorTemp.value(colorTemp, Dpt(7, 600));
    KoHCL_StatusBrightness.value(brightness, DPT_Scaling);
}