#ifndef TMP100_WE_H_
#define TMP100_WE_H_

#include "Arduino.h"
#include <Wire.h>

typedef enum TMP100_RESOLUTION_MODE{
    RES05      = 0b00000000,    // 9 Bits (0.5°C) - 40ms CONVERSION TIME (typical)
    RES025     = 0b00100000,    // 10 Bits (0.25°C) - 80ms CONVERSION TIME (typical)
    RES0125    = 0b01000000,    // 11 Bits (0.125°C) - 160ms CONVERSION TIME (typical)
    RES00625   = 0b01100000     // 12 Bits (0.0625°C) - 320ms CONVERSION TIME (typical)
} TMP100_resolutionMode;

class TMP100_WE
{
    public:
        /* registers */
        static constexpr uint8_t TMP100_ADDRESS             {0x48};
        static constexpr uint8_t TMP100_TEMP_REG            {0x00}; //Configuration Register
        static constexpr uint8_t TMP100_CONF_REG            {0x01}; //Configuration Register
        /* parameters, flag bits */
        static constexpr uint8_t TMP100_RST                 {0x80}; //Reset 
        // Constructors: if not passed, 0x48 / Wire will be set as address / wire object
        TMP100_WE(const int addr = 0x48) : _wire{&Wire}, i2cAddress{addr} {}
        TMP100_WE(TwoWire *w, const int addr = 0x48) : _wire{w}, i2cAddress{addr} {}

        bool init();
        void reset_TMP100();
        void setResolution(TMP100_RESOLUTION_MODE resolution);
        float getTemperature();

    protected:
        TwoWire *_wire;
        int i2cAddress;
        void writeRegister(uint8_t reg, uint16_t val);
        uint16_t readRegister(uint8_t reg);
        TMP100_RESOLUTION_MODE deviceResolutionMode;
};

#endif