#ifndef DIMCHANNEL_TW_H
#define DIMCHANNEL_TW_H

//#include <OpenKNX.h>
#include "DimChannel.h"

#include "LEDModule.h"
#include "HwChannel.h"

#define TIMEBASE_SECONDS        0
#define TIMEBASE_MINUTES        1
#define TIMEBASE_HOURS          2
#define TIMEBASE_TENTH_SECONDS  3

// dim actions
#define DIM_IDLE    0
#define DIM_STOP    1
#define DIM_B_UP    4
#define DIM_B_DOWN  5
#define DIM_K_UP    6
#define DIM_K_DOWN  7

// scene actions
#define SC_TW_None              0
#define SC_TW_OnValueDayNight   1
#define SC_TW_SetBrightness     2
#define SC_TW_SetColorTemp      3
#define SC_TW_SetBoth           4
#define SC_EK_Off               9

class LEDModule;
class DimChannel_TW : public DimChannel
{
public:
    DimChannel_TW(uint8_t index);
    ~DimChannel_TW();
    const std::string name() override;

    //dimmchannel
    void setup(uint8_t* hwchannel) override;
    void processInputKo(GroupObject &ko) override;
    void task() override;

    void setDayNight(bool isNight);

private:
    uint8_t m_hwchannel_ww;
    uint8_t m_hwchannel_cw;

    uint16_t m_colortempww;
    uint16_t m_colortempcw;
    bool m_usedayvalue;
    uint8_t m_daybrightness;
    uint16_t m_daycolortemp;
    bool m_usenightvalue;
    uint8_t m_nightbrightness;
    uint16_t m_nightcolortemp;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;
    uint8_t m_curve;

    uint8_t _index;

    uint16_t _currentValueTW[2] = {255, 4000};    // 0 = Brightness, 1 = Kelvin
    uint16_t _lastDayValue[2] = {255, 4000};      // 0 = Brightness, 1 = Kelvin
    uint16_t _lastNightValue[2] = {100, 4000};    // 0 = Brightness, 1 = Kelvin

    uint32_t _currentUpdateRun = 0;
    uint32_t _lastUpdatekRun = 0;

    uint8_t percent = 0;
    uint8_t percentWW = 0;
    uint8_t percentCW = 0;

    bool isNight = false;

    void koHandleSwitch(GroupObject &ko);
    void koHandleDimmAbsBrightness(GroupObject &ko);
    void koHandleDimmAbsColorTemp(GroupObject &ko);
    void koHandleDimmRelBrightness(GroupObject &ko);
    void koHandleDimmRelColorTemp(GroupObject &ko);
    void koHandleScene(GroupObject &ko);

    uint16_t calcKoNumber(int koNum);
    void sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool onchange);
    void sendDimValue();
    void setDimValue(); 
    void updateDimValue();

    uint16_t prozToDim(uint8_t value, uint8_t curve);
    uint32_t getTimeWithPattern(uint16_t time, uint8_t base);

    void dimmerTask();
    bool _busy = false;
    uint8_t _valueMinBrightness = 0;
    uint8_t _valueMaxBrightness = 255;
    uint8_t _currentTask = DIM_IDLE;
    uint32_t _currentMillis = 0;
    uint32_t _lastTaskExecution;
    uint32_t _delayRelative;

    HWChannel *hwchannels[MAXCHANNELSHW];
};

#endif