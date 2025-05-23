#ifndef HCLMODULE_H
#define HCLMODULE_H

#include <OpenKNX.h>

class HclChannel
{
public:
    void setup(uint8_t index);
    void loop();

private:
    const std::string logPrefix();
    const uint8_t channelIndex();
    uint8_t _index = 0;
    bool _isConfigured = false;
    uint8_t _type = 0;

    uint16_t getValueFromSun(uint16_t minCurr, uint16_t minDiff, uint16_t minK, uint16_t maxK);
};

#endif