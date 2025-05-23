#include "DimChannel.h"

DimChannel::DimChannel(uint8_t index)
{
  _channelIndex = index;
}

DimChannel::~DimChannel() {}

void DimChannel::setup(uint8_t *hwchannel) {}

void DimChannel::processInputKo(GroupObject &ko) {}

void DimChannel::task() {}

void DimChannel::setDayNight(bool value) {}

uint8_t DimChannel::getChannelType() { return 0; } 

void DimChannel::setHcl(uint8_t channel, uint16_t kelvin, uint8_t brightness) {}