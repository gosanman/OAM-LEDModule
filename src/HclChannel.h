#ifndef HCLMODULE_H
#define HCLMODULE_H

#include <OpenKNX.h>
#include <cstdint>

class HclChannel
{
public:
    void setup(uint8_t index);
    void loop(uint16_t &out_k, uint8_t &out_b);

private:
    std::string logPrefix();
    uint8_t channelIndex();
    uint8_t _index = 0;
    bool _isConfigured = false;
    uint8_t _type = 0;

    uint16_t getCircadianValue(uint16_t elapsedMin, uint16_t totalMin, uint16_t minVal, uint16_t maxVal, float riseExp, float setExp);
    uint16_t normalizeMinute(int32_t minuteOfDay);
    bool inTimeWindow(uint16_t currentMin, uint16_t startMin, uint16_t endMin, uint16_t &elapsedMin, uint16_t &totalMin);
    uint16_t applyOffset(uint16_t baseMinute, uint8_t offsetType, uint8_t offsetMin);
    void setStatus(uint16_t colorTemp, uint8_t brightness);
};

#endif
