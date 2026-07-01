#include "LIB_INA.h"

INASensor::INASensor(TwoWire *wire, uint8_t i2cAddress)
{
  _wire = wire;
  _address = i2cAddress;
  _shuntR = 0.0;
  _maxCurrent = 0.0;
  _currentLSB = 0.0;
  _powerLSB = 0.0;
}

bool INASensor::detectChipType()
{
  write16(0x00, 0x8000); // Force reset
  _id = read16(0x00);    // Read reset Register value
  if (_id == 0x4127)
  {                        // INA226, INA230, INA231 Family after reset
    _dieId = read16(0xFF); // Try INA226 Die ID first
    if (_dieId == 0x2260)
    {
      _chipType = INA226_TYPE;
      return true;
    }
  }
  if (_id == 0x0000)
  {                        // INA228 Family after reset
    _dieId = read16(0x3F); // Try INA228 Device ID
    if ((_dieId & 0xFFF0) == 0x2280)
    {
      _chipType = INA228_TYPE;
      return true;
    }
  }
  return false; // Unknown
}

bool INASensor::begin(float shuntResistance, float maxCurrent, ConversionTimeCode ct, Averaging avg)
{
  _shuntR = shuntResistance;
  _maxCurrent = maxCurrent;
  _wire->begin();

  if (!detectChipType())
  {
    return false; // Unable to detect chip
  }
  if (_chipType == INA226_TYPE)
  {
    if (!initINA226(ct, avg))
      return false;
    calibrateINA226();
  }
  else if (_chipType == INA228_TYPE)
  {
    if (!initINA228(ct, avg))
      return false;
    calibrateINA228();
  }
  else
  {
    return false; // Invalid chip type
  }
  return true;
}

bool INASensor::initINA226(ConversionTimeCode ct, Averaging avg)
{
  // Configure: Continuous shunt and bus
  uint16_t config = (static_cast<uint16_t>(avg) << 9) | // AVG Bits 9-11
                    (static_cast<uint16_t>(ct) << 6) |  // VBUSCT Bits 6-8
                    (static_cast<uint16_t>(ct) << 3) |  // VSHCT Bits 3-5
                    0x07;                               // MODE=111 (continuous shunt+bus)
  write16(INA226_CONFIG_REG, config);
  return true;
}

bool INASensor::initINA228(ConversionTimeCode ct, Averaging avg)
{
  // Configure: ADC continuous all
  uint16_t adcConfig = (0x0F << 12) |                     // MODE=1111 (continuous all)   INA228_MODE_CONT_TEMP_BUS_SHUNT
                       (static_cast<uint16_t>(ct) << 9) | // VBUSCT Bits 9-11             setBusVoltageConversionTime
                       (static_cast<uint16_t>(ct) << 6) | // VSHCT Bits 6-8               setShuntVoltageConversionTime
                       (static_cast<uint16_t>(ct) << 3) | // VTCT Bits 3-5                setTemperatureConversionTime
                       static_cast<uint16_t>(avg);        // AVG Bits 0-2                 setAverageingCount
  write16(INA228_ADC_CONFIG_REG, adcConfig);
  return true;
}

void INASensor::calibrateINA226()
{
  _currentLSB = _maxCurrent / 32768.0; // For 15-bit effective
  _cal = (uint16_t)(0.00512 / (_currentLSB * _shuntR));
  write16(INA226_CALIB_REG, _cal);
  _powerLSB = 25.0 * _currentLSB;
}

void INASensor::calibrateINA228()
{
  _currentLSB = _maxCurrent / 524288.0; // For 19-bit effective
  // Compute calibration using double precision, then round and clamp
  // SHUNT_CAL is bits 14..0 (15 bits) — ensure we do not set reserved bit 15
  double tmp = 13107200000.0 * (double)_currentLSB * (double)_shuntR; // Adjusted for ADCRANGE=0
  unsigned long long rounded = 0;
  if (tmp > 0.0)
  {
    rounded = (unsigned long long)(tmp + 0.5);
  }
  if (rounded > 0x7FFFULL)
    rounded = 0x7FFFULL; // clamp to 15 bits
  _cal = (uint16_t)rounded;
  write16(INA228_SHUNT_CAL_REG, _cal);
  _powerLSB = 3.2 * _currentLSB;
}

