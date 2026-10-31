// ============================================================================
// Simulator: Zustand fuer die Anzeige und Waechter.
//
// Liest die privaten Felder der Module fuer die Anzeige. Dafuer werden sie in
// DIESER Uebersetzungseinheit per #define geoeffnet - ein Kniff fuer
// Testwerkzeuge, der Firmware-Code bleibt unveraendert.
// ============================================================================

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "sim.h"

#define private public
#define protected public
#include "DimChannel_EK.h"
#include "DimChannel_RGB.h"
#include "DimChannel_TW.h"
#include "FrontPanelModule.h"
#include "HclChannel.h"
#include "LEDModule.h"
#include "MeasuringModule.h"
#undef private
#undef protected

namespace sim
{
    namespace
    {
        std::string b(bool v) { return v ? "true" : "false"; }

        std::string ledJson(OpenKNX::Led::Base *l)
        {
            return "[" + std::to_string(l->mode()) + "," + std::to_string(l->freq()) + "]";
        }

        std::string channelJson(DimChannel *c)
        {
            uint8_t ports[3] = {0, 0, 0};
            const uint8_t n = c->getHWPorts(ports);
            std::string o = "{\"type\":" + esc(c->name()) + ",\"no\":" + std::to_string(c->getChannelIndex() + 1) + ",\"ports\":[";
            for (uint8_t i = 0; i < n; i++)
                o += (i ? "," : "") + std::to_string(ports[i]);
            o += "]";
            switch (c->getChannelType())
            {
                case ChannelType::EK:
                {
                    auto *e = static_cast<DimChannel_EK *>(c);
                    o += ",\"on\":" + b(e->_isOn) + ",\"b\":" + std::to_string(e->_currentValueEK) +
                         ",\"night\":" + b(e->isNight) + ",\"task\":" + std::to_string(e->_currentTask) +
                         ",\"curve\":" + std::to_string(e->m_curve) + ",\"hcl\":" + b(e->_hclValid);
                    break;
                }
                case ChannelType::TW:
                {
                    auto *t = static_cast<DimChannel_TW *>(c);
                    o += ",\"on\":" + b(t->_isOn) + ",\"b\":" + std::to_string(t->_currentValueTW[0]) +
                         ",\"k\":" + std::to_string(t->_currentValueTW[1]) + ",\"ww\":" + std::to_string(t->m_colortempww) +
                         ",\"kw\":" + std::to_string(t->m_colortempcw) + ",\"night\":" + b(t->isNight) +
                         ",\"task\":" + std::to_string(t->_currentTask) + ",\"curve\":" + std::to_string(t->m_curve) +
                         ",\"hcl\":" + b(t->_hclValid);
                    break;
                }
                case ChannelType::RGB:
                {
                    auto *r = static_cast<DimChannel_RGB *>(c);
                    o += ",\"on\":" + b(r->_isOn) + ",\"rgb\":[" + std::to_string(r->_currentValueRGB[0]) + "," +
                         std::to_string(r->_currentValueRGB[1]) + "," + std::to_string(r->_currentValueRGB[2]) +
                         "],\"night\":" + b(r->isNight) + ",\"task\":" + std::to_string(r->_currentTask) +
                         ",\"curve\":" + std::to_string(r->m_curve) + ",\"gamma\":" + num(r->m_gammacorrection, 2) +
                         ",\"hcl\":" + b(r->_hclValid);
                    break;
                }
                default:
                    break;
            }
            return o + "}";
        }

        std::string hhmm(const OpenKNX::TimeOnly &t)
        {
            char buf[8];
            std::snprintf(buf, sizeof(buf), "%02d:%02d", t.hour, t.minute);
            return buf;
        }
    } // namespace

