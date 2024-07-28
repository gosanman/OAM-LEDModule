#ifndef DIMCHANNEL_EK_H
#define DIMCHANNEL_EK_H

//#include <OpenKNX.h>
#include "DimChannel.h"

#include "LEDModule.h"
#include "HwChannel.h"

#define TIMEBASE_SECONDS        0
#define TIMEBASE_MINUTES        1
#define TIMEBASE_HOURS          2
#define TIMEBASE_TENTH_SECONDS  3

// scene actions
#define SC_EK_None              0
#define SC_EK_OnValueDayNight   1
#define SC_EK_SetBrightness     2
#define SC_EK_Off               9

//class LEDModule;
class DimChannel_EK : public DimChannel
{
public:
    DimChannel_EK(uint8_t index);
    ~DimChannel_EK();
    const std::string name() override;

    //dimmchannel
    void setup(uint8_t* hwchannel) override;
    void processInputKo(GroupObject &ko) override;
    void task() override;

    void setDayNight(bool value) override;

private:
    uint8_t m_hwchannel;

    bool m_usedayvalue;
    uint8_t m_dayvalue = 255;
    bool m_usenightvalue;
    uint8_t m_nightvalue = 25;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;
    uint8_t m_curve;

    uint8_t _index;

    uint8_t _currentValueEK = 0;
	uint8_t _lastDayValue = 255;
	uint8_t _lastNightValue = 25;

    uint32_t _currentUpdateRun = 0;
    uint32_t _lastUpdatekRun = 0;

    bool isNight = false;

    void koHandleSwitch(GroupObject &ko);
    void koHandleDimmAbs(GroupObject &ko);
    void koHandleDimmRel(GroupObject &ko);
    void koHandleScene(GroupObject &ko);

    uint16_t calcKoNumber(int koNum);
    void sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type);
    //void getDimValue();
    void sendDimValue(); 
    void updateDimValue();

    uint32_t getTimeWithPattern(uint16_t time, uint8_t base);

    HWChannel *hwchannels[MAXCHANNELSHW]; 
};

#endif