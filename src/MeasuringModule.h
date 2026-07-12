#ifndef MEASURINGMODULE_H
#define MEASURINGMODULE_H

#include "LIB_TEMP100.h"
#include "LIB_INA.h"
#include <OpenKNX.h>
#include "hardware.h"
#include "LEDModule.h"

#include "LEDHelper.h"


#define OVER_CURRENT    6.0 // 6A bei 24V = 144W

// Timing parameters
#define MEASUREMENT_ALARM_CHECK     5000    // ms
#define MEASUREMENT_CHECK_I2C       10000   // ms
#define MEASUREMENT_INA_OVERFLOW    6000    // ms
#define REACTIVATE_COOLDOWN         3000    // ms - Sperrzeit nach einem Trip, bevor eine Reaktivierung zulässig ist
#define REACTIVATE_VERIFY_DELAY     400     // ms - nach Wiedereinschalten den echten Strom prüfen

class MeasuringModule : public OpenKNX::Module
{
public:
    MeasuringModule();
    void setup() override;
    void setup1();
    void loop() override;
    void loop1();
    const std::string name() override;
    const std::string version() override;
    
    void readFlash(const uint8_t *buffer, const uint16_t size) override;
    void writeFlash() override;
    uint16_t flashSize() override;

    void sendSingleMeasurement();
    void showHelp() override;
    bool processCommand(const std::string cmd, bool diagnoseKo);

    float getMeasurementValue(const std::string &parameter);
    float readCurrentNow(); // frische Strommessung für den Testmodus (-1 wenn INA nicht verbunden)

    bool getTempI2cConnectionState();
    bool getInaI2cConnectionState();

    static MeasuringModule *instance();
    static MeasuringModule *_instance;

private:
    float shuntValue = 0;                   // value will divided by 1000
    float maxcurrent = 0;
    bool measurementSend = true;            // 0=No, 1=Yes
    bool tempSensorPresent = false;         // 0=No, 1=Yes
    uint32_t measurementInterval = 60000;   
    uint32_t _lastMeasurementSend = 0;
    uint32_t _lastMeasurementGet = 0;

    bool checkTemp = false;
    float overTemp = 0;
    bool checkVoltage = false;
    float overVoltage = 0;
    float underVoltage = 0;
    bool checkCurrent = false;
    float overCurrent = 0;

    // Status flags for alarms
    bool overTempTriggered = false;
    bool overVoltageTriggered = false;
    bool underVoltageTriggered = false;
    bool overCurrentTriggered = false;

    // Status flags for I2C connection
    bool tempI2cConnection = false;
    bool inaI2cConnection = false;

    float elapsedTime_s = 0;
    unsigned long currentTime = 0;
    unsigned long lastUpdateTime = 0;

    float busVoltage_V = 0.0;
    float current_A = 0.0;
    float power_W = 0.0;
    double totalEnergy_Wh = 0.0; // double: float-Mantisse friert bei hohen kWh-Summen ein
    double lastEnergy_Wh = 0.0;
    
    float temperatur_C = 0.0;

    uint32_t _timerCheckOverflow = 0;
    uint32_t _timerCheckI2cConnection = 0;
    bool doResetI2cTemp = false;
    bool doResetI2cIna = false;

    // Über-strom-Latch / Reaktivierung
    uint32_t _lastTripTime = 0;   // Zeitpunkt des letzten Trips (für Cooldown)
    bool _verifyActive = false;   // ein Reaktivierungsversuch läuft, Strom wird gleich geprüft
    uint32_t _verifyStart = 0;    // Startzeitpunkt des Verify-Fensters
    void triggerFault();          // Ausgänge abschalten/latchen und Trip-Zeit merken
    void handleReactivation();    // Reaktivierungs-Anforderung + Strom-Verify (läuft in loop1)

    void getSingleMeasurement();
    bool initI2cConnectionTemp();
    bool checkI2cConnectionTemp();
    bool initI2cConnectionIna();
    bool checkI2cConnectionIna();
    void getAlertValues();
    void checkAlarmDefinitions();
    void checkAndTriggerAlarm(bool condition, bool &triggeredFlag, uint16_t alarmKo, const String &messageDiagnoseKo);


    INASensor _ina;
    TMP100_WE _tmp100;
};

extern MeasuringModule openknxMeasuringModule;

#endif