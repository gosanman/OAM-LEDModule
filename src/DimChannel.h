#ifndef DIMCHANNEL_H
#define DIMCHANNEL_H

#include <OpenKNX.h>

#include "LEDModule.h"

class LEDModule;
class DimChannel : public OpenKNX::Channel
{
public:
    DimChannel(uint8_t index);
    virtual ~DimChannel();
    virtual const std::string name() = 0;
    virtual std::vector<uint8_t> getHWPorts() = 0;
    virtual uint8_t getChannelIndex() = 0;

    // dimmchannel
    virtual void setup(uint8_t *hwchannel);
    virtual void processInputKo(GroupObject &ko);
    virtual void task();
    virtual void setDayNight(bool value);
    virtual uint8_t getChannelType();
    virtual void setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness);
    virtual void resend() {} // aktuellen Ausgabewert neu in die HW schreiben (z.B. nach Fehler-Freigabe)

private:
};

#endif