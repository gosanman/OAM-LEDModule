#ifndef DIMCHANNEL_EK_H
#define DIMCHANNEL_EK_H

#include "DimChannel.h"
#include "LEDModule.h"

// dim actions
enum DimTaskEK {
    EK_DIM_IDLE,
    EK_DIM_STOP,
    EK_DIM_SOFT_ON,
    EK_DIM_SOFT_OFF,
    EK_DIM_B_SET,
    EK_DIM_B_UP,
    EK_DIM_B_DOWN
}; // EK ist helligkeitsbasiert - keine Kelvin-Tasks (die gab es nur als toter Copy-Paste aus TW)

// scene actions
#define SC_EK_None              0
#define SC_EK_OnValueDayNight   1
#define SC_EK_SetBrightness     2
#define SC_EK_Off               9

// class LEDModule;
class DimChannel_EK : public DimChannel
{
public:
    DimChannel_EK(uint8_t index);
    ~DimChannel_EK();
    const std::string name() override;

    // dimmchannel
    void setup(uint8_t *hwchannel) override;
    void processInputKo(GroupObject &ko) override;
    void task() override;

    void setDayNight(bool value) override;
    uint8_t getHWPorts(uint8_t *ports) override;
    uint8_t getChannelIndex() override;
    uint8_t getChannelType() override;
    void setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness) override;
    void resend() override;

private:
    uint8_t m_hwchannel;

    bool m_usedayvalue;
    uint8_t m_dayvalue = 255;
    bool m_usenightvalue;
    uint8_t m_nightvalue = 25;
    uint32_t m_durationrelativ; // ms aus getTimeWithPattern (darf > 65535 sein)
    uint32_t m_durationabsolut;
    uint8_t m_curve;

    uint8_t _index;

    uint8_t _newValueEK = 255;
    uint8_t _currentValueEK = 0;
    uint8_t _lastDayValue = 255;
    uint8_t _lastNightValue = 100;
    uint16_t _currentHclValue[2] = {0, 0};     // 0 = Brightness, 1 = Kelvin

    bool isNight = false;

    void koHandleSwitch(GroupObject &ko);
    void koHandleDimmAbs(GroupObject &ko);
    void koHandleDimmRel(GroupObject &ko);
    void koHandleScene(GroupObject &ko);

    void switchOnHelper();
    void switchOffHelper();

    uint16_t calcKoNumber(int koNum);
    void sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool alwayssend);
    void sendDimValue();
    void updateDimValue();

   // dimmer task
    void startTask(uint8_t task); // setzt neue Aufgabe und erzwingt _time-Neuberechnung
    void dimmerTask();
    void handleDimGeneric(uint8_t& currentValue, uint8_t targetValue, uint8_t minValue, uint8_t maxValue, bool isAbsolute);
    bool _busy = false;
    uint8_t _valueMinBrightness = 0;
    uint8_t _valueMaxBrightness = 255;
    uint8_t _currentTask = DimTaskEK::EK_DIM_IDLE;
    uint32_t _currentMillis = 0;
    uint32_t _lastTaskExecution = 0;
    uint32_t _time = 0;
    bool _isOn = false;         // true = on, false = off

    void handleDimStop();
    void handleDimSoftOn();
    void handleDimSoftOff();
    void handleDimSetBrightness();
    void handleDimBrightnessUp();
    void handleDimBrightnessDown();
};

#endif