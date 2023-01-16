#pragma once

#include <Arduino.h>
#include <knx.h>

class LEDControl;
class DimChannel_RGB
{
public:
    DimChannel_RGB(uint8_t iId);
    ~DimChannel_RGB();

    void setup(int8_t hwchannel_r, int8_t hwchannel_g, int8_t hwchannel_b, uint16_t startKO);
    void processInputKo(GroupObject &iKo);

private:
    uint8_t mId = 0;

    uint8_t m_curve;
    uint8_t m_hwchannel_r;
    uint8_t m_hwchannel_g;
    uint8_t m_hwchannel_b;
    byte m_oncolor;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;

    uint16_t calc_ko_switch;
    uint16_t calc_ko_dimabsolutergb;
    uint16_t calc_ko_dimabsolutehsv;
    uint16_t calc_ko_dimrelativrgb;
    uint16_t calc_ko_dimrelativhsv;
    uint16_t calc_ko_statusonoff;
    uint16_t calc_ko_statusrgb;
    uint16_t calc_ko_statushsv;

    uint32_t calcParamIndex(uint16_t iParamIndex);
};