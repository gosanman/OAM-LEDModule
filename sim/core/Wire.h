#pragma once
// ============================================================================
// Simulator: Ersatz fuer TwoWire (I2C).
//
// Ein Schreibvorgang (beginTransmission ... endTransmission) und ein
// Lesevorgang (requestFrom) gehen als ganze Transaktion an die Chip-Modelle
// in sim_hw.cpp: PCA9685, INA226, TMP100, SSD1306. Antwortet an der Adresse
// kein Chip (nicht bestueckt oder als Fehler simuliert), meldet endTransmission
// wie am Geraet 2 (NACK auf die Adresse).
// ============================================================================

#include <cstddef>
#include <cstdint>
#include <vector>

#define WIRE_HAS_TIMEOUT

namespace sim
{
    /// true: Chip hat die Adresse bestaetigt.
    bool i2cWrite(uint8_t addr, const uint8_t *data, size_t len);
    /// Anzahl gelieferter Bytes, 0 = NACK.
    size_t i2cRead(uint8_t addr, uint8_t *data, size_t len);
} // namespace sim

class TwoWire
{
  public:
    void begin() {}
    void end() {}
    void setClock(uint32_t hz) { mClock = hz; }
    void setSDA(int) {}
    void setSCL(int) {}
    void setWireTimeout(uint32_t = 25000, bool = false) {}
    uint32_t clock() const { return mClock; }

    void beginTransmission(uint8_t addr)
    {
        mAddr = addr;
        mTx.clear();
    }
    void beginTransmission(int addr) { beginTransmission(static_cast<uint8_t>(addr)); }
    size_t write(uint8_t b)
    {
        mTx.push_back(b);
        return 1;
    }
    size_t write(const uint8_t *data, size_t len)
    {
        mTx.insert(mTx.end(), data, data + len);
        return len;
    }
    size_t write(int b) { return write(static_cast<uint8_t>(b)); }
    uint8_t endTransmission(bool stop = true)
    {
        (void)stop;
        return sim::i2cWrite(mAddr, mTx.data(), mTx.size()) ? 0 : 2;
    }
    uint8_t endTransmission(uint8_t stop) { return endTransmission(stop != 0); }

    // Signaturen wie arduino-pico (TwoWire::requestFrom)
    size_t requestFrom(uint8_t addr, size_t qty, bool stop)
    {
        (void)stop;
        mRx.assign(qty, 0);
        const size_t n = sim::i2cRead(addr, mRx.data(), qty);
        mRx.resize(n);
        mRxPos = 0;
        return n;
    }
    size_t requestFrom(uint8_t addr, size_t qty) { return requestFrom(addr, qty, true); }

    int available() { return static_cast<int>(mRx.size() - mRxPos); }
    int read() { return mRxPos < mRx.size() ? mRx[mRxPos++] : -1; }
    int peek() { return mRxPos < mRx.size() ? mRx[mRxPos] : -1; }

  private:
    uint8_t mAddr = 0;
    uint32_t mClock = 100000;
    std::vector<uint8_t> mTx;
    std::vector<uint8_t> mRx;
    size_t mRxPos = 0;
};

extern TwoWire Wire;
extern TwoWire Wire1;
