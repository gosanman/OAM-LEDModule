#pragma once
// ============================================================================
// Simulator: SPI-Ersatz. Das Display haengt am I2C-Bus; die SPI-Zweige der
// SSD1306-Bibliothek werden nur uebersetzt, nie ausgefuehrt.
// ============================================================================

#include <cstdint>

#define SPI_HAS_TRANSACTION
#define MSBFIRST 1
#define LSBFIRST 0
#define SPI_MODE0 0

class SPISettings
{
  public:
    SPISettings() = default;
    SPISettings(uint32_t, uint8_t, uint8_t) {}
};

class SPIClass
{
  public:
    void begin() {}
    void end() {}
    void beginTransaction(SPISettings) {}
    void endTransaction() {}
    uint8_t transfer(uint8_t) { return 0; }
    void transfer(void *, size_t) {}
};

extern SPIClass SPI;
