#include <LEDControl.h>
#include <DimChannel_EK.h>

DimChannel_RGB::DimChannel_RGB(uint8_t iId)
{
  mId = iId;
}

DimChannel_RGB::~DimChannel_RGB() {}

void DimChannel_RGB::setup(int8_t hwchannel_r, int8_t hwchannel_g, int8_t hwchannel_b, uint16_t startKO)
{
    // Parameter
    m_hwchannel_r = hwchannel_r;
    m_hwchannel_g = hwchannel_g;
    m_hwchannel_b = hwchannel_b;
    m_oncolor = knx.paramByte(calcParamIndex(PT_RGBOnColor));
    m_durationrelativ = knx.paramByte(calcParamIndex(PT_RGBRelativDimTime)) * 1000;
    m_durationabsolut = knx.paramByte(calcParamIndex(PT_RGBOnOffTime)) * 100;
    m_curve = knx.paramByte(calcParamIndex(PT_RGBDimCurve));     // 0=A, 1=B, 2=C, 3=D
     
    // calculate KO Objects and save this
    calc_ko_switch = KO_ChannelRGBSwitch;
    calc_ko_dimabsolutergb = KO_ChannelRGBDimAbsoluteRGB;
    calc_ko_dimabsolutehsv = KO_ChannelRGBDimAbsoluteHSV;
    calc_ko_dimrelativrgb = KO_ChannelRGBDimRelativRGB;
    calc_ko_dimrelativhsv = KO_ChannelRGBDimRelativHSV;
    calc_ko_statusonoff = KO_ChannelRGBStatusOnOff;
    calc_ko_statusrgb = KO_ChannelRGBStatusRGB;
    calc_ko_statushsv = KO_ChannelRGBStatusHSV;

    // send parameter to LEDControl
    LEDControl::sInstance->setChannelDurationRelativ(m_hwchannel_r, m_durationrelativ);
    LEDControl::sInstance->setChannelDurationRelativ(m_hwchannel_g, m_durationrelativ);
    LEDControl::sInstance->setChannelDurationRelativ(m_hwchannel_b, m_durationrelativ);
    LEDControl::sInstance->setChannelDurationAbsolut(m_hwchannel_r, m_durationabsolut);
    LEDControl::sInstance->setChannelDurationAbsolut(m_hwchannel_g, m_durationabsolut);
    LEDControl::sInstance->setChannelDurationAbsolut(m_hwchannel_b, m_durationabsolut);
    
    #ifdef KDEBUG_LED
        SERIAL_DEBUG.println("------------------ DEBUG -------------------");
        SERIAL_DEBUG.printf("Channel RGB: %i KO Switch: %i\n\r", mId, calc_ko_switch);
        SERIAL_DEBUG.printf("Channel RGB: %i KO Dim Absolute RGB: %i\n\r", mId, calc_ko_dimabsolutergb);
        SERIAL_DEBUG.printf("Channel RGB: %i KO Dim Absolute HSV: %i\n\r", mId, calc_ko_dimabsolutehsv);
        SERIAL_DEBUG.printf("Channel RGB: %i KO Dim Relativ RGB: %i\n\r", mId, calc_ko_dimrelativrgb);
        SERIAL_DEBUG.printf("Channel RGB: %i KO Dim Relativ HSV: %i\n\r", mId, calc_ko_dimrelativhsv);
        SERIAL_DEBUG.printf("Channel RGB: %i KO Status OnOff: %i\n\r", mId, calc_ko_statusonoff);
        SERIAL_DEBUG.printf("Channel RGB: %i KO Status RGB: %i\n\r", mId, calc_ko_statusrgb);
        SERIAL_DEBUG.printf("Channel RGB: %i KO Status HSV: %i\n\r", mId, calc_ko_statushsv);
        SERIAL_DEBUG.printf("Channel RGB: %i HW Port R: %i\n\r", mId, m_hwchannel_r);
        SERIAL_DEBUG.printf("Channel RGB: %i HW Port G: %i\n\r", mId, m_hwchannel_g);
        SERIAL_DEBUG.printf("Channel RGB: %i HW Port B: %i\n\r", mId, m_hwchannel_b);
        SERIAL_DEBUG.printf("Channel RGB: %i PT DurationRelativ R: %i\n\r", mId, LEDControl::sInstance->getChannelDurationRelativ(m_hwchannel_r));
        SERIAL_DEBUG.printf("Channel RGB: %i PT DurationAbsolut R: %i\n\r", mId, LEDControl::sInstance->getChannelDurationAbsolut(m_hwchannel_r));
        SERIAL_DEBUG.printf("Channel RGB: %i PT DurationRelativ G: %i\n\r", mId, LEDControl::sInstance->getChannelDurationRelativ(m_hwchannel_g));
        SERIAL_DEBUG.printf("Channel RGB: %i PT DurationAbsolut G: %i\n\r", mId, LEDControl::sInstance->getChannelDurationAbsolut(m_hwchannel_g));
        SERIAL_DEBUG.printf("Channel RGB: %i PT DurationRelativ B: %i\n\r", mId, LEDControl::sInstance->getChannelDurationRelativ(m_hwchannel_b));
        SERIAL_DEBUG.printf("Channel RGB: %i PT DurationAbsolut B: %i\n\r", mId, LEDControl::sInstance->getChannelDurationAbsolut(m_hwchannel_b));
        SERIAL_DEBUG.printf("Channel RGB: %i Curve: %i\n\r", mId, m_curve);
        SERIAL_DEBUG.println("--------------------------------------------");
    #endif
}

