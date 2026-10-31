// ============================================================================
// Simulator: Hardware am I2C-Bus und die angeschlossene Last.
//
// Die Bausteine sind auf Registerebene nachgebildet, damit die Treiber
// unveraendert mitlaufen (Adafruit PWM/BusIO/SSD1306, LIB_INA, LIB_TEMP100):
//
//   PCA9685 0x40  MODE1/MODE2, LEDn_ON/OFF mit Full-On/Full-Off-Bit,
//                 ALL_LED-Broadcast, PRE_SCALE (nur im Sleep schreibbar),
//                 Auto-Increment. MODE1 liest RESTART als 0.
//   INA226  0x41  Wandlung mit Mittelung nach CONFIG (CT, AVG), Kalibrierung,
//                 MASK/ENABLE mit Alert-Flag (AFF) und Latch.
//   TMP100  0x48  Temperatur 12 Bit, Konfigurationsregister.
//   SSD1306 0x3C  Befehls- und Datenstrom in den Grafikspeicher.
//
// Last: Jeder Port zieht bei 100 % die eingestellte Leistung aus der
// LED-Versorgung, proportional zum Tastverhaeltnis (Konstantspannungs-LED).
// Ein Kurzschluss zieht 10 A. Die Spannung sinkt am Innenwiderstand.
// ============================================================================

#include <cmath>
#include <cstring>

#include "sim.h"

TwoWire Wire;
TwoWire Wire1;

namespace sim
{
    Load load;
    Thermal thermal;
    Chips chips;
    bool chipsPowered = false;
    bool oledOn = false;
    bool oledChanged = false;
    uint64_t i2cFrames = 0;
    uint64_t i2cNacks = 0;
    double shuntOhm = 0.010;

    // ================================================================ PCA9685
    namespace
    {
        struct Pca9685
        {
            uint8_t reg[256] = {};
            uint8_t ptr = 0;

            void powerOn()
            {
                std::memset(reg, 0, sizeof(reg));
                reg[0x00] = 0x11; // SLEEP + ALLCALL
                reg[0x01] = 0x04; // OUTDRV
                reg[0xFE] = 0x1E; // 200 Hz
                for (int ch = 0; ch < 16; ch++)
                    reg[0x06 + ch * 4 + 3] = 0x10; // LEDn_OFF_H: Full-Off
                ptr = 0;
            }

            void writeReg(uint8_t r, uint8_t v)
            {
                if (r == 0x00)
                    reg[0] = v & 0x7F; // RESTART wird beim Schreiben ausgefuehrt und liest 0
                else if (r == 0xFE)
                {
                    if (reg[0] & 0x10) // Vorteiler nur im Sleep
                        reg[0xFE] = v;
                }
                else if (r >= 0xFA && r <= 0xFD)
                {
                    for (int ch = 0; ch < 16; ch++)
                        reg[0x06 + ch * 4 + (r - 0xFA)] = v; // Broadcast auf alle Kanaele
                }
                else
                    reg[r] = v;
            }

            void write(const uint8_t *d, size_t n)
            {
                if (n == 0)
                    return;
                ptr = d[0];
                for (size_t i = 1; i < n; i++)
                {
                    writeReg(ptr, d[i]);
                    if (reg[0] & 0x20)
                        ptr++;
                }
            }

            size_t read(uint8_t *d, size_t n)
            {
                for (size_t i = 0; i < n; i++)
                {
                    d[i] = (ptr >= 0xFA && ptr <= 0xFD) ? 0 : reg[ptr];
                    if (reg[0] & 0x20)
                        ptr++;
                }
                return n;
            }
        } pca;

        // ============================================================ INA226
        struct Ina226
        {
            uint16_t config = 0x4127, cal = 0, mask = 0, alertLim = 0;
            uint8_t ptr = 0;
            int16_t shuntReg = 0;
            uint16_t busReg = 0;
            bool aff = false;
            double accI = 0, accV = 0, accMs = 0, avgI = 0, avgV = 0;

            void powerOn()
            {
                config = 0x4127;
                cal = mask = alertLim = 0;
                ptr = 0;
                shuntReg = 0;
                busReg = 0;
                aff = false;
                accI = accV = accMs = 0;
            }

