#pragma once

#include <Arduino.h>
#include <knx.h>

class LEDControl;
class DimChannel_EK
{
public:
    DimChannel_EK(uint8_t iId);
    ~DimChannel_EK();

    void setup(int8_t hwchannel, uint16_t startKO);
    void processInputKo(GroupObject &iKo);

private:
    uint8_t mId = 0;

    uint8_t m_curve;
    uint8_t m_hwchannel;
    uint8_t m_onbrightness;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;

    uint16_t calc_ko_switch;
    uint16_t calc_ko_dimabsolute;
    uint16_t calc_ko_dimrelativ;
    uint16_t calc_ko_statusonoff;
    uint16_t calc_ko_statusbrightness;

    uint32_t calcParamIndex(uint16_t iParamIndex);
};