uint32_t DimChannel_RGB::calcParamIndex(uint16_t iParamIndex)
{
  return iParamIndex + mId * PT_SCBlockSize + PT_SCBlockOffset;
}

void DimChannel_RGB::processInputKo(GroupObject &iKo)
{
    uint16_t ko = iKo.asap();
    if (ko == calc_ko_switch)
    {
        bool value = iKo.value(DPT_Switch);
        SERIAL_DEBUG.printf("Switch - Value: %i\n\r", value);
        if (value) // on
        {
            LEDControl::sInstance->onChannel(m_hwchannel_r);
            LEDControl::sInstance->onChannel(m_hwchannel_g);
            LEDControl::sInstance->onChannel(m_hwchannel_b);           
        }
        else // off
        {
            LEDControl::sInstance->offChannel(m_hwchannel_r);
            LEDControl::sInstance->offChannel(m_hwchannel_g);
            LEDControl::sInstance->offChannel(m_hwchannel_b);
        }
    }
    else if (ko == calc_ko_dimabsolutergb)
    {
        uint8_t brightness = iKo.value(DPT_Percent_U8);
        SERIAL_DEBUG.printf("Dim Absolute - Value: %i\n\r", brightness);


    }
    else if (ko == calc_ko_dimabsolutehsv)
    {
        uint8_t brightness = iKo.value(DPT_Percent_U8);
        SERIAL_DEBUG.printf("Dim Absolute - Value: %i\n\r", brightness);


    }
    else if (ko == calc_ko_dimrelativrgb)
    {
        uint8_t direction = iKo.value(Dpt(3,7,0));
        uint8_t step = iKo.value(Dpt(3,7,1));
        SERIAL_DEBUG.printf("Dim Relativ - Direction: %i, Step: %i\n\r", direction, step);
        //direction true = dim up, false = dim down, step = 0 then stop
        if (step == 0)
        {
            SERIAL_DEBUG.printf("Dim Relativ - Stop\n\r");

        }
        else if (direction == 1)
        {
            SERIAL_DEBUG.printf("Dim Relativ - DimUp\n\r");

        }
        else if (direction == 0)
        {
            SERIAL_DEBUG.printf("Dim Relativ - DimDown\n\r");

        }
    }
    else if (ko == calc_ko_dimabsolutergb)
    {
        uint8_t direction = iKo.value(Dpt(3,7,0));
        uint8_t step = iKo.value(Dpt(3,7,1));
        SERIAL_DEBUG.printf("Dim Relativ - Direction: %i, Step: %i\n\r", direction, step);
        //direction true = dim up, false = dim down, step = 0 then stop
        if (step == 0)
        {
            SERIAL_DEBUG.printf("Dim Relativ - Stop\n\r");

        }
        else if (direction == 1)
        {
            SERIAL_DEBUG.printf("Dim Relativ - DimUp\n\r");

        }
        else if (direction == 0)
        {
            SERIAL_DEBUG.printf("Dim Relativ - DimDown\n\r");

        }
    }
}