#ifndef DIMCHANNEL_TW_H
#define DIMCHANNEL_TW_H

#include <OpenKNX.h>

#include "LEDModule.h"
#include "HwChannel.h"

class LEDModule;
class DimChannel_TW : public OpenKNX::Channel
{
public:
    DimChannel_TW(uint8_t index);
    ~DimChannel_TW();
    const std::string name() override;

    //dimmchannel
    void setup(int8_t hwchannel_ww, int8_t hwchannel_cw, uint16_t startKO);
    void processInputKoTW(GroupObject &ko);
    void task();

private:
    uint8_t m_curve;
    uint8_t m_hwchannel_ww;
    uint8_t m_hwchannel_cw;
    uint16_t m_colortempww;
    uint16_t m_colortempcw;
    uint8_t m_onbrightness;
    uint16_t m_oncolortemp;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;

    uint16_t calc_ko_switch;
    uint16_t calc_ko_dimabsolute;
    uint16_t calc_ko_dimkelvin;
    uint16_t calc_ko_dimrelativ;
    uint16_t calc_ko_statusonoff;
    uint16_t calc_ko_statusbrightness;
    uint16_t calc_ko_statuskelvin;

    uint8_t _lastbrightness = 0;
    uint16_t _lastcolortemp = 0;
    uint8_t _lastvalue_ww = 0;
    uint8_t _lastvalue_cw = 0;

    uint32_t _currentTaskRun = 0;
    uint32_t _lastTaskRun = 0;

    uint16_t prozToDim(uint8_t value, uint8_t curve);

    HWChannel *hwchannels[CHANNELSHW];
};

#endif