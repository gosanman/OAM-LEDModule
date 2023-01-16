#include <LEDControl.h>
#include <DimChannel_EK.h>

DimChannel_EK::DimChannel_EK(uint8_t iId)
{
  mId = iId;
}

DimChannel_EK::~DimChannel_EK() {}

void DimChannel_EK::setup(int8_t hwchannel, uint16_t startKO)
{
    // Parameter
    m_hwchannel = hwchannel;
    m_onbrightness = knx.paramByte(calcParamIndex(PT_SCOnBrightness));
    m_durationrelativ = knx.paramByte(calcParamIndex(PT_SCRelativDimTime)) * 1000;
    m_durationabsolut = knx.paramByte(calcParamIndex(PT_SCOnOffTime)) * 100;
    m_curve = knx.paramByte(calcParamIndex(PT_SCDimCurve));     // 0=A, 1=B, 2=C, 3=D
     
    // calculate KO Objects and save this
    calc_ko_switch = startKO + KO_OFFSET_EK_SWITCH;
    calc_ko_dimabsolute = startKO + KO_OFFSET_EK_DIMABSOLUT;
    calc_ko_dimrelativ = startKO + KO_OFFSET_EK_DIMRELATIV;
    calc_ko_statusonoff = startKO + KO_OFFSET_EK_STATUSONOFF;
    calc_ko_statusbrightness = startKO + KO_OFFSET_EK_STATUSBRIGHTNESS;

    // send parameter to LEDControl
    LEDControl::sInstance->setChannelDurationRelativ(m_hwchannel, m_durationrelativ);
    LEDControl::sInstance->setChannelDurationAbsolut(m_hwchannel, m_durationabsolut);
    
    #ifdef KDEBUG_LED
        SERIAL_DEBUG.println("------------------ DEBUG -------------------");
        SERIAL_DEBUG.printf("Channel EK: %i KO Switch: %i\n\r", mId, calc_ko_switch);
        SERIAL_DEBUG.printf("Channel EK: %i KO Dim Absolute: %i\n\r", mId, calc_ko_dimabsolute);
        SERIAL_DEBUG.printf("Channel EK: %i KO Dim Relativ: %i\n\r", mId, calc_ko_dimrelativ);
        SERIAL_DEBUG.printf("Channel EK: %i KO Status OnOff: %i\n\r", mId, calc_ko_statusonoff);
        SERIAL_DEBUG.printf("Channel EK: %i KO Status Brightness: %i\n\r", mId, calc_ko_statusbrightness);
        SERIAL_DEBUG.printf("Channel EK: %i HW Port: %i\n\r", mId, m_hwchannel);
        SERIAL_DEBUG.printf("Channel EK: %i PT OnBrightness: %i\n\r", mId, m_onbrightness);
        SERIAL_DEBUG.printf("Channel EK: %i PT DurationRelativ: %i\n\r", mId, LEDControl::sInstance->getChannelDurationRelativ(m_hwchannel));
        SERIAL_DEBUG.printf("Channel EK: %i PT DurationAbsolut: %i\n\r", mId, LEDControl::sInstance->getChannelDurationAbsolut(m_hwchannel));
        SERIAL_DEBUG.printf("Channel EK: %i PT Curve: %i\n\r", mId, m_curve);
        SERIAL_DEBUG.println("--------------------------------------------");
    #endif
}

uint32_t DimChannel_EK::calcParamIndex(uint16_t iParamIndex)
{
  return iParamIndex + mId * PT_SCBlockSize + PT_SCBlockOffset;
}

void DimChannel_EK::processInputKo(GroupObject &iKo)
{
    uint16_t ko = iKo.asap();
    if (ko == calc_ko_switch)
    {
        bool value = iKo.value(DPT_Switch);
        SERIAL_DEBUG.printf("Switch - Value: %i\n\r", value);
        if (value) // on
        {
            LEDControl::sInstance->dimChannel(m_hwchannel, m_onbrightness);
            knx.getGroupObject(calc_ko_statusonoff).value(value, DPT_Switch);
            knx.getGroupObject(calc_ko_statusbrightness).value(m_onbrightness, DPT_Percent_U8);
        }
        else // off
        {
            LEDControl::sInstance->offChannel(m_hwchannel);
            knx.getGroupObject(calc_ko_statusonoff).value(value, DPT_Switch);
            knx.getGroupObject(calc_ko_statusbrightness).value((uint8_t)0, DPT_Percent_U8);
        }
    }
    else if (ko == calc_ko_dimabsolute)
    {
        uint8_t brightness = iKo.value(DPT_Percent_U8);
        SERIAL_DEBUG.printf("Dim Absolute - Value: %i\n\r", brightness);
        LEDControl::sInstance->dimChannel(m_hwchannel, brightness);
        knx.getGroupObject(calc_ko_statusbrightness).value(brightness, DPT_Percent_U8);
        // onoff noch zu aktuallisieren
    }
    else if (ko == calc_ko_dimrelativ)
    {
        uint8_t direction = iKo.value(Dpt(3,7,0));
        uint8_t step = iKo.value(Dpt(3,7,1));
        SERIAL_DEBUG.printf("Dim Relativ - Direction: %i, Step: %i\n\r", direction, step);
        //direction true = dim up, false = dim down, step = 0 then stop
        if (step == 0)
        {
            SERIAL_DEBUG.printf("Dim Relativ - Stop\n\r");
            LEDControl::sInstance->dimStop(m_hwchannel);
        }
        else if (direction == 1)
        {
            SERIAL_DEBUG.printf("Dim Relativ - DimUp\n\r");
            LEDControl::sInstance->dimUp(m_hwchannel);
        }
        else if (direction == 0)
        {
            SERIAL_DEBUG.printf("Dim Relativ - DimDown\n\r");
            LEDControl::sInstance->dimDown(m_hwchannel);
        }
    }
}