// ============================================================================
// Simulator: Hauptprogramm des Kerns (ledsim_<board>.exe).
//
// Liest Befehle zeilenweise von stdin, schreibt Ereignisse als JSON-Zeilen nach
// stdout. Jeder Befehl endet mit {"ev":"ok"}. Steuerung durch die Python-App
// oder direkt mit einer Szenario-Datei:   ledsim_DK_12_V12.exe < szenario.txt
//
// Ablauf wie src/main.cpp: Kern 0 ruft loop() aller Module (OpenKNX Common),
// Kern 1 ruft LEDModule/MeasuringModule/FrontPanelModule::loop1(). Im
// Simulator laufen beide abwechselnd in Schritten von 1 ms.
//
// Befehle:
//   load <hex>           Parameterspeicher setzen, als waere das Geraet schon
//                        programmiert (ohne Download, ohne Neustart)
//   pimg <hex>           Parameterspeicher fuer den naechsten ETS-Download
//   ets [ms]             ETS-Download: knx.configured() faellt fuer [ms] (Vorgabe
//                        3000) auf false, dann processBeforeRestart, Flash
//                        sichern, neue Parameter, Neustart
//   unload               Applikation entladen (Geraet wie neu), Neustart
//   restart              Neustart (processBeforeRestart, Flash, setup)
//   bus 0|1              KNX-Bus aus/ein. Aus: SAVE (savePower, Flash), Prozessor
//                        und Bausteine aus. Ein: Bausteine im Einschaltzustand, Neustart
//   busdip <ms>          kurzer Busspannungseinbruch: SAVE, <ms> warten,
//                        restorePower() ohne Neustart (wie OpenKNX Common)
//   supply 0|1           LED-Versorgung (24 V) aus/ein
//   ko <nr> <dpt> <wert> Telegramm vom Bus an Objekt <nr>, z. B. ko 57 1.1 1
//   btn short|long|double  Funktionstaste
//   panel <0..3>         Taste am Bedienfeld (0 Zurueck, 1 links, 2 rechts, 3 Auswahl)
//   fp <obj> <prop> <hex>  Function Property (ETS-Funktionstest), Antwort als Ereignis
//   run <ms>             Zeit fortschreiben
//   stepms <ms>          Schrittweite (Vorgabe 1 ms)
//   cmd <text>           Konsolenbefehl, z. B. cmd show con
//   diag <text>          Befehl ueber das Diagnose-KO
//   portw <port> <W>     Last an Port (0 = A) bei 100 %
//   short <port> 0|1     Kurzschluss an Port
//   supplyv <V>          Leerlaufspannung der LED-Versorgung
//   shunt <ohm>          tatsaechlich bestueckter Messwiderstand
//   ambient <C>          Umgebungstemperatur
//   chip pca|ina|tmp|oled 0|1  Baustein vorhanden / ausgefallen
//   time <sekunden>      lokale Uhrzeit beim Start der Simulationszeit (Unix-Sekunden)
//   timevalid 0|1        Uhrzeit vom Bus vorhanden
//   pa <adresse>         physikalische Adresse fuer die Anzeige
//   state                Zustand als JSON
// ============================================================================

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <new>
#include <sstream>
#include <string>

#include "sim.h"

#include "FrontPanelModule.h"
#include "LEDModule.h"
#include "MeasuringModule.h"

namespace
{
    std::vector<uint8_t> pending;
    uint32_t stepMs = 1;
    bool savedPower = false; ///< SAVE ausgeloest, Prozessor laeuft noch

    // Reihenfolge und Nummern wie src/main.cpp (FileTransfer laeuft nicht mit)
    OpenKNX::Module *modules[] = {&openknxLEDModule, &openknxMeasuringModule, &openknxFrontPanelModule};
    constexpr int kModules = 3;

    void event(const std::string &name, const std::string &extra = "")
    {
        sim::emit("{\"ev\":\"" + name + "\",\"t\":" + std::to_string(sim::now) + (extra.empty() ? "" : "," + extra) + "}");
    }

    bool parseHex(const std::string &hex, std::vector<uint8_t> &out)
    {
        if (hex.size() % 2 != 0)
            return false;
        out.assign(hex.size() / 2, 0);
        for (size_t i = 0; i < out.size(); i++)
            out[i] = static_cast<uint8_t>(std::stoul(hex.substr(i * 2, 2), nullptr, 16));
        return true;
    }