    std::string snapshotJson()
    {
        LEDModule &led = openknxLEDModule;
        MeasuringModule &m = openknxMeasuringModule;
        const bool run = mcuRunning;
        std::string o = "{";

        // --- Ausgaenge
        o += "\"ports\":" + std::to_string(LED_HW_CHANNEL_COUNT) + ",\"pwm\":{\"freq\":" + num(pwmFrequency(), 1) +
             ",\"sleep\":" + b(pcaSleeping()) + ",\"regs\":[";
        for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
        {
            uint16_t on, off;
            bool fOn, fOff;
            portRegs(p, on, off, fOn, fOff);
            o += (p ? ",[" : "[") + std::to_string(on) + "," + std::to_string(off) + "," + b(fOn) + "," + b(fOff) + "]";
        }
        o += "]},\"duty\":[";
        for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
            o += (p ? "," : "") + num(busOn ? portDuty(p) : 0.0, 4);
        o += "]";

        // --- Last und Bausteine
        o += ",\"load\":{\"supplyOn\":" + b(load.supplyOn) + ",\"supplyV\":" + num(load.supplyV, 2) +
             ",\"I\":" + num(busOn ? loadCurrent() : 0.0, 3) + ",\"V\":" + num(busOn ? busVoltage() : 0.0, 3) +
             ",\"shunt\":" + num(shuntOhm, 4) + ",\"ambient\":" + num(thermal.ambientC, 1) +
             ",\"tempC\":" + num(thermal.tempC, 2) + ",\"portW\":[";
        for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
            o += (p ? "," : "") + num(load.portW[p], 2);
        o += "],\"short\":[";
        for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
            o += (p ? "," : "") + b(load.shortPort[p]);
        o += "]},\"chips\":{\"pca\":" + b(chips.pca) + ",\"ina\":" + b(chips.ina) + ",\"tmp\":" + b(chips.tmp) +
             ",\"oled\":" + b(chips.oled) + "},\"i2c\":{\"frames\":" + std::to_string(i2cFrames) +
             ",\"nacks\":" + std::to_string(i2cNacks) + "}";

        // --- LED-Modul
        o += ",\"led\":{\"running\":" + b(run) + ",\"device\":" + std::to_string(led.deviceSelect) +
             ",\"mode\":" + std::to_string(led.operatinModeSelect) + ",\"used\":" + std::to_string(led.usedChannels) +
             ",\"pwmFreq\":" + std::to_string(led.pwmFreqSelect) + ",\"pcaConn\":" + b(led.pcaI2cConnection) +
             ",\"powerFault\":" + b(led.isPowerFault()) + ",\"hwMismatch\":" + b(led.isHardwareMismatch()) + ",\"busPowerSave\":" + b(led._busPowerSave) +
             ",\"test\":{\"active\":" + b(led._testActive) + ",\"auto\":" + b(led._testAuto) +
             ",\"port\":" + std::to_string(led._testPort) + ",\"current\":" + num(led._testCurrentA, 3) + "}}";

        // --- Kanaele
        o += ",\"channels\":[";
        if (run && configured())
            for (int i = 0; i < led.usedChannels; i++)
                if (led.channel[i])
                    o += (i ? "," : "") + channelJson(led.channel[i]);
        o += "]";

        // --- HCL
        o += ",\"hcl\":[";
        if (run && configured())
            for (int i = 0; i < MAXCHANNELSHCL; i++)
                if (led.hclchannel[i])
                    o += std::string(i ? "," : "") + "{\"no\":" + std::to_string(i + 1) +
                         ",\"configured\":" + b(led.hclchannel[i]->_isConfigured) +
                         ",\"type\":" + std::to_string(static_cast<int>(led.hclchannel[i]->_type)) + "}";
        o += "]";

        // --- Messung
        o += ",\"meas\":{\"V\":" + num(m.busVoltage_V, 3) + ",\"I\":" + num(m.current_A, 3) + ",\"P\":" + num(m.power_W, 2) +
             ",\"Wh\":" + num(m.totalEnergy_Wh, 3) + ",\"T\":" + num(m.temperatur_C, 2) + ",\"ina\":" + b(m.inaI2cConnection) +
             ",\"tmp\":" + b(m.tempI2cConnection) + ",\"alarm\":{\"overV\":" + b(m.overVoltageTriggered) +
             ",\"underV\":" + b(m.underVoltageTriggered) + ",\"overI\":" + b(m.overCurrentTriggered) +
             ",\"overT\":" + b(m.overTempTriggered) + "},\"verify\":" + b(m._verifyActive) +
             ",\"inaI\":" + num(inaCurrent(), 3) + "}";

        // --- LEDs, Bedienfeld, Uhrzeit
        o += ",\"leds\":[" + ledJson(openknx.leds.getLed(0)) + "," + ledJson(openknx.leds.getLed(1)) + "," +
             ledJson(openknx.leds.getLed(2)) + "," + ledJson(openknx.leds.getLed(3)) + "]";
        o += ",\"panel\":{\"on\":" + b(oledOn) + ",\"screen\":" + std::to_string(openknxFrontPanelModule.currentscreen) + "}";
        const OpenKNX::SimDateTime t = openknx.time.getLocalTime();
        char ts[32];
        std::snprintf(ts, sizeof(ts), "%04d-%02d-%02d %02d:%02d:%02d", t.year, t.month, t.day, t.hour, t.minute, t.second);
        o += ",\"time\":{\"valid\":" + b(timeValid) + ",\"local\":\"" + ts + "\",\"sunrise\":\"" +
             hhmm(openknx.sun.sunRiseLocalTime()) + "\",\"sunset\":\"" + hhmm(openknx.sun.sunSetLocalTime()) + "\"}";
        return o + "}";
    }

