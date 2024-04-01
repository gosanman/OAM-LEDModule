#include "DimChannel.h"

DimChannel::DimChannel(uint8_t index) {
  _channelIndex = index;
}

DimChannel::~DimChannel() {}

void DimChannel::setup(uint8_t* hwchannel, uint16_t startKO) {}

void DimChannel::processInputKo(GroupObject &ko) {}

void DimChannel::task() {}

void DimChannel::setDayNight(bool value) {}
