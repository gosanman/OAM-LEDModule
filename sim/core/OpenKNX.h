#pragma once
// ============================================================================
// Simulator: Ersatz fuer OGM-Common/OpenKNX.h.
//
// Bildet genau die Teile von OpenKNX und dem KNX-Stack nach, die LEDModule,
// MeasuringModule und FrontPanelModule benutzen. Der Firmware-Code selbst wird
// UNVERAENDERT uebersetzt; nur was er von aussen aufruft, kommt von hier:
// Parameterspeicher, Gruppenobjekte, Logger, Konsole, Flash, Info-LEDs,
// Funktionstaste, Uhrzeit und Sonnenstand.
//
// Die DPT-Kodierung ist die ECHTE der KNX-Bibliothek (dptconvert.cpp,
// knx_value.cpp): ein Wert wird beim Senden so kodiert und beim Empfangen so
// dekodiert wie am Bus.
// ============================================================================

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "Arduino.h"
#include "Wire.h"
#include "hardware.h"

#include "dpt.h"
#include "dptconvert.h"
#include "knx_value.h"

// ---------------------------------------------------------------- KNX-Stack

enum ParameterFloatEncodings
{
    Float_Enc_DPT9 = 0,
    Float_Enc_IEEE754Single = 1,
    Float_Enc_IEEE754Double = 2,
};

/// Ein Gruppenobjekt. Haelt die Nutzdaten wie im echten Stack als Bytes.
class GroupObject
{
  public:
    uint16_t asap() const { return mNumber; }

    /// Empfangener oder zuletzt gesendeter Wert, dekodiert mit `type`.
    KNXValue value(const Dpt &type);
    /// Wert setzen UND senden - wie GroupObject::value() im Stack.
    void value(const KNXValue &value, const Dpt &type);
    /// Wert setzen, ohne zu senden. false: DPT nicht kodierbar.
    bool valueNoSend(const KNXValue &value, const Dpt &type);
    /// Wie im Stack: setzt den Wert und meldet, ob er sich geaendert hat
    /// (ein nie gesetztes Objekt gilt als geaendert).
    bool valueNoSendCompare(const KNXValue &value, const Dpt &type);
    /// Senden mit dem aktuellen Inhalt.
    void objectWritten();

    // --- nur Simulator ------------------------------------------------------
    void simInit(uint16_t number)
    {
        mNumber = number;
        std::memset(mData, 0, sizeof(mData));
        mInitialized = false;
        mLastDpt = Dpt();
    }
    /// Telegramm vom Bus: Bytes so ablegen, wie sie ankommen.
    bool simReceive(const KNXValue &value, const Dpt &type);
    const uint8_t *simData() const { return mData; }

  private:
    uint16_t mNumber = 0;
    uint8_t mData[16] = {};
    bool mInitialized = false;
    Dpt mLastDpt;
};

class SimKnx
{
  public:
    uint8_t paramByte(uint32_t addr);
    uint16_t paramWord(uint32_t addr);
    uint32_t paramInt(uint32_t addr);
    double paramFloat(uint32_t addr, ParameterFloatEncodings enc);
    uint8_t *paramData(uint32_t addr);
    char *paramString(uint32_t addr, uint32_t length);
    GroupObject &getGroupObject(uint16_t number);
    bool configured();
    void loop() {}
};

extern SimKnx knx;

/// Wie OpenKNX: true, wenn seit `last` mindestens `duration` ms vergangen sind.
bool delayCheck(uint32_t last, uint32_t duration);

// ---------------------------------------------------------------- OpenKNX

#define CONSOLE_HEADLINE_COLOR 33

namespace OpenKNX
{
    class Base
    {
      public:
        virtual ~Base() = default;
        virtual const std::string name() = 0;
        virtual void init() {}
        virtual void setup(bool configured)
        {
            if (configured)
                setup();
        }
        virtual void setup() {}
        virtual void loop(bool configured)
        {
            if (configured)
                loop();
        }
        virtual void loop() {}
        virtual void setup1(bool configured)
        {
            if (configured)
                setup1();
        }
        virtual void setup1() {}
        virtual void loop1(bool configured)
        {
            if (configured)
                loop1();
        }
        virtual void loop1() {}
        virtual void processInputKo(GroupObject &ko) { (void)ko; }
        virtual bool processFunctionProperty(uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t *, uint8_t &) { return false; }
        virtual bool processFunctionPropertyState(uint8_t, uint8_t, uint8_t, uint8_t *, uint8_t *, uint8_t &) { return false; }

      protected:
        virtual const std::string logPrefix() { return name(); }
    };

    class Channel : public Base
    {
      protected:
        uint8_t _channelIndex = 0;
        const std::string logPrefix() override
        {
            return name() + "<" + std::to_string(_channelIndex + 1) + ">";
        }

      public:
        virtual uint8_t channelIndex() { return _channelIndex; }
    };

    class Module : public Base
    {
      public:
        virtual const std::string version() = 0;
        virtual uint16_t flashSize() { return 0; }
        virtual void writeFlash() {}
        virtual void readFlash(const uint8_t *data, const uint16_t size)
        {
            (void)data;
            (void)size;
        }
        virtual void processAfterStartupDelay() {}
        virtual void processBeforeRestart() {}
        virtual void savePower() {}
        virtual bool restorePower() { return true; }
        virtual bool processCommand(const std::string cmd, bool diagnoseKo)
        {
            (void)cmd;
            (void)diagnoseKo;
            return false;
        }
        virtual void showHelp() {}
    };