    std::string toHex(const uint8_t *d, size_t n)
    {
        static const char *hx = "0123456789abcdef";
        std::string s;
        for (size_t i = 0; i < n; i++)
        {
            s += hx[d[i] >> 4];
            s += hx[d[i] & 0x0F];
        }
        return s;
    }

    // ------------------------------------------------------------ Flash

    void flashSave()
    {
        sim::flashBlocks.assign(kModules, {});
        for (int i = 0; i < kModules; i++)
        {
            openknx.flash.writeTarget = &sim::flashBlocks[i];
            modules[i]->writeFlash();
            sim::flashBlocks[i].resize(modules[i]->flashSize(), 0);
        }
        openknx.flash.writeTarget = nullptr;
        sim::flashValid = true;
        sim::flashSaveRequested = false;
        event("flash", "\"action\":\"save\"");
    }

    void flashLoad()
    {
        for (int i = 0; i < kModules; i++)
        {
            const uint16_t size = modules[i]->flashSize();
            if (sim::flashValid && i < static_cast<int>(sim::flashBlocks.size()) && size > 0 &&
                sim::flashBlocks[i].size() == size)
            {
                openknx.flash.readPtr = sim::flashBlocks[i].data();
                openknx.flash.readEnd = sim::flashBlocks[i].data() + size;
                modules[i]->readFlash(sim::flashBlocks[i].data(), size);
            }
            else
            {
                openknx.flash.readPtr = nullptr;
                modules[i]->readFlash(nullptr, 0);
            }
        }
        openknx.flash.readPtr = nullptr;
    }

    // ------------------------------------------------------------ Prozessor

    /// Kaltstart des Prozessors: RAM weg; Bausteine, Parameter und Flash bleiben.
    void boot()
    {
        openknxLEDModule.~LEDModule();
        new (&openknxLEDModule) LEDModule();
        openknxMeasuringModule.~MeasuringModule();
        new (&openknxMeasuringModule) MeasuringModule();
        openknxFrontPanelModule.~FrontPanelModule();
        new (&openknxFrontPanelModule) FrontPanelModule();

        for (size_t i = 0; i < sim::kos.size(); i++)
            sim::kos[i].simInit(static_cast<uint16_t>(i));
        openknx.leds.simReset();
        openknx.func1Button = OpenKNX::SimButton{};

        sim::bootAt = sim::now;
        sim::mcuRunning = true;
        sim::downloading = false;
        savedPower = false;
        sim::resetInvariants();

        const bool cfg = sim::configured();
        event("boot", std::string("\"configured\":") + (cfg ? "true" : "false"));

        // Wie OpenKNX Common::setup(): setup() aller Module, dann den Flash lesen;
        // danach setup1() auf Kern 1.
        for (auto *m : modules)
            m->setup(cfg);
        if (cfg)
            flashLoad();
        for (auto *m : modules)
            m->setup1(cfg);
    }

    void loopOnce()
    {
        const bool cfg = sim::configured();
        // Kern 0: OpenKNX Common::loop()
        for (auto *m : modules)
            m->loop(cfg);
        // Kern 1: src/main.cpp loop1()
        openknxLEDModule.loop1();
        openknxMeasuringModule.loop1();
        openknxFrontPanelModule.loop1();
    }

    // ------------------------------------------------------------ Ausgaenge melden

    double lastDuty[sim::kPorts];
    bool dutyPending = false;
    uint64_t lastDutyEmit = 0;
    uint64_t lastDispEmit = 0;

    void emitDuty(bool force)
    {
        bool changed = false;
        double d[sim::kPorts];
        for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
        {
            d[p] = sim::busOn ? sim::portDuty(p) : 0.0;
            if (std::fabs(d[p] - lastDuty[p]) > 1e-6)
                changed = true;
        }
        if (changed)
            dutyPending = true;
        if (!dutyPending || (!force && sim::now - lastDutyEmit < 10))
            return;
        std::string o = "{\"ev\":\"pwm\",\"t\":" + std::to_string(sim::now) + ",\"d\":[";
        for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
        {
            lastDuty[p] = d[p];
            o += (p ? "," : "") + sim::num(d[p], 4);
        }
        sim::emit(o + "]}");
        dutyPending = false;
        lastDutyEmit = sim::now;
    }

    void emitDisplay(bool force)
    {
        if (!sim::oledChanged || (!force && sim::now - lastDispEmit < 100))
            return;
        sim::oledChanged = false;
        lastDispEmit = sim::now;
        event("disp", std::string("\"on\":") + (sim::oledOn ? "true" : "false") + ",\"hex\":\"" + sim::oledHex() + "\"");
    }

    void run(uint64_t ms)
    {
        const uint64_t end = sim::now + ms;
        while (sim::now < end)
        {
            const uint64_t step = std::min<uint64_t>(stepMs, end - sim::now);
            sim::now += step;
            if (sim::busOn || savedPower)
                sim::hwStep(static_cast<uint32_t>(step));
            if (sim::mcuRunning)
                loopOnce();
            if (sim::flashSaveRequested && sim::mcuRunning)
                flashSave();
            sim::checkInvariants();
            emitDuty(false);
            emitDisplay(false);
        }
        emitDuty(true);
        emitDisplay(true);
    }

    void beforeRestart()
    {
        if (!sim::mcuRunning)
            return;
        for (auto *m : modules)
            m->processBeforeRestart();
        flashSave();
    }

    void saveInterrupt()
    {
        // SAVE-Interrupt: nur bei konfiguriertem Geraet (Common::loop)
        if (sim::mcuRunning && sim::configured())
        {
            for (auto *m : modules)
                m->savePower();
            flashSave();
        }
    }

    void setBus(bool on)
    {
        if (on == sim::busOn)
            return;
        sim::busOn = on;
        sim::chipsPowered = on;
        if (!on)
        {
            saveInterrupt();
            sim::mcuRunning = false;
            event("mcu", "\"running\":false");
        }
        else
        {
            sim::hwPowerOn();
            boot();
        }
    }

    void busDip(uint64_t ms)
    {
        if (!sim::busOn || !sim::mcuRunning)
        {
            event("error", "\"text\":\"Busspannungseinbruch nur bei laufendem Geraet\"");
            return;
        }
        event("busdip", "\"phase\":\"start\",\"ms\":" + std::to_string(ms));
        saveInterrupt();
        savedPower = true;
        sim::busOn = false;
        run(ms);
        sim::busOn = true;
        savedPower = false;
        // Common::processRestorePower: jedes Modul; liefert eines false, folgt ein Neustart
        bool reboot = false;
        for (auto *m : modules)
            if (sim::configured() && !m->restorePower())
                reboot = true;
        event("busdip", std::string("\"phase\":\"end\",\"reboot\":") + (reboot ? "true" : "false"));
        if (reboot)
        {
            beforeRestart();
            boot();
        }
    }

    // ------------------------------------------------------------ Befehle

    std::string rest(std::istringstream &in)
    {
        std::string r;
        std::getline(in, r);
        const size_t p = r.find_first_not_of(' ');
        return p == std::string::npos ? "" : r.substr(p);
    }

    void receiveKo(int nr, const std::string &dptText, const std::string &valueText)
    {
        if (nr < 0 || nr >= static_cast<int>(sim::kos.size()))
        {
            event("error", "\"text\":" + sim::esc("unbekanntes Objekt " + std::to_string(nr)));
            return;
        }
        const Dpt dpt = sim::parseDpt(dptText);
        GroupObject &ko = sim::kos[nr];
        bool ok;
        if (dpt.mainGroup == 1)
        {
            const bool v = valueText == "1" || valueText == "true" || valueText == "ein";
            ok = ko.simReceive(KNXValue(v), dpt);
        }
        else if (dpt.mainGroup == 16)
            ok = ko.simReceive(KNXValue(valueText.c_str()), dpt);
        else if (dpt.mainGroup == 3)
        {
            // DPT 3.007 als Rohwert 0..15: Bit 3 = Richtung (1 heller), Bits 0..2 = Schritt (0 = Stopp)
            const int raw = std::stoi(valueText) & 0x0F;
            ok = ko.simReceive(KNXValue(static_cast<uint8_t>(raw)), Dpt(5, 10));
        }
        else if (dpt.mainGroup == 232)
        {
            // DPT 232.600 als Zahl 0xRRGGBB
            const uint32_t rgb = static_cast<uint32_t>(std::stoul(valueText, nullptr, 0));
            ok = ko.simReceive(KNXValue(rgb), dpt);
        }
        else
            ok = ko.simReceive(KNXValue(std::stod(valueText)), dpt);
        if (!ok)
        {
            event("error", "\"text\":" + sim::esc("Wert " + valueText + " passt nicht zu DPT " + dptText +
                                                   " - Objekt " + std::to_string(nr)));
            return;
        }

        const bool delivered = sim::mcuRunning && sim::configured();
        event("rx", "\"nr\":" + std::to_string(nr) + ",\"dpt\":\"" + dptText + "\",\"val\":" + sim::koValueJson(ko, dpt) +
                        ",\"raw\":\"" + sim::rawHex(ko, dpt) + "\",\"delivered\":" + (delivered ? "true" : "false"));
        if (delivered)
            for (auto *m : modules)
                m->processInputKo(ko);
    }

