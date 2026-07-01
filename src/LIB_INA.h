#ifndef INASENSOR_H
#define INASENSOR_H

#include <Arduino.h>
#include <Wire.h>

#define INA226_TYPE 226
#define INA228_TYPE 228
#define INA_UNKNOWN 000

// INA226 Register Addresses
#define INA226_CONFIG_REG     0x00
#define INA226_SHUNT_V_REG    0x01
#define INA226_BUS_V_REG      0x02
#define INA226_POWER_REG      0x03
#define INA226_CURRENT_REG    0x04
#define INA226_CALIB_REG      0x05
#define INA226_MASK_EN_REG    0x06
#define INA226_ALERT_LIM_REG  0x07
#define INA226_DIE_ID_REG     0xFF

// INA228 Register Addresses
#define INA228_CONFIG_REG     0x00
#define INA228_ADC_CONFIG_REG 0x01
#define INA228_SHUNT_CAL_REG  0x02
#define INA228_VSHUNT_REG     0x04
#define INA228_VBUS_REG       0x05
#define INA228_DIETEMP_REG    0x06
#define INA228_CURRENT_REG    0x07
#define INA228_POWER_REG      0x08
#define INA228_ENERGY_REG     0x09
#define INA228_DIAG_ALRT_REG  0x0B
#define INA228_SOVL_REG       0x0C
#define INA228_BOVL_REG       0x0E
#define INA228_TEMP_LIM_REG   0x10
#define INA228_PWR_LIM_REG    0x11
#define INA228_DEVICE_ID_REG  0x3F

// Alert Types (common for both, but mapped differently)
enum AlertType
{
  ALERT_SHUNT_OVER_VOLTAGE,
  ALERT_BUS_OVER_VOLTAGE,
  ALERT_POWER_OVER_LIMIT,
  ALERT_TEMPERATURE_OVER_LIMIT, // Only for INA228
  ALERT_OVER_CURRENT
};

// Averaging options (common for both chips)
enum Averaging
{
  AVG_1 = 0,
  AVG_4 = 1,
  AVG_16 = 2,
  AVG_64 = 3,
  AVG_128 = 4,
  AVG_256 = 5,
  AVG_512 = 6,
  AVG_1024 = 7
};

// Conversion Time codes (common codes, but times vary slightly by chip)
enum ConversionTimeCode
{
  CT_0 = 0, // INA226: 140 µs,   INA228: 50 µs
  CT_1 = 1, // INA226: 204 µs,   INA228: 84 µs
  CT_2 = 2, // INA226: 332 µs,   INA228: 150 µs
  CT_3 = 3, // INA226: 588 µs,   INA228: 280 µs
  CT_4 = 4, // INA226: 1.1 ms,   INA228: 540 µs
  CT_5 = 5, // INA226: 2.116 ms, INA228: 1.052 ms
  CT_6 = 6, // INA226: 4.156 ms, INA228: 2.074 ms
  CT_7 = 7  // INA226: 8.244 ms, INA228: 4.120 ms
};

class INASensor
{
public:
  INASensor(TwoWire *wire, uint8_t i2cAddress = 0x40);
  bool detectChipType(); // Detects and sets the chip type (INA226 or INA228)
  bool begin(float shuntResistance = 0.1, float maxCurrent = 1.0,
             ConversionTimeCode ct = CT_4, Averaging avg = AVG_64);                                  // Initializes the sensor
  float getVoltage();                                                                                // Bus Voltage in V
  float getCurrent();                                                                                // Current in A
  float getPower();                                                                                  // Power in W
  double getEnergy();                                                                                // Energy in Wh (INA228 only, returns 0 for INA226)
  void resetEnergy();                                                                                // Resets the energy accumulator (INA228 only)
  float getTemperature();                                                                            // Die Temperature in °C (INA228 only, returns 0 for INA226)
  int getChipType();                                                                                 // Returns the detected chip type
  void resetChip();                                                                                  // Resets the chip
  void configureAlert(AlertType type, float limit, bool latch = false, bool invertPolarity = false); // Configures alert
  uint8_t getAlertFlags();                                                                           // Reads and returns alert flags
  bool readLimitRegisters(uint16_t &sovl, uint16_t &bovl, uint16_t &pwr_lim, uint16_t &temp_lim);    // Reads limit registers

private:
  uint8_t _address;
  int _chipType;
  float _currentLSB;
  float _powerLSB;
  float _shuntR;
  float _maxCurrent;
  uint16_t _cal;
  uint16_t _id;
  uint16_t _dieId;
  TwoWire *_wire;

  uint16_t read16(uint8_t reg);
  uint32_t read24(uint8_t reg); // For 24-bit registers (INA228)
  uint64_t read40(uint8_t reg); // For 40-bit registers (INA228 Energy/Charge)
  void write16(uint8_t reg, uint16_t value);

  bool initINA226(ConversionTimeCode ct, Averaging avg);
  bool initINA228(ConversionTimeCode ct, Averaging avg);
  void calibrateINA226();
  void calibrateINA228();
};

#endif