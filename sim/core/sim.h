#pragma once
// ============================================================================
// Simulator: gemeinsamer Zustand von Laufzeit, Hardware-Modellen und
// Hauptprogramm.
//
// Getrennt wie im Geraet:
//   - der Prozessor (RP2040) mit der Firmware - laeuft nur mit KNX-Bus,
//   - die Bausteine am I2C (PCA9685, INA226, TMP100, SSD1306) - laufen mit der
//     Geraeteversorgung und behalten ihre Register ueber einen Neustart,
//   - die LED-Versorgung (24 V) mit den angeschlossenen Lasten,
//   - Parameterspeicher (ETS) und Flash - ueberleben einen Neustart.
// ============================================================================

#include <cstdint>
#include <string>
#include <vector>

#include "OpenKNX.h"

namespace sim
{
    // --- Zeit ----------------------------------------------------------------
    extern uint64_t now;    ///< Simulationszeit in ms seit Programmstart
    extern uint64_t bootAt; ///< Zeitpunkt des letzten Prozessorstarts

    // --- Geraetezustand -------------------------------------------------------
    extern std::vector<uint8_t> params; ///< wirksamer Parameterspeicher
    extern bool paramsValid;            ///< die ETS hat das Geraet programmiert
    extern bool downloading;            ///< ETS-Download laeuft gerade
    extern bool mcuRunning;             ///< Prozessor laeuft (KNX-Bus da)
    extern bool busOn;                  ///< KNX-Busspannung liegt an
    bool configured();                  ///< knx.configured()

    extern std::vector<GroupObject> kos;

    // --- Flash, je Modul ein Block --------------------------------------------
    extern std::vector<std::vector<uint8_t>> flashBlocks;
    extern bool flashValid;
    extern bool flashSaveRequested; ///< openknx.flash.save() aus der Firmware

    // --- Hardware ------------------------------------------------------------
    constexpr int kPorts = 16; ///< PCA9685 hat 16 Ausgaenge, das Board nutzt 6 bzw. 12

    struct Load
    {
        double supplyV = 24.0;       ///< LED-Versorgung im Leerlauf
        bool supplyOn = true;        ///< LED-Versorgung eingeschaltet
        double sourceOhm = 0.05;     ///< Innenwiderstand von Netzteil und Leitungen
        double idleA = 0.015;        ///< Grundlast auf der LED-Seite
        double portW[kPorts] = {};   ///< Leistung je Port bei 100 % (Nennspannung)
        bool shortPort[kPorts] = {}; ///< Kurzschluss: der Port zieht beim Einschalten 10 A
    };
    extern Load load;

    struct Thermal
    {
        double ambientC = 25.0; ///< Umgebung
        double kPerW = 0.25;    ///< Erwaermung des Geraets je Watt
        double tauS = 120.0;    ///< Zeitkonstante
        double tempC = 25.0;    ///< aktuelle Geraetetemperatur
    };
    extern Thermal thermal;

    /// Bestueckung und Ausfaelle der I2C-Bausteine.
    struct Chips
    {
        bool pca = true;
        bool ina = true;
        bool tmp = true;
        bool oled = true;
    };
    extern Chips chips;
    /// Versorgung der Bausteine: mit dem KNX-Bus, beim kurzen Einbruch aus Kondensatoren.
    extern bool chipsPowered;
    extern double shuntOhm; ///< tatsaechlich bestueckter Messwiderstand
    extern uint64_t i2cFrames;
    extern uint64_t i2cNacks;

    /// Tastverhaeltnis eines PCA9685-Ausgangs 0...1, so wie er am Pin liegt.
    double portDuty(int port);
    /// PWM-Frequenz aus dem Vorteiler.
    double pwmFrequency();
    /// Registerwerte eines Ausgangs (fuer die Anzeige).
    void portRegs(int port, uint16_t &on, uint16_t &off, bool &fullOn, bool &fullOff);
    bool pcaSleeping();
    /// Strom aus der LED-Versorgung jetzt (A).
    double loadCurrent();
    double busVoltage();
    /// Hardware einen Schritt weiterrechnen (Messwandler, Temperatur).
    void hwStep(uint32_t ms);
    /// Geraeteversorgung aus/ein: Bausteine im Einschaltzustand.
    void hwPowerOn();
    /// Gemittelter Strom und Spannung, wie sie der INA zuletzt gewandelt hat.
    double inaCurrent();
    double inaVoltage();

    // Bedienfeld
    extern bool oledOn;
    extern bool oledChanged;
    std::string oledHex(); ///< 128x32 Bildpunkte, 512 Byte, Seite fuer Seite
    void panelPress(int button, uint32_t holdMs);

    // Uhrzeit (OpenKNX::time): Wanduhr = Startzeit + Simulationszeit
    extern bool timeValid;
    extern int64_t timeStartEpoch; ///< lokale Zeit beim Start der Simulationszeit, Sekunden
    extern std::string individualAddress;

    // --- Ausgabe ------------------------------------------------------------
    void emit(const std::string &json);
    std::string esc(const std::string &s);
    std::string num(double v, int decimals = 3);
    std::string koValueJson(GroupObject &ko, const Dpt &type);
    std::string rawHex(GroupObject &ko, const Dpt &type);
    Dpt parseDpt(const std::string &s);
    Dpt fallbackDpt(const Dpt &type);
    std::string dptText(const Dpt &t);

    /// Inspektion (sim_inspect.cpp): Zustand als JSON.
    std::string snapshotJson();
    /// Waechter, nach jedem Durchlauf.
    void checkInvariants();
    void resetInvariants();
    /// Meldung an den Waechter, gleiche Meldung gedrosselt.
    void warn(const std::string &key, const std::string &inv, const std::string &text);
} // namespace sim