    void console(const std::string &text, bool debugKo)
    {
        if (!sim::mcuRunning)
        {
            event("error", "\"text\":\"Prozessor laeuft nicht (kein KNX-Bus)\"");
            return;
        }
        if (text == "help" || text == "?")
        {
            for (auto *m : modules)
                m->showHelp();
            return;
        }
        for (auto *m : modules)
            if (m->processCommand(text, debugKo))
                return;
        event("error", "\"text\":" + sim::esc("unbekannter Befehl: " + text));
    }

    void functionProperty(int obj, int prop, const std::string &hex)
    {
        std::vector<uint8_t> data;
        if (!parseHex(hex, data))
        {
            event("error", "\"text\":\"Function Property: Daten sind kein Hex\"");
            return;
        }
        if (!sim::mcuRunning)
        {
            event("fp", "\"handled\":false,\"res\":\"\",\"text\":\"Geraet antwortet nicht (kein KNX-Bus)\"");
            return;
        }
        uint8_t res[64] = {};
        uint8_t resLen = 0;
        for (auto *m : modules)
            if (m->processFunctionProperty(static_cast<uint8_t>(obj), static_cast<uint8_t>(prop),
                                           static_cast<uint8_t>(data.size()), data.data(), res, resLen))
            {
                event("fp", "\"handled\":true,\"res\":\"" + toHex(res, resLen) + "\"");
                return;
            }
        event("fp", "\"handled\":false,\"res\":\"\"");
    }

    void state()
    {
        std::string flags = std::string("\"uptime\":") + std::to_string(millis()) +
                            ",\"mcu\":" + (sim::mcuRunning ? "true" : "false") +
                            ",\"bus\":" + (sim::busOn ? "true" : "false") +
                            ",\"supply\":" + (sim::load.supplyOn ? "true" : "false") +
                            ",\"paramsValid\":" + (sim::paramsValid ? "true" : "false") +
                            ",\"downloading\":" + (sim::downloading ? "true" : "false") +
                            ",\"configured\":" + (sim::configured() ? "true" : "false");
        sim::emit("{\"ev\":\"state\",\"t\":" + std::to_string(sim::now) + "," + flags + ",\"sim\":" + sim::snapshotJson() + "}");
    }

    void handle(const std::string &line)
    {
        std::istringstream in(line);
        std::string op;
        in >> op;

        if (op.empty() || op[0] == '#')
            return;
        if (op == "load" || op == "pimg")
        {
            std::string hex;
            in >> hex;
            std::vector<uint8_t> img;
            if (!parseHex(hex, img))
            {
                event("error", "\"text\":\"Parameterabbild ist kein Hex\"");
                return;
            }
            img.resize(MAIN_ParameterSize, 0);
            if (op == "load")
            {
                sim::params = img;
                sim::paramsValid = true;
            }
            pending = img;
        }
        else if (op == "ets")
        {
            uint64_t ms = 3000;
            in >> ms;
            if (!sim::mcuRunning)
            {
                event("error", "\"text\":\"ETS-Download ohne KNX-Bus nicht moeglich\"");
                return;
            }
            if (pending.empty())
                pending = sim::params;
            sim::downloading = true;
            event("download", "\"phase\":\"start\"");
            run(ms);
            beforeRestart();
            sim::params = pending;
            sim::paramsValid = true;
            event("download", "\"phase\":\"done\"");
            boot();
        }
        else if (op == "unload")
        {
            beforeRestart();
            sim::paramsValid = false;
            if (sim::mcuRunning)
                boot();
        }
        else if (op == "restart")
        {
            if (sim::mcuRunning)
            {
                beforeRestart();
                boot();
            }
        }
        else if (op == "bus")
        {
            int v = 0;
            in >> v;
            setBus(v != 0);
        }
        else if (op == "busdip")
        {
            uint64_t ms = 200;
            in >> ms;
            busDip(ms);
        }
        else if (op == "supply")
        {
            int v = 0;
            in >> v;
            sim::load.supplyOn = v != 0;
            event("supply", std::string("\"on\":") + (v ? "true" : "false"));
        }
        else if (op == "ko")
        {
            int nr = -1;
            std::string dpt;
            in >> nr >> dpt;
            receiveKo(nr, dpt, rest(in));
        }
        else if (op == "btn")
        {
            std::string kind;
            in >> kind;
            auto &b = openknx.func1Button;
            std::function<void()> cb = kind == "short" ? b.shortCb : (kind == "long" ? b.longCb : b.doubleCb);
            if (sim::mcuRunning && cb)
                cb();
            else if (!cb)
                event("error", "\"text\":\"Funktionstaste: keine Aktion (Geraet ohne Taste oder nicht parametriert)\"");
        }
        else if (op == "panel")
        {
            int b = 0;
            in >> b;
            sim::panelPress(b, 60);
        }
        else if (op == "fp")
        {
            int obj = 0, prop = 0;
            std::string hex;
            in >> obj >> prop >> hex;
            functionProperty(obj, prop, hex);
        }
        else if (op == "run")
        {
            uint64_t ms = 0;
            in >> ms;
            run(ms);
        }
        else if (op == "stepms")
        {
            in >> stepMs;
            if (stepMs == 0)
                stepMs = 1;
        }
        else if (op == "cmd")
            console(rest(in), false);
        else if (op == "diag")
            console(rest(in), true);
        else if (op == "portw")
        {
            int p = 0;
            double w = 0;
            in >> p >> w;
            if (p >= 0 && p < sim::kPorts)
                sim::load.portW[p] = std::max(0.0, w);
        }
        else if (op == "short")
        {
            int p = 0, v = 0;
            in >> p >> v;
            if (p >= 0 && p < sim::kPorts)
                sim::load.shortPort[p] = v != 0;
        }
        else if (op == "supplyv")
            in >> sim::load.supplyV;
        else if (op == "shunt")
            in >> sim::shuntOhm;
        else if (op == "ambient")
            in >> sim::thermal.ambientC;
        else if (op == "chip")
        {
            std::string which;
            int v = 1;
            in >> which >> v;
            bool *c = which == "pca" ? &sim::chips.pca : which == "ina" ? &sim::chips.ina
                                                         : which == "tmp" ? &sim::chips.tmp
                                                         : which == "oled" ? &sim::chips.oled : nullptr;
            if (c)
                *c = v != 0;
        }
        else if (op == "time")
        {
            long long t = 0;
            in >> t;
            sim::timeStartEpoch = t - static_cast<long long>(sim::now / 1000);
        }
        else if (op == "timevalid")
        {
            int v = 1;
            in >> v;
            sim::timeValid = v != 0;
        }
        else if (op == "pa")
            in >> sim::individualAddress;
        else if (op == "state")
            state();
        else if (op == "quit")
            std::exit(0);
        else
            event("error", "\"text\":" + sim::esc("unbekannter Befehl: " + op));
    }
} // namespace

int main()
{
    std::ios::sync_with_stdio(false);
    static char outbuf[1 << 16];
    std::setvbuf(stdout, outbuf, _IOFBF, sizeof(outbuf));

    for (auto &d : lastDuty)
        d = -1;
    for (size_t i = 0; i < sim::kos.size(); i++)
        sim::kos[i].simInit(static_cast<uint16_t>(i));
    event("hello", "\"paramSize\":" + std::to_string(MAIN_ParameterSize) + ",\"maxKo\":" + std::to_string(MAIN_MaxKoNumber) +
                       ",\"ports\":" + std::to_string(LED_HW_CHANNEL_COUNT) + ",\"device\":" + sim::esc(DEVICE_ID) +
#ifdef FRONT_PANEL_PRESENT
                       ",\"panel\":true"
#else
                       ",\"panel\":false"
#endif
#ifdef FUNC1_BUTTON_PIN
                       ",\"func1\":true"
#else
                       ",\"func1\":false"
#endif
    );
    std::fflush(stdout);

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        try
        {
            handle(line);
        }
        catch (const std::exception &e)
        {
            event("error", "\"text\":" + sim::esc(std::string("Ausnahme: ") + e.what()));
        }
        sim::emit("{\"ev\":\"ok\"}");
        std::fflush(stdout);
    }
    return 0;
}
