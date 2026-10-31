#include "LIB_TEMP100.h"

bool TMP100_WE::init(){
    _wire->beginTransmission(i2cAddress);
    if(_wire->endTransmission()){
        return 0;
    }
    reset_TMP100();
    setResolution(RES05);
    return 1;
}

void TMP100_WE::reset_TMP100(){
    writeRegister(TMP100_CONF_REG, TMP100_RST); 
}

void TMP100_WE::setResolution(TMP100_RESOLUTION_MODE resolution){
    deviceResolutionMode = resolution;
    uint8_t currentConfReg = readRegister(TMP100_CONF_REG, 1);
    currentConfReg &= ~(0x60);
    currentConfReg |= deviceResolutionMode;
    writeRegister(TMP100_CONF_REG, currentConfReg);
}

float TMP100_WE::getTemperature(){
    int16_t val;
    val = static_cast<int16_t>(readRegister(TMP100_TEMP_REG));
    return (val / 16 * 0.0625);  
}

/************************************************ 
    private functions
*************************************************/

void TMP100_WE::writeRegister(uint8_t reg, uint8_t val){
  // Das TMP100-Konfigregister ist 1 Byte breit -> nur ein Datenbyte senden
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->write(val);
  _wire->endTransmission();
}

uint16_t TMP100_WE::readRegister(uint8_t reg, uint8_t bytes){
  uint16_t regValue = 0;
  _wire->beginTransmission(i2cAddress);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom(static_cast<uint8_t>(i2cAddress), bytes);
  if(bytes >= 2){
    uint8_t MSByte = 0, LSByte = 0;
    if(_wire->available() >= 2){
      MSByte = _wire->read();
      LSByte = _wire->read();
    }
    regValue = (MSByte<<8) + LSByte;
  } else {
    if(_wire->available()){
      regValue = _wire->read();
    }
  }
  return regValue;
}
    