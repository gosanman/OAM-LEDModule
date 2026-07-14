#ifndef LEDMODULE_H
#define LEDMODULE_H

#include <Adafruit_PWMServoDriver.h>
#include <OpenKNX.h>
#include "hardware.h"
#include "HclChannel.h"

#include "LEDHelper.h"

// Timing parameters
#define LED_CHECK_I2C           10000   // ms
#define HCL_TIMER_BROADCAST     60000   // ms

// Testmodus
#define TEST_SETTLE_MS          500     // ms - nach Port-An warten, bis der INA-Strom stabil ist
#define TEST_DWELL_MS           3000    // ms - Verweildauer je Port im Auto-Durchlauf
#define TEST_TIMEOUT_MS         300000  // ms - Auto-Ende nach Inaktivität (5 min)

class DimChannel_EK;
class DimChannel_TW;
class DimChannel_RGB;
class DimChannel;
class HWChannel;

class LEDModule : public OpenKNX::Module
{
public:
    LEDModule();
    void setup() override;
    void setup1();
    void loop() override;
    void loop1();
    const std::string name() override;
    const std::string version() override;
    void processInputKo(GroupObject &ko) override;
    void showHelp() override;
    bool processCommand(const std::string cmd, bool diagnoseKo);

    void processBeforeRestart();
    void savePower();                     // Trip: Ausgänge aus + Latch setzen
    void clearFaultAndResend();           // Latch lösen und Kanäle aus Software-Zustand neu schreiben
    void requestReactivation();           // von Core 0 (KO/Konsole): Reaktivierung anfordern
    bool consumeReactivationRequest();    // von Core 1 (loop1): Anforderung abholen
    bool isPowerFault();

    // Testmodus (Anforderung von Core 0/Display; Ausführung in loop1 auf Core 1)
    void testStart(bool autoAdvance);     // Kanal-Durchlauf starten
    void testStop();
    void testNext();
    void testPrev();
    void testGoTo(uint8_t port);          // Einzeltest eines HW-Ports (startet den Test)
    bool isTestActive();
    uint8_t testPort();                   // aktuell getesteter HW-Port
    float testCurrent();                  // zuletzt gemessener Strom (A), -1 = ungültig

    bool getPcaI2cConnectionState();
    uint8_t getUsedChannels();
    const std::string getChannelName(uint8_t channelIndex);
    uint8_t getChannelHWPort(uint8_t channelIndex, uint8_t *ports);
    void toggleChannelHWPort(uint8_t channelIndex);
    uint8_t getChannelIndex(uint8_t channelIndex);

    static LEDModule *instance();
    static LEDModule *_instance;

    void setHwChannelValue(byte channel, byte value, int curve);
    void setHwChannelValuePWM(byte channel, word start, word end, int curve);
    bool processFunctionProperty(uint8_t objectIndex, uint8_t propertyId, uint8_t length, uint8_t *data, uint8_t *resultData, uint8_t &resultLength) override;

private:
    int8_t deviceSelect = 0;       // 0=LED-DK-06x24V, 1=LED-DK-12x24V
    int8_t operatinModeSelect = 0; // based on deviceSelect
    int16_t pwmFreqSelect = 488;   // 211, 488, 600, 832, 1000, 1200
    bool useDiagnoseKo = 0;
    unsigned long currentTime = 0;
    int8_t usedChannels = 0;
    uint32_t _timerCheckI2cConnection = 0;
    uint32_t _timerCheckHclChannel = 0;
    bool doResetI2c = false;
    volatile bool _powerFault = false;            // gesetzt bei Hardware-Alarm (Überstrom u.Ä.), sperrt das Neubestromen der Kanäle; cross-core (Core 0 liest, Core 1 schreibt)
    volatile bool _reactivationRequested = false; // Core 0 fordert Reaktivierung an, Core 1 (loop1) führt sie aus

    // Testmodus-Zustand (Ausführung in loop1/Core 1)
    volatile bool _testActive = false;   // cross-core: Core 1 schreibt (testLoop), Core 0 liest (processCommand)
    bool _testAuto = false;
    uint8_t _testPort = 0;               // nur Core 1 (testLoop schreibt, showTestScreen liest)
    uint8_t _testPhase = 0;              // 0 = settle (auf Strom warten), 1 = gemessen/anzeigen
    uint32_t _testStepStart = 0;
    uint32_t _testLastActivity = 0;
    float _testCurrentA = -1.0f;         // nur Core 1 (testLoop schreibt, showTestScreen liest)
    volatile uint8_t _testReq = 0;       // 0=keine,1=start,2=start-auto,3=next,4=prev,5=stop,6=goto
    volatile int16_t _testReqPort = -1;
    void testLoop();                     // Zustandsmaschine, aus loop1
    void testEnterPort(uint8_t port);    // alle aus, Port an (100%), Messphase starten
    void allPortsOff();                  // alle PWM-Ausgaenge per ALL_LED_OFF-Broadcast (eine I2C-Transaktion) aus
    void resendChannels();               // alle Kanäle aus Software-Zustand neu ausgeben

    // Cross-core Diagnose-KO-Ausgabe: writeDiagenoseKo() ruft intern knx.loop() und darf nur auf Core 0
    // laufen. loop1/Core-1-Code (Testmodus-Ergebnis, I2C-Reconnect) postet hierhin; loop()/Core 0 gibt es aus.
    void postDiagCore1(const char *fmt, ...);
    volatile bool _diagCore1Pending = false;
    char _diagCore1Buf[16] = {};

    // hcl channels
    uint8_t hclBrightness = 0;
    uint16_t hclKelvin = 0;

    // Status flags for I2C connection
    bool pcaI2cConnection = false;

    void koHandleDayNight(GroupObject &ko);
    bool initI2cConnection();
    bool checkI2cConnection();
#ifdef FUNC1_BUTTON_PIN
    void handleFunc1(uint8_t setting);
    bool _currentToggleState = false;
#endif
    void handleFunctionPropertySwitch(uint8_t *data, uint8_t *resultData, uint8_t &resultLength);

    byte readRegister(byte registerAddress);

    Adafruit_PWMServoDriver _pwm;

    DimChannel *channel[MAXCHANNELSHW];
    DimChannel *channelEK[MAXCHANNELSHW];
    DimChannel *channelTW[MAXCHANNELSHW];
    DimChannel *channelRGB[MAXCHANNELSHW];
    HclChannel *hclchannel[MAXCHANNELSHCL];
    HWChannel *hwchannels[MAXCHANNELSHW];
};

extern LEDModule openknxLEDModule;

#endif