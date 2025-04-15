#ifndef DIMCHANNEL_H
#define DIMCHANNEL_H

#include <OpenKNX.h>

#include "LEDModule.h"
#include "HwChannel.h"

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

private:
};

#endif