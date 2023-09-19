#ifndef DIMCHANNEL_EK_H
#define DIMCHANNEL_EK_H

#include <OpenKNX.h>

#include <LEDModule.h>
#include <HwChannel.h>

class LEDModule;
class DimChannel_EK : public OpenKNX::Channel
{
public:
    DimChannel_EK(uint8_t index);
    ~DimChannel_EK();
    const std::string name() override;

    //dimmchannel
    void setup(int8_t hwchannel, uint16_t startKO);
    void processInputKoEK(GroupObject &ko);
    void task();

private:
    uint8_t m_curve;
    uint8_t m_hwchannel;
    uint16_t m_startko;
    uint8_t m_onbrightness;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;
    
    uint16_t calc_ko_switch;
    uint16_t calc_ko_dimabsolute;
    uint16_t calc_ko_dimrelativ;
    uint16_t calc_ko_statusonoff;
    uint16_t calc_ko_statusbrightness;

    uint8_t _lastbrightness = 0;
    uint8_t _setbrightness = 0;

    uint32_t _currentTaskRun = 0;
    uint32_t _lastTaskRun = 0;

    HWChannel *hwchannels[CHANNELSHW]; 
};

#endif