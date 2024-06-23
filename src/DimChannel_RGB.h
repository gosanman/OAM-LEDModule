#ifndef DIMCHANNEL_RGB_H
#define DIMCHANNEL_RGB_H

//#include <OpenKNX.h>
#include "DimChannel.h"

#include "LEDModule.h"
#include "HwChannel.h"

#define TIMEBASE_SECONDS        0
#define TIMEBASE_MINUTES        1
#define TIMEBASE_HOURS          2
#define TIMEBASE_TENTH_SECONDS  3

class LEDModule;
class DimChannel_RGB : public DimChannel
{
public:
    DimChannel_RGB(uint8_t index);
    ~DimChannel_RGB();
    const std::string name() override;

    //dimmchannel
    void setup(uint8_t* hwchannel) override;
    void processInputKo(GroupObject &ko) override;
    void task() override;

    void setDayNight(bool isNight);

private:
    uint8_t m_hwchannel_r;
    uint8_t m_hwchannel_g;
    uint8_t m_hwchannel_b;
    
    bool m_usedayvalue;
    uint8_t *m_dayvalue;
    bool m_usenightvalue;
    uint8_t *m_nightvalue;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;
    uint8_t m_curve;

    uint8_t _index;

    uint8_t _currentValueRGB[3];    // 0 = Red, 1 = Green, 2 = Blue
    uint8_t _currentValueHSV[3];    // 0 = h, 1 = s, 2 = v
    uint8_t _lastDayValue[3] = {125, 125, 125};       // 0 = Red, 1 = Green, 2 = Blue
    uint8_t _lastNightValue[3] = {125, 125, 125};     // 0 = Red, 1 = Green, 2 = Blue

    uint32_t _currentUpdateRun = 0;
    uint32_t _lastUpdatekRun = 0;

    bool isNight = false;

    static void hsvToRGB(uint8_t in_h, uint8_t in_s, uint8_t in_v, uint8_t& out_r, uint8_t& out_g, uint8_t& out_b);
    static void rgbToHSV(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint8_t& out_h, uint8_t& out_s, uint8_t& out_v);
    static double threeway_max(double a, double b, double c);
    static double threeway_min(double a, double b, double c); 

    void koHandleSwitch(GroupObject &ko);
    void koHandleDimmAbsColorRGB(GroupObject &ko);
    void koHandleDimmAbsColorHSV(GroupObject &ko);
    void koHandleDimmAbsRGB(GroupObject &ko, uint8_t index);
    void koHandleDimmAbsHSV(GroupObject &ko, uint8_t index);
     
    void koHandleDimmRelH(GroupObject &ko);
    void koHandleDimmRelS(GroupObject &ko);
    void koHandleDimmRelV(GroupObject &ko);
    void koHandleDimmRelR(GroupObject &ko);
    void koHandleDimmRelG(GroupObject &ko);
    void koHandleDimmRelB(GroupObject &ko);
    void koHandleScene(GroupObject &ko);

    uint16_t calcKoNumber(int koNum);
    void sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type);
    void sendDimValue(); 
    void updateDimValue();

    uint32_t getTimeWithPattern(uint16_t time, uint8_t base);

    HWChannel *hwchannels[MAXCHANNELSHW];
       
};

#endif