            double periodMs() const
            {
                static const double ct[] = {0.140, 0.204, 0.332, 0.588, 1.100, 2.116, 4.156, 8.244};
                static const int avg[] = {1, 4, 16, 64, 128, 256, 512, 1024};
                const double t = ct[(config >> 6) & 7] + ct[(config >> 3) & 7];
                return t * avg[(config >> 9) & 7];
            }

            int16_t currentReg() const
            {
                const long v = static_cast<long>(shuntReg) * cal / 2048;
                return static_cast<int16_t>(std::max(-32768L, std::min(32767L, v)));
            }

            uint16_t powerReg() const
            {
                const long v = std::labs(static_cast<long>(currentReg())) * busReg / 20000;
                return static_cast<uint16_t>(std::min(65535L, v));
            }

            void evalAlert()
            {
                bool cond = false;
                if (mask & 0x8000)
                    cond |= shuntReg > static_cast<int16_t>(alertLim);
                if (mask & 0x4000)
                    cond |= shuntReg < static_cast<int16_t>(alertLim);
                if (mask & 0x2000)
                    cond |= busReg > alertLim;
                if (mask & 0x1000)
                    cond |= busReg < alertLim;
                if (mask & 0x0800)
                    cond |= powerReg() > alertLim;
                if (cond)
                    aff = true;
                else if (!(mask & 0x0001))
                    aff = false; // transparent: Flag folgt dem Zustand
            }

            void step(double ms, double current, double volt)
            {
                if ((config & 0x07) == 0) // Power-Down
                    return;
                accI += current * ms;
                accV += volt * ms;
                accMs += ms;
                const double p = periodMs();
                if (accMs + 1e-9 < p)
                    return;
                avgI = accI / accMs;
                avgV = accV / accMs;
                accI = accV = accMs = 0;
                const double sh = std::round(avgI * shuntOhm / 2.5e-6);
                shuntReg = static_cast<int16_t>(std::max(-32768.0, std::min(32767.0, sh)));
                busReg = static_cast<uint16_t>(std::max(0.0, std::min(32767.0, std::round(avgV / 1.25e-3))));
                evalAlert();
            }

            uint16_t readReg(uint8_t r)
            {
                switch (r)
                {
                    case 0x00: return config;
                    case 0x01: return static_cast<uint16_t>(shuntReg);
                    case 0x02: return busReg;
                    case 0x03: return powerReg();
                    case 0x04: return static_cast<uint16_t>(currentReg());
                    case 0x05: return cal;
                    case 0x06:
                    {
                        const uint16_t v = static_cast<uint16_t>(mask | (aff ? 0x0010 : 0) | 0x0008); // CVRF
                        if (mask & 0x0001)
                            aff = false; // Latch: Lesen von MASK/ENABLE quittiert
                        return v;
                    }
                    case 0x07: return alertLim;
                    case 0xFE: return 0x5449;
                    case 0xFF: return 0x2260;
                    default: return 0;
                }
            }

            void write(const uint8_t *d, size_t n)
            {
                if (n == 0)
                    return;
                ptr = d[0];
                if (n < 3)
                    return; // nur Registerzeiger
                const uint16_t v = static_cast<uint16_t>((d[1] << 8) | d[2]);
                switch (ptr)
                {
                    case 0x00:
                        if (v & 0x8000)
                        {
                            powerOn();
                            ptr = 0;
                        }
                        else
                            config = v;
                        break;
                    case 0x05: cal = v & 0x7FFF; break;
                    case 0x06: mask = v & 0xFC03; break;
                    case 0x07: alertLim = v; break;
                    default: break;
                }
            }

            size_t read(uint8_t *d, size_t n)
            {
                const uint16_t v = readReg(ptr);
                if (n > 0)
                    d[0] = static_cast<uint8_t>(v >> 8);
                if (n > 1)
                    d[1] = static_cast<uint8_t>(v);
                for (size_t i = 2; i < n; i++)
                    d[i] = 0;
                return n;
            }
        } ina;

        // ============================================================ TMP100
        struct Tmp100
        {
            uint8_t ptr = 0;
            uint8_t conf = 0;

            void powerOn()
            {
                ptr = 0;
                conf = 0;
            }

            void write(const uint8_t *d, size_t n)
            {
                if (n == 0)
                    return;
                ptr = d[0] & 0x03;
                if (n >= 2 && ptr == 1)
                    conf = d[1];
            }

