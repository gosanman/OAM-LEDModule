#ifndef DIMCHANNEL_RGB_H
#define DIMCHANNEL_RGB_H

//#include <OpenKNX.h>
#include "DimChannel.h"

#include "LEDModule.h"
#include "HwChannel.h"

class LEDModule;
class DimChannel_RGB : public DimChannel
{
public:
    DimChannel_RGB(uint8_t index);
    ~DimChannel_RGB();
    const std::string name() override;

    //dimmchannel
    void setup(uint8_t* hwchannel, uint16_t startKO) override;
    void processInputKo(GroupObject &ko) override;
    void task() override;

    void setDayNight(bool isNight);

private:
    uint8_t m_curve;
    uint8_t m_hwchannel_r;
    uint8_t m_hwchannel_g;
    uint8_t m_hwchannel_b;
    uint8_t *m_oncolor;
    uint8_t *m_nightcolor;
    uint32_t m_oncolorvalue;
    bool m_useoncolor;
    bool m_usenightcolor;
    uint16_t m_durationrelativ;
    uint16_t m_durationabsolut;

    uint16_t calc_ko_switch;
    uint16_t calc_ko_colorrgb;
    uint16_t calc_ko_colorhsv;
    uint16_t calc_ko_dimabsolutshadeh;
    uint16_t calc_ko_dimabsolutsaturations;
    uint16_t calc_ko_dimabsolutbrightnessv;
    uint16_t calc_ko_dimrelativshadeh;
    uint16_t calc_ko_dimrelativsaturations;
    uint16_t calc_ko_dimrelativbrightnessv;

    uint16_t calc_ko_statusonoff;
    uint16_t calc_ko_statuscolorrgb;
    uint16_t calc_ko_statuscolorhsv;
    uint16_t calc_ko_statusshadeh;
    uint16_t calc_ko_statussaturations;
    uint16_t calc_ko_statusbrightnessv;

    uint8_t _index;

    uint8_t _lasthwvalue[3];
    uint32_t _currentvalue_rgb;
    uint32_t _currentvalue_hsv;
    uint8_t _currentrgb[3];
    uint8_t _currenthsv[3];
    uint8_t _currenth, _currents, _currentv;

    static void hsvToRGB(uint8_t in_h, uint8_t in_s, uint8_t in_v, uint8_t& out_r, uint8_t& out_g, uint8_t& out_b);
    static void rgbToHSV(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint8_t& out_h, uint8_t& out_s, uint8_t& out_v);
    static double threeway_max(double a, double b, double c);
    static double threeway_min(double a, double b, double c); 

    uint32_t _currentTaskRun = 0;
    uint32_t _lastTaskRun = 0;

    bool isNight = false;

    void updateDimValue();

    HWChannel *hwchannels[MAXCHANNELSHW];
       
};

#endif