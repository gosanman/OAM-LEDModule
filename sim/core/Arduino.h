#pragma once
// ============================================================================
// Simulator: Ersatz fuer die Arduino-Plattform (arduino-pico).
//
// Nur was die Firmware und die mituebersetzten Adafruit-Bibliotheken (PWM-
// Treiber, BusIO, GFX, SSD1306) benutzen: Grundtypen, Zeit, GPIO der Taster,
// String und Print. Zeit und Pegel kommen aus dem Simulator (sim_hw.cpp).
// ============================================================================

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#ifndef ARDUINO
#define ARDUINO 10819 // auch per -D, weil Adafruit_GFX.h es vor Arduino.h abfragt
#endif

typedef uint8_t byte;
typedef uint16_t word;
typedef bool boolean;
typedef unsigned int uint; // newlib (RP2040) kennt uint

class __FlashStringHelper;

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define FALLING 2
#define RISING 3

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)

// Flash-Konstanten liegen am PC im normalen Speicher. Zeigerbreite beachten:
// unsigned long ist unter Windows 32 Bit, ein Zeiger 64 Bit.
#define PROGMEM
#define PSTR(s) (s)
#define F(s) (s)
// Nur falls noch nicht definiert: Adafruit_SSD1306.cpp legt pgm_read_byte vor
// dem Einbinden selbst an. pgm_read_pointer definiert Adafruit_GFX.cpp ohnehin
// selbst (ueber pgm_read_dword, am PC also auf 32 Bit gekuerzt) - das betrifft
// nur GFXfont-Schriften, die die Firmware nicht nutzt.
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const uint32_t *)(addr))
#endif

uint32_t millis();
uint32_t micros();
void delay(uint32_t ms);
void delayMicroseconds(uint32_t us);
void yield();

void pinMode(int pin, int mode);
int digitalRead(int pin);
void digitalWrite(int pin, int value);

template <typename T, typename L, typename H>
inline T constrain(T x, L lo, H hi) { return x < lo ? lo : (x > hi ? hi : x); }

// min/max wie im arduino-pico-Kern (api/Common.h): auch fuer gemischte Typen
template <class T, class L>
inline auto min(const T &a, const L &b) -> decltype((b < a) ? b : a) { return (b < a) ? b : a; }
template <class T, class L>
inline auto max(const T &a, const L &b) -> decltype((b < a) ? b : a) { return (a < b) ? b : a; }
using std::isfinite;
using std::isnan;

// ------------------------------------------------------------ String
class String
{
  public:
    String() = default;
    String(const char *s) : mS(s ? s : "") {}
    String(const std::string &s) : mS(s) {}
    String(char c) : mS(1, c) {}
    String(int v) : mS(std::to_string(v)) {}
    String(unsigned int v) : mS(std::to_string(v)) {}
    String(long v) : mS(std::to_string(v)) {}
    String(unsigned long v) : mS(std::to_string(v)) {}
    String(float v, int decimals = 2) { fmt(v, decimals); }
    String(double v, int decimals = 2) { fmt(v, decimals); }

    const char *c_str() const { return mS.c_str(); }
    unsigned int length() const { return static_cast<unsigned int>(mS.size()); }
    String &operator+=(const String &o) { mS += o.mS; return *this; }
    friend String operator+(const String &a, const String &b) { return String(a.mS + b.mS); }
    friend String operator+(const char *a, const String &b) { return String(std::string(a) + b.mS); }
    friend String operator+(const String &a, const char *b) { return String(a.mS + b); }
    bool operator==(const String &o) const { return mS == o.mS; }

  private:
    void fmt(double v, int decimals)
    {
        char b[48];
        std::snprintf(b, sizeof(b), "%.*f", decimals, v);
        mS = b;
    }
    std::string mS;
};

// ------------------------------------------------------------ Print
class Print
{
  public:
    virtual ~Print() = default;
    virtual size_t write(uint8_t c) = 0;
    virtual size_t write(const uint8_t *buffer, size_t size)
    {
        size_t n = 0;
        while (size--)
            n += write(*buffer++);
        return n;
    }
    size_t write(const char *s) { return s ? write(reinterpret_cast<const uint8_t *>(s), std::strlen(s)) : 0; }
    size_t write(const char *buffer, size_t size) { return write(reinterpret_cast<const uint8_t *>(buffer), size); }

    size_t print(const char *s) { return write(s); }
    size_t print(const __FlashStringHelper *s) { return write(reinterpret_cast<const char *>(s)); }
    size_t print(const String &s) { return write(s.c_str()); }
    size_t print(char c) { return write(static_cast<uint8_t>(c)); }
    size_t print(unsigned char v, int base = DEC) { return print(static_cast<unsigned long>(v), base); }
    size_t print(int v, int base = DEC) { return print(static_cast<long>(v), base); }
    size_t print(unsigned int v, int base = DEC) { return print(static_cast<unsigned long>(v), base); }
    size_t print(long v, int base = DEC)
    {
        if (base == DEC)
            return write(std::to_string(v).c_str());
        return print(static_cast<unsigned long>(v), base);
    }
    size_t print(unsigned long v, int base = DEC)
    {
        char b[40];
        if (base == HEX)
            std::snprintf(b, sizeof(b), "%lX", v);
        else if (base == OCT)
            std::snprintf(b, sizeof(b), "%lo", v);
        else if (base == BIN)
        {
            std::string s;
            do
            {
                s.insert(s.begin(), static_cast<char>('0' + (v & 1)));
                v >>= 1;
            } while (v);
            return write(s.c_str());
        }
        else
            std::snprintf(b, sizeof(b), "%lu", v);
        return write(b);
    }
    size_t print(double v, int digits = 2)
    {
        char b[64];
        std::snprintf(b, sizeof(b), "%.*f", digits, v);
        return write(b);
    }
    size_t println() { return write("\r\n"); }
    template <typename T>
    size_t println(const T &v) { return print(v) + println(); }
    template <typename T>
    size_t println(const T &v, int b) { return print(v, b) + println(); }
};

/// Serielle Schnittstelle: Ausgaben der Bibliotheken werden verworfen.
class SimSerial : public Print
{
  public:
    void begin(unsigned long) {}
    size_t write(uint8_t) override { return 1; }
    using Print::write;
    explicit operator bool() const { return true; }
};
extern SimSerial Serial;