float INASensor::getVoltage()
{
  if (_chipType == INA226_TYPE)
  {
    int16_t raw = read16(INA226_BUS_V_REG);
    return raw * 0.00125; // 1.25 mV/LSB
  }
  else if (_chipType == INA228_TYPE)
  {
    uint32_t raw = read24(INA228_VBUS_REG) >> 4; // 20-bit
    return raw * 0.0001953125;                   // 195.3125 uV/LSB
  }
  return 0.0f;
}

float INASensor::getCurrent()
{
  if (_chipType == INA226_TYPE)
  {
    int16_t raw = read16(INA226_CURRENT_REG);
    return raw * _currentLSB;
  }
  else if (_chipType == INA228_TYPE)
  {
    int32_t raw = (int32_t)(read24(INA228_CURRENT_REG) >> 4);
    if (raw & 0x00080000)
    {
      raw |= 0xFFF00000;
    }
    return raw * _currentLSB;
  }
  return 0.0f;
}

float INASensor::getPower()
{
  if (_chipType == INA226_TYPE)
  {
    uint16_t raw = read16(INA226_POWER_REG);
    return raw * _powerLSB; // raw * 25 * CURRENT_LSB
  }
  else if (_chipType == INA228_TYPE)
  {
    uint32_t raw = read24(INA228_POWER_REG);
    return raw * _powerLSB; // raw * 3.2 * CURRENT_LSB
  }
  return 0.0f;
}

double INASensor::getEnergy()
{
  if (_chipType != INA228_TYPE)
    return 0.0;
  double raw = read40(INA228_ENERGY_REG);
  double energyJ = raw * 16.0 * _powerLSB; // 16 * 3.2 * CURRENT_LSB * raw
  return energyJ / 3600.0;                 // To Wh
}

void INASensor::resetEnergy()
{
  if (_chipType != INA228_TYPE)
    return; // Only for INA228
  uint16_t config = read16(INA228_CONFIG_REG);
  config |= 0x4000;                   // Set Bit 14 (RSTACC)
  write16(INA228_CONFIG_REG, config); // Bit self-clears, no need to reset it manually
}

float INASensor::getTemperature()
{
  if (_chipType != INA228_TYPE)
    return 0.0;
  int16_t raw = read16(INA228_DIETEMP_REG);
  return raw * 0.0078125; // 7.8125 mC/LSB
}

int INASensor::getChipType()
{
  return _chipType;
}

void INASensor::resetChip()
{
  uint8_t configReg = (_chipType == INA226_TYPE) ? INA226_CONFIG_REG : INA228_CONFIG_REG;
  write16(configReg, 0x8000); // Set RST bit (Bit 15), reset also the energy accumulator
  delay(10);                  // Short delay for reset to complete
}

/*
INA228 DIAG/ALRT Register Bits:
----------------------------------------
ALATCH  bit15  = 0x8000 (Latching Alert)        0h = Transparent, 1h = Latched
APOL    bit12  = 0x1000 (Alert Polarity Invert) 0h = Normal (Active-low, open-drain), 1h = Inverted (active-high, open-drain )
TMPOL   bit7   = 0x0080 (Temperatur Overlimit)  0h = Normal, 1h = Over Temp Event
SHNTOL  bit6   = 0x0040 (Shunt Overvoltage)     0h = Normal, 1h = Over Shunt Event
SHNTUL  bit5   = 0x0020 (Shunt Undervoltage)    0h = Normal, 1h = Under Shunt Event
BUSOL   bit4   = 0x0010 (Bus Overvoltage)       0h = Normal, 1h = Bus Over-Limit Event
BUSUL   bit3   = 0x0008 (Bus Undervoltage)      0h = Normal, 1h = Bus Under-Limit Event
POL     bit2   = 0x0004 (Power Over‑Limit)      0h = Normal, 1h = Power Over-Limit Event

INA226 MASK/ENABLE Register Bits:
----------------------------------------
POL     bit11  = 0x0800 (Power Over‑Limit)      0h = Normal, 1h = Power Over-Limit Event
BUSUL   bit12  = 0x1000 (Bus Undervoltage)      0h = Normal, 1h = Bus Under-Limit Event
BUSOL   bit13  = 0x2000 (Bus Overvoltage)       0h = Normal, 1h = Bus Over-Limit Event
SHNTUL  bit14  = 0x4000 (Shunt Undervoltage)    0h = Normal, 1h = Under Shunt Event
SHNTOL  bit15  = 0x8000 (Shunt Overvoltage)     0h = Normal, 1h = Over Shunt Event
LEN     bit0   = 0x0001 (Latch Enable)          0h = Transparent, 1h = Latched
POL     bit1   = 0x0002 (Alert Polarity Invert) 0h = Normal (Active-low, open-drain), 1h = Inverted (active-high, open-drain)
*/

void INASensor::configureAlert(AlertType type, float limit, bool latch, bool invertPolarity)
{
  uint16_t limitReg = 0;
  uint16_t limitVal = 0;

  if (type == ALERT_OVER_CURRENT)
  {
    limit *= _shuntR;                // Convert current (A) to shunt voltage (V)
    type = ALERT_SHUNT_OVER_VOLTAGE; // Map to shunt over voltage
  }

  if (_chipType == INA226_TYPE) // INA226
  {
    uint16_t mask = read16(INA226_MASK_EN_REG);
    mask &= ~0xF800; // Clear alert bits (bits 11-15)
    if (latch)
      mask |= 0x0001;
    if (invertPolarity)
      mask |= 0x0002;
    switch (type)
    {
    case ALERT_SHUNT_OVER_VOLTAGE:
      mask |= 0x8000;
      limitReg = INA226_ALERT_LIM_REG;
      limitVal = (uint16_t)(limit / 0.0000025);
      break; // 2.5 uV/LSB
    case ALERT_BUS_OVER_VOLTAGE:
      mask |= 0x2000;
      limitReg = INA226_ALERT_LIM_REG;
      limitVal = (uint16_t)(limit / 0.00125);
      break; // 1.25 mV/LSB
    case ALERT_POWER_OVER_LIMIT:
      mask |= 0x0800;
      limitReg = INA226_ALERT_LIM_REG;
      limitVal = (uint16_t)(limit / _powerLSB);
      break;
    default:
      return; // Not supported
    }
    write16(limitReg, limitVal);
    write16(INA226_MASK_EN_REG, mask);
  }
  else if (_chipType == INA228_TYPE) // INA228
  {
    uint16_t diag = read16(INA228_DIAG_ALRT_REG);
    diag &= ~0x00FC; // Clear alert bits (bits 2-7)
    if (latch)
      diag |= 0x8000; // ALATCH (bit 15)
    if (invertPolarity)
      diag |= 0x1000; // APOL (bit 12)
    switch (type)
    {
    case ALERT_SHUNT_OVER_VOLTAGE:
      diag |= 0x0040;
      limitReg = INA228_SOVL_REG;
      limitVal = (uint16_t)(limit / 0.000005);
      break; // 5 uV/LSB (ADCRANGE=0) or 1.25 uV/LSB (ADCRANGE=1)
    case ALERT_BUS_OVER_VOLTAGE:
      diag |= 0x0010;
      limitReg = INA228_BOVL_REG;
      limitVal = (uint16_t)(limit / 0.003125);
      break; // 3.125 mV/LSB
    case ALERT_POWER_OVER_LIMIT:
      diag |= 0x0004;
      limitReg = INA228_PWR_LIM_REG;
      limitVal = (uint16_t)(limit / (256.0 * _powerLSB));
      break;
    case ALERT_TEMPERATURE_OVER_LIMIT:
      diag |= 0x0080;
      limitReg = INA228_TEMP_LIM_REG;
      limitVal = (uint16_t)(limit / 0.0078125);
      break;
    default:
      return;
    }
    write16(limitReg, limitVal);
    write16(INA228_DIAG_ALRT_REG, diag);
  }
}