    // ------------------------------------------------------------ Zeit
    struct TimeOnly
    {
        uint8_t hour = 0;
        uint8_t minute = 0;
        uint8_t second = 0;
    };

    struct SimDateTime
    {
        int year = 2026;
        int month = 1;
        int day = 1;
        int hour = 0;
        int minute = 0;
        int second = 0;
    };

    class SimTime
    {
      public:
        bool isValid();
        SimDateTime getLocalTime();
    };

    class SimSun
    {
      public:
        bool isSunCalculatioValid();
        TimeOnly sunRiseLocalTime();
        TimeOnly sunSetLocalTime();
    };

    // ------------------------------------------------------------ LEDs
    namespace Led
    {
        enum LedType
        {
            LED_TYPE_PROG = 0,
            LED_TYPE_INFO1 = 1,
            LED_TYPE_INFO2 = 2,
            LED_TYPE_INFO3 = 3,
            LED_TYPE_USER = 10
        };

        /// Eine LED, so wie die Firmware sie schaltet.
        class Base
        {
          public:
            void on(bool active = true)
            {
                mMode = active ? 1 : 0;
                mFreq = 0;
            }
            void off() { on(false); }
            void blinking(uint16_t frequency = 1000)
            {
                mMode = 2;
                mFreq = frequency;
            }
            void pulsing(uint16_t duration = 1000)
            {
                mMode = 3;
                mFreq = duration;
            }
            void flash(uint8_t = 1, uint16_t = 0) { mFlashes++; }

            // --- nur Simulator ---------------------------------------------
            uint8_t mode() const { return mMode; } ///< 0 aus, 1 an, 2 blinkt, 3 pulsiert
            uint16_t freq() const { return mFreq; }
            void simReset()
            {
                mMode = 0;
                mFreq = 0;
                mFlashes = 0;
            }

          private:
            uint8_t mMode = 0;
            uint16_t mFreq = 0;
            uint32_t mFlashes = 0;
        };
    } // namespace Led

    class SimLeds
    {
      public:
        Led::Base *getLed(uint8_t type) { return &mLeds[type < 4 ? type : 0]; }
        void simReset()
        {
            for (auto &l : mLeds)
                l.simReset();
        }

      private:
        Led::Base mLeds[4];
    };

    // ------------------------------------------------------------ Konsole, Logger
    class SimConsole
    {
      public:
        void printHelpLine(const char *cmd, const char *text);
        void writeDiagenoseKo(const char *fmt, ...);
    };

    class SimLogger
    {
      public:
        void logMacroWrapper(int color, const char *prefix, const char *fmt, ...);
        void logWithPrefixAndValues(const char *prefix, const char *fmt, ...);
        void logWithPrefixAndValues(const std::string &prefix, const char *fmt, ...);
        void log(const char *text);
        void color(int c) { (void)c; }
        void indentUp() { mIndent++; }
        void indentDown()
        {
            if (mIndent > 0)
                mIndent--;
        }
        std::string buildUptime();

      private:
        int mIndent = 0;
    };

    /// Flash wie OpenKNX: je Modul ein Block, sequentiell geschrieben/gelesen.
    class SimFlash
    {
      public:
        void save(bool force = false);
        void writeByte(uint8_t v);
        void writeWord(uint16_t v);
        void writeInt(uint32_t v);
        void writeFloat(float v);
        void writeDouble(double v);
        uint8_t readByte();
        uint16_t readWord();
        uint32_t readInt();
        float readFloat();
        double readDouble();

        // --- nur Simulator -------------------------------------------------
        std::vector<uint8_t> *writeTarget = nullptr;
        const uint8_t *readPtr = nullptr;
        const uint8_t *readEnd = nullptr;
    };

    class SimButton
    {
      public:
        void onShortClick(std::function<void()> cb) { shortCb = cb; }
        void onLongClick(std::function<void()> cb) { longCb = cb; }
        void onDoubleClick(std::function<void()> cb) { doubleCb = cb; }
        std::function<void()> shortCb, longCb, doubleCb;
    };

    class SimInfo
    {
      public:
        std::string firmwareName();
        std::string humanIndividualAddress();
    };

    struct SimOpenKnx
    {
        SimConsole console;
        SimLogger logger;
        SimFlash flash;
        SimLeds leds;
        SimButton func1Button;
        SimTime time;
        SimSun sun;
        SimInfo info;
    };
} // namespace OpenKNX

extern OpenKNX::SimOpenKnx openknx;

#define logInfoP(...) openknx.logger.logMacroWrapper(0, logPrefix().c_str(), __VA_ARGS__)
#define logErrorP(...) openknx.logger.logMacroWrapper(31, logPrefix().c_str(), __VA_ARGS__)
#define logDebugP(...) openknx.logger.logMacroWrapper(90, logPrefix().c_str(), __VA_ARGS__)
#define logInfo(...) openknx.logger.logMacroWrapper(0, __VA_ARGS__)
#define logError(...) openknx.logger.logMacroWrapper(31, __VA_ARGS__)
#define logDebug(...) openknx.logger.logMacroWrapper(90, __VA_ARGS__)
#define logIndentUp() openknx.logger.indentUp()
#define logIndentDown() openknx.logger.indentDown()

#include "knxprod.h"
