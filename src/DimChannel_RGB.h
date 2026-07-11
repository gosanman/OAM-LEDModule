#ifndef DIMCHANNEL_RGB_H
#define DIMCHANNEL_RGB_H

#include "DimChannel.h"
#include "LEDModule.h"

// dim actions
enum DimTaskRGB {
    RGB_DIM_IDLE,
    RGB_DIM_STOP,
    RGB_DIM_SOFT_ON,
    RGB_DIM_SOFT_OFF,
    RGB_DIM_RGB_SET,
    RGB_DIM_RGB_REL,
    RGB_DIM_H_UP,
    RGB_DIM_H_DOWN,
    RGB_DIM_S_UP,
    RGB_DIM_S_DOWN,
    RGB_DIM_V_UP,
    RGB_DIM_V_DOWN
};

// scene actions
#define SC_RGB_None             0
#define SC_RGB_OnValueDayNight  1
#define SC_RGB_SetColor         2
#define SC_RGB_Off              9

class LEDModule;
class DimChannel_RGB : public DimChannel
{
public:
    DimChannel_RGB(uint8_t index);
    ~DimChannel_RGB();
    const std::string name() override;

    // dimmchannel
    void setup(uint8_t *hwchannel) override;
    void processInputKo(GroupObject &ko) override;
    void task() override;

    void setDayNight(bool isNight) override;
    std::vector<uint8_t> getHWPorts() override;
    uint8_t getChannelIndex() override;
    uint8_t getChannelType() override;
    void setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness) override;
    void resend() override;

private:
    uint8_t m_hwchannel_r;
    uint8_t m_hwchannel_g;
    uint8_t m_hwchannel_b;

    bool m_usedayvalue;
    uint8_t *m_dayvalue;
    bool m_usenightvalue;
    uint8_t *m_nightvalue;
    uint32_t m_durationrelativ; // ms aus getTimeWithPattern (darf > 65535 sein)
    uint32_t m_durationabsolut;
    uint8_t m_curve;
    float m_gammacorrection;
    uint8_t m_gammaT[256]; // eigene Gamma-Tabelle je Kanal (globale gammaT wird sonst von allen geteilt)

    uint8_t _index;

    uint8_t _newValueRGB[3];                      // 0 = Red, 1 = Green, 2 = Blue
    uint8_t _currentValueRGB[3];                  // 0 = Red, 1 = Green, 2 = Blue
    uint16_t _currentValueHSV[3];                 // 0 = h, 1 = s, 2 = v
    uint8_t _lastDayValue[3] = {125, 125, 125};   // 0 = Red, 1 = Green, 2 = Blue
    uint8_t _lastNightValue[3] = {125, 125, 125}; // 0 = Red, 1 = Green, 2 = Blue
    uint8_t _valueOff[3] = {0, 0, 0};             // 0 = Red, 1 = Green, 2 = Blue
    uint16_t _currentHclValue[2] = {0, 0};        // 0 = Brightness, 1 = Kelvin

    bool isNight = false;

    void koHandleSwitch(GroupObject &ko);
    void koHandleDimmAbsColorRGB(GroupObject &ko);
    void koHandleDimmAbsColorHSV(GroupObject &ko);
    void koHandleDimmAbsRGB(GroupObject &ko, uint8_t index);
    void koHandleDimmAbsHSV(GroupObject &ko, uint8_t index);
    void koHandleDimmRelH(GroupObject &ko);
    void koHandleDimmRelS(GroupObject &ko);
    void koHandleDimmRelV(GroupObject &ko);
    void koHandleDimmRelRGB(GroupObject &ko, uint8_t index);
    void koHandleScene(GroupObject &ko);

    void switchOnHelper();
    void switchOffHelper();

    uint16_t calcKoNumber(int koNum);
    void sendKoStateOnChange(uint16_t koNr, const KNXValue &value, const Dpt &type, bool alwayssend);
    void setNewValueRGB(uint8_t *value);
    void sendDimValue();
    void updateDimValue();

    // dimmer task
    void startTask(uint8_t task); // setzt neue Aufgabe und erzwingt Neuberechnung von Increments/_time
    void dimmerTask();
    void handleDimGeneric(uint8_t *currentValues, uint8_t *targetValues, uint8_t minValue, uint8_t maxValue, bool isAbsolute);
    bool _busy = false;
    uint8_t _valueMin = 0;
    uint8_t _valueMax = 255;
    uint8_t _currentTask = DimTaskRGB::RGB_DIM_IDLE;
    uint32_t _currentMillis = 0;
    uint32_t _lastTaskExecution = 0;
    uint32_t _time = 0;
    float _dimIncrement[3] = {0};
    float _dimAcc[3] = {0};
    bool _dimmingInit = false;
    bool _isOn = false;         // true = on, false = off

    void handleDimStop();
    void handleDimSoftOn();
    void handleDimSoftOff();
    void handleDimSetRGB();
    void handleDimRelRGB();
};

#endif