            size_t read(uint8_t *d, size_t n)
            {
                if (ptr == 0)
                {
                    const int16_t raw = static_cast<int16_t>(std::lround(thermal.tempC / 0.0625)) * 16;
                    if (n > 0)
                        d[0] = static_cast<uint8_t>(raw >> 8);
                    if (n > 1)
                        d[1] = static_cast<uint8_t>(raw);
                }
                else
                {
                    for (size_t i = 0; i < n; i++)
                        d[i] = ptr == 1 ? conf : 0;
                }
                return n;
            }
        } tmp;

        // ============================================================ SSD1306
        struct Ssd1306
        {
            uint8_t ram[128 * 8] = {};
            uint8_t col = 0, colStart = 0, colEnd = 127;
            uint8_t page = 0, pageStart = 0, pageEnd = 7;
            int pending = 0; ///< noch erwartete Parameterbytes des letzten Befehls
            uint8_t lastCmd = 0;
            int paramIdx = 0;

            void powerOn()
            {
                std::memset(ram, 0, sizeof(ram));
                col = colStart = 0;
                colEnd = 127;
                page = pageStart = 0;
                pageEnd = 7;
                pending = 0;
                oledOn = false;
                oledChanged = true;
            }

            static int paramCount(uint8_t c)
            {
                switch (c)
                {
                    case 0x20: case 0x81: case 0x8D: case 0xA8: case 0xD3: case 0xD5:
                    case 0xD9: case 0xDA: case 0xDB:
                        return 1;
                    case 0x21: case 0x22: case 0xA3:
                        return 2;
                    case 0x29: case 0x2A:
                        return 5;
                    case 0x26: case 0x27:
                        return 6;
                    default:
                        return 0;
                }
            }

            void command(uint8_t c)
            {
                if (pending > 0)
                {
                    if (lastCmd == 0x21)
                    {
                        if (paramIdx == 0) colStart = col = c & 0x7F;
                        else colEnd = c & 0x7F;
                    }
                    else if (lastCmd == 0x22)
                    {
                        if (paramIdx == 0) pageStart = page = c & 0x07;
                        else pageEnd = (c == 0xFF) ? 7 : (c & 0x07);
                    }
                    paramIdx++;
                    pending--;
                    return;
                }
                lastCmd = c;
                paramIdx = 0;
                pending = paramCount(c);
                if (c == 0xAE) { oledOn = false; oledChanged = true; }
                else if (c == 0xAF) { oledOn = true; oledChanged = true; }
            }

            void data(uint8_t b)
            {
                uint8_t &cell = ram[page * 128 + col];
                if (cell != b)
                {
                    cell = b;
                    oledChanged = true;
                }
                if (col >= colEnd)
                {
                    col = colStart;
                    page = page >= pageEnd ? pageStart : page + 1;
                }
                else
                    col++;
            }

            void write(const uint8_t *d, size_t n)
            {
                if (n == 0)
                    return;
                const bool isData = (d[0] & 0x40) != 0;
                for (size_t i = 1; i < n; i++)
                {
                    if (isData)
                        data(d[i]);
                    else
                        command(d[i]);
                }
            }
        } oled;

        // ============================================================ Taster
        uint64_t panelReleaseAt[4] = {0, 0, 0, 0};
    } // namespace

    // ================================================================ I2C-Bus

    bool i2cWrite(uint8_t addr, const uint8_t *data, size_t len)
    {
        i2cFrames++;
        if (!chipsPowered) { i2cNacks++; return false; }
        if (addr == I2C_PCA9685_DEVICE_ADDRESS && chips.pca) { pca.write(data, len); return true; }
        if (addr == I2C_INA22x_DEVICE_ADDRESS && chips.ina) { ina.write(data, len); return true; }
        if (addr == I2C_TMP100_DEVICE_ADDRESS && chips.tmp) { tmp.write(data, len); return true; }
        if (addr == I2C_SSD1306_DEVICE_ADDRESS && chips.oled) { oled.write(data, len); return true; }
        i2cNacks++;
        return false;
    }

    size_t i2cRead(uint8_t addr, uint8_t *data, size_t len)
    {
        i2cFrames++;
        if (!chipsPowered) { i2cNacks++; return 0; }
        if (addr == I2C_PCA9685_DEVICE_ADDRESS && chips.pca) return pca.read(data, len);
        if (addr == I2C_INA22x_DEVICE_ADDRESS && chips.ina) return ina.read(data, len);
        if (addr == I2C_TMP100_DEVICE_ADDRESS && chips.tmp) return tmp.read(data, len);
        i2cNacks++;
        return 0;
    }

    // ================================================================ Ausgaenge

    bool pcaSleeping() { return (pca.reg[0] & 0x10) != 0; }

    void portRegs(int port, uint16_t &on, uint16_t &off, bool &fullOn, bool &fullOff)
    {
        const uint8_t *r = &pca.reg[0x06 + port * 4];
        on = static_cast<uint16_t>(((r[1] & 0x0F) << 8) | r[0]);
        off = static_cast<uint16_t>(((r[3] & 0x0F) << 8) | r[2]);
        fullOn = (r[1] & 0x10) != 0;
        fullOff = (r[3] & 0x10) != 0;
    }

    double portDuty(int port)
    {
        if (port < 0 || port >= kPorts || !chipsPowered || !chips.pca || pcaSleeping())
            return 0.0;
        uint16_t on, off;
        bool fOn, fOff;
        portRegs(port, on, off, fOn, fOff);
        double d;
        if (fOff)
            d = 0.0;
        else if (fOn)
            d = 1.0;
        else
            d = ((off - on) & 0x0FFF) / 4096.0;
        if (pca.reg[1] & 0x10) // INVRT
            d = 1.0 - d;
        return d;
    }

    double pwmFrequency() { return 25e6 / (4096.0 * (pca.reg[0xFE] + 1)); }

    double loadCurrent()
    {
        if (!load.supplyOn)
            return 0.0;
        double i = load.idleA;
        for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
        {
            const double d = portDuty(p);
            i += d * (load.shortPort[p] ? 10.0 : load.portW[p] / 24.0);
        }
        return i;
    }

    double busVoltage()
    {
        if (!load.supplyOn)
            return 0.0;
        return std::max(0.0, load.supplyV - loadCurrent() * load.sourceOhm);
    }

    double inaCurrent() { return ina.avgI; }
    double inaVoltage() { return ina.avgV; }

    void hwStep(uint32_t ms)
    {
        const double i = loadCurrent();
        const double v = busVoltage();
        ina.step(ms, i, v);
        const double target = thermal.ambientC + thermal.kPerW * i * v;
        thermal.tempC += (target - thermal.tempC) * std::min(1.0, ms / 1000.0 / thermal.tauS);
    }

    void hwPowerOn()
    {
        pca.powerOn();
        ina.powerOn();
        tmp.powerOn();
        oled.powerOn();
    }

    std::string oledHex()
    {
        static const char *hx = "0123456789abcdef";
        std::string s;
        s.reserve(1024);
        for (int i = 0; i < 128 * 4; i++)
        {
            s += hx[oled.ram[i] >> 4];
            s += hx[oled.ram[i] & 0x0F];
        }
        return s;
    }

    void panelPress(int button, uint32_t holdMs)
    {
        if (button >= 0 && button < 4)
            panelReleaseAt[button] = now + holdMs;
    }

    namespace detail
    {
        int panelLevel(int pin)
        {
#ifdef FRONT_PANEL_PRESENT
            const int pins[4] = {IO1_PIN, IO2_PIN, IO3_PIN, IO4_PIN};
            for (int b = 0; b < 4; b++)
                if (pin == pins[b])
                    return now < panelReleaseAt[b] ? LOW : HIGH;
#else
            (void)pin;
#endif
            return HIGH;
        }
    } // namespace detail
} // namespace sim

// ------------------------------------------------------------ Arduino-Plattform

uint32_t millis() { return static_cast<uint32_t>(sim::now - sim::bootAt); }
uint32_t micros() { return static_cast<uint32_t>((sim::now - sim::bootAt) * 1000); }
void delay(uint32_t) {}
void delayMicroseconds(uint32_t) {}
void yield() {}
void pinMode(int, int) {}
void digitalWrite(int, int) {}
int digitalRead(int pin) { return sim::detail::panelLevel(pin); }