    // ------------------------------------------------------------ Waechter

    namespace
    {
        std::map<std::string, std::pair<uint64_t, int>> lastWarn; // key -> (Zeit, Anzahl)
        uint64_t overSince = 0;
        uint64_t lastNacks = 0;
    } // namespace

    void warn(const std::string &key, const std::string &inv, const std::string &text)
    {
        auto &w = lastWarn[key];
        w.second++;
        if (w.first != 0 && now - w.first < 5000)
            return;
        w.first = now;
        emit("{\"ev\":\"warn\",\"t\":" + std::to_string(now) + ",\"inv\":" + esc(inv) + ",\"text\":" + esc(text) +
             ",\"count\":" + std::to_string(w.second) + "}");
        w.second = 0;
    }

    static void info(const std::string &text)
    {
        emit("{\"ev\":\"info\",\"t\":" + std::to_string(now) + ",\"text\":" + esc(text) + "}");
    }

    void resetInvariants()
    {
        overSince = 0;
    }

    void checkInvariants()
    {
        if (!busOn)
            return;
        // Ueberstrom muss abgeschaltet werden (Hardware-Schutz bei 6 A)
        const double i = loadCurrent();
        if (i > 6.5)
        {
            if (overSince == 0)
                overSince = now;
            else if (now - overSince > 1500)
                warn("overcurrent", "Überstrom",
                     "Strom " + num(i, 2) + " A seit " + num((now - overSince) / 1000.0, 1) +
                         " s über 6 A ohne Abschaltung");
        }
        else
            overSince = 0;

        // Bei gesetztem Fehler-Latch duerfen keine Ausgaenge leuchten
        if (mcuRunning && openknxLEDModule.isPowerFault())
            for (int p = 0; p < LED_HW_CHANNEL_COUNT; p++)
                if (portDuty(p) > 0.0)
                {
                    warn("latch-out", "Latch", "Ausgang " + std::string(1, HWPortsMapping[p]) +
                                                  " ist an, obwohl der Überstrom-Latch gesetzt ist");
                    break;
                }

        if (i2cNacks != lastNacks)
        {
            if (i2cNacks - lastNacks > 0 && (chips.pca && chips.ina))
                info("I2C: " + std::to_string(i2cNacks - lastNacks) + " Zugriff(e) ohne Antwort");
            lastNacks = i2cNacks;
        }
    }
} // namespace sim