/*
INA22x MASK/ENABLE Register Bits:
----------------------------------------
bit0 = POL      (Power Over‑Limit)          (DIAG bit2 for INA228)  (MASK bit11 for INA226)
bit1 = BUSUL    (Bus Undervoltage)          (DIAG bit3 for INA228)  (MASK bit12 for INA226)
bit2 = BUSOL    (Bus Overvoltage)           (DIAG bit4 for INA228)  (MASK bit13 for INA226)
bit3 = SHNTUL   (Shunt Undervoltage)        (DIAG bit5 for INA228)  (MASK bit14 for INA226)
bit4 = SHNTOL   (Shunt Overvoltage)         (DIAG bit6 for INA228)  (MASK bit15 for INA226)
bit5 = TMPOL    (Temperature Overlimit)     (DIAG bit7 for INA228)  (MASK bit15 for INA226)
*/

uint8_t INASensor::getAlertFlags()
{
  if (_chipType == INA226_TYPE)
  {
    uint16_t reg = read16(INA226_MASK_EN_REG);
    bool limitAlert = (reg >> 4) & 0x0001;
    uint8_t out = (uint8_t)((((reg & 0xF800) >> 11) & 0x1F) * limitAlert); // bits 11-15 -> bits 0-4
    return out & 0x3F;
  }
  else if (_chipType == INA228_TYPE)
  {
    uint16_t reg = read16(INA228_DIAG_ALRT_REG);
    return (uint8_t)((reg >> 2) & 0x3F); // DIAG bits 2..7 -> returned bits 0..5
  }
  return 0;
}

bool INASensor::readLimitRegisters(uint16_t &sovl, uint16_t &bovl, uint16_t &pwr_lim, uint16_t &temp_lim)
{
  // default outputs
  sovl = bovl = pwr_lim = temp_lim = 0;

  if (_chipType == INA226_TYPE)
  {
    // INA226 exposes mask/alert register and a single alert limit reg
    sovl = read16(INA226_ALERT_LIM_REG); // INA226 uses ALERT_LIM_REG for limits
    // INA226 does not have separate SOVL/BOVL/PWR/TEMP
    return true;
  }
  else if (_chipType == INA228_TYPE)
  {
    sovl = read16(INA228_SOVL_REG);         // Shunt overvoltage threshold (two's complement)
    bovl = read16(INA228_BOVL_REG);         // Bus overvoltage threshold
    pwr_lim = read16(INA228_PWR_LIM_REG);   // Power limit register
    temp_lim = read16(INA228_TEMP_LIM_REG); // Temperature limit register
    return true;
  }
  return false; // unknown/unsupported chip
}

uint16_t INASensor::read16(uint8_t reg)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom(_address, (uint8_t)2);
  if (_wire->available() < 2)
    return 0;
  return (_wire->read() << 8) | _wire->read();
}

uint32_t INASensor::read24(uint8_t reg)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom(_address, (uint8_t)3);
  if (_wire->available() < 3)
    return 0;
  return (_wire->read() << 16) | (_wire->read() << 8) | _wire->read();
}

uint64_t INASensor::read40(uint8_t reg)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission();
  _wire->requestFrom(_address, (uint8_t)5);
  if (_wire->available() < 5)
    return 0;
  return ((uint64_t)_wire->read() << 32) | ((uint64_t)_wire->read() << 24) | ((uint64_t)_wire->read() << 16) | ((uint64_t)_wire->read() << 8) | _wire->read();
}

void INASensor::write16(uint8_t reg, uint16_t value)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value >> 8);
  _wire->write(value & 0xFF);
  _wire->endTransmission();
}