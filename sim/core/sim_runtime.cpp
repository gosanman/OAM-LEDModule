// ============================================================================
// Simulator: Laufzeit fuer die OpenKNX-Nachbildung (OpenKNX.h).
// ============================================================================

#include <cmath>
#include <cstdio>

#include "sim.h"
#include "SPI.h"

SimKnx knx;
OpenKNX::SimOpenKnx openknx;
SimSerial Serial;
SPIClass SPI;

static uint8_t lengthFor(const Dpt &type)
{
    const uint8_t n = type.dataLength();
    return n == 0 ? 1 : (n > 14 ? 14 : n);
}

namespace sim
{
    uint64_t now = 0;
    uint64_t bootAt = 0;
    std::vector<uint8_t> params(MAIN_ParameterSize, 0);
    bool paramsValid = false;
    bool downloading = false;
    bool mcuRunning = false;
    bool busOn = false;
    std::vector<GroupObject> kos(MAIN_MaxKoNumber + 1);
    std::vector<std::vector<uint8_t>> flashBlocks;
    bool flashValid = false;
    bool flashSaveRequested = false;
    bool timeValid = true;
    int64_t timeStartEpoch = 0;
    std::string individualAddress = "1.1.36";

    bool configured() { return paramsValid && !downloading; }

    void emit(const std::string &json)
    {
        std::fputs(json.c_str(), stdout);
        std::fputc('\n', stdout);
    }

    /// true: s ist gueltiges UTF-8 (Quelltexte der Firmware), sonst ISO 8859-1
    /// (Texte aus dem ETS-Parameterspeicher, DPT 16).
    static bool isUtf8(const std::string &s)
    {
        for (size_t i = 0; i < s.size();)
        {
            const unsigned char c = s[i];
            const int n = c < 0x80 ? 0 : (c & 0xE0) == 0xC0 ? 1 : (c & 0xF0) == 0xE0 ? 2 : (c & 0xF8) == 0xF0 ? 3 : -1;
            if (n < 0 || i + n >= s.size())
                return false;
            for (int k = 1; k <= n; k++)
                if ((static_cast<unsigned char>(s[i + k]) & 0xC0) != 0x80)
                    return false;
            i += n + 1;
        }
        return true;
    }

    std::string esc(const std::string &s)
    {
        const bool utf8 = isUtf8(s);
        std::string o = "\"";
        for (unsigned char c : s)
        {
            switch (c)
            {
                case '"': o += "\\\""; break;
                case '\\': o += "\\\\"; break;
                case '\n': o += "\\n"; break;
                case '\r': break;
                case '\t': o += "\\t"; break;
                default:
                    if (c < 0x20)
                    {
                        char b[8];
                        std::snprintf(b, sizeof(b), "\\u%04x", c);
                        o += b;
                    }
                    else if (c >= 0x80 && utf8)
                        o += static_cast<char>(c);
                    else if (c >= 0x80)
                    {
                        // ISO 8859-1 (ETS-Texte, Grad-Zeichen in DPT 16) nach UTF-8
                        o += static_cast<char>(0xC0 | (c >> 6));
                        o += static_cast<char>(0x80 | (c & 0x3F));
                    }
                    else
                        o += static_cast<char>(c);
            }
        }
        return o + "\"";
    }

    std::string num(double v, int decimals)
    {
        if (!std::isfinite(v))
            return "null";
        char b[64];
        std::snprintf(b, sizeof(b), "%.*f", decimals, v);
        std::string s = b;
        if (s.find('.') != std::string::npos)
        {
            while (!s.empty() && s.back() == '0')
                s.pop_back();
            if (!s.empty() && s.back() == '.')
                s.pop_back();
        }
        if (s == "-0")
            s = "0";
        return s;
    }

    std::string dptText(const Dpt &t)
    {
        return std::to_string(t.mainGroup) + "." + std::to_string(t.subGroup);
    }

    Dpt parseDpt(const std::string &s)
    {
        int main = 0, sub = 0;
        std::sscanf(s.c_str(), "%d.%d", &main, &sub);
        return Dpt(main, sub);
    }

    /// Gleich grosser Ersatztyp, wenn die KNX-Bibliothek einen DPT nicht kennt.
    Dpt fallbackDpt(const Dpt &type)
    {
        switch (type.mainGroup)
        {
            case 1: return Dpt(1, 1);
            case 3: return Dpt(3, 7);
            case 5: return Dpt(5, 10);
            case 7: return Dpt(7, 1);
            case 9: return Dpt(9, 1);
            case 13: return Dpt(13, 1);
            case 14: return Dpt(14, 0);
            case 17: return Dpt(17, 1);
            default: return type;
        }
    }

    /// Nutzdaten wie am Bus, damit die Oberflaeche nach dem ETS-DPT dekodieren kann.
    std::string rawHex(GroupObject &ko, const Dpt &type)
    {
        static const char *hx = "0123456789abcdef";
        std::string s;
        const uint8_t *d = ko.simData();
        for (uint8_t i = 0; i < lengthFor(type); i++)
        {
            s += hx[d[i] >> 4];
            s += hx[d[i] & 0x0F];
        }
        return s;
    }

    std::string koValueJson(GroupObject &ko, const Dpt &requested)
    {
        Dpt type = requested;
        KNXValue v = "";
        uint8_t *data = const_cast<uint8_t *>(ko.simData());
        // Gemischte Typen als Rohwert: DPT 3 (Richtung + Schritt), DPT 232 (RGB, 3 Byte)
        if (type.mainGroup == 3)
            return std::to_string(data[0] & 0x0F);
        if (type.mainGroup == 232)
            return std::to_string((data[0] << 16) | (data[1] << 8) | data[2]);
        if (!KNX_Decode_Value(data, lengthFor(type), type, v))
        {
            type = fallbackDpt(type);
            v = "";
            KNX_Decode_Value(data, lengthFor(type), type, v);
        }
        if (type.mainGroup == 1)
            return static_cast<bool>(v) ? "true" : "false";
        if (type.mainGroup == 16)
            return esc(static_cast<const char *>(v));
        return num(static_cast<double>(v), 3);
    }
} // namespace sim

// ------------------------------------------------------------ Gruppenobjekte

// Verhalten wie lib/knx/src/knx/group_object.cpp - auch im Fehlerfall:
//  - value(type): scheitert die Dekodierung, kommt KNXValue("") zurueck, also 0.
//  - value(v, type): scheitert die Kodierung, bleiben die Daten unveraendert,
//    und es wird NICHT gesendet.
// Beides meldet der Simulator, weil es am Geraet unbemerkt passiert.
KNXValue GroupObject::value(const Dpt &type)
{
    KNXValue v = "";
    if (!KNX_Decode_Value(mData, lengthFor(type), type, v))
        sim::warn("DPT-R" + std::to_string(mNumber), "DPT",
                  "Firmware liest Objekt " + std::to_string(mNumber) + " als DPT " + sim::dptText(type) +
                      " - die KNX-Bibliothek kann diesen DPT nicht dekodieren, Ergebnis 0");
    return v;
}

bool GroupObject::valueNoSend(const KNXValue &value, const Dpt &type)
{
    if (!KNX_Encode_Value(value, mData, lengthFor(type), type))
        return false;
    mInitialized = true;
    mLastDpt = type;
    return true;
}

bool GroupObject::valueNoSendCompare(const KNXValue &value, const Dpt &type)
{
    if (!mInitialized)
    {
        if (!valueNoSend(value, type))
            sim::warn("DPT-W" + std::to_string(mNumber), "DPT",
                      "Firmware setzt Objekt " + std::to_string(mNumber) + " als DPT " + sim::dptText(type) +
                          " - die KNX-Bibliothek kann diesen DPT nicht kodieren");
        return true;
    }
    uint8_t tmp[16] = {};
    if (!KNX_Encode_Value(value, tmp, lengthFor(type), type))
    {
        sim::warn("DPT-W" + std::to_string(mNumber), "DPT",
                  "Firmware setzt Objekt " + std::to_string(mNumber) + " als DPT " + sim::dptText(type) +
                      " - die KNX-Bibliothek kann diesen DPT nicht kodieren");
        return false;
    }
    mLastDpt = type;
    const bool changed = std::memcmp(tmp, mData, lengthFor(type)) != 0;
    if (changed)
        std::memcpy(mData, tmp, lengthFor(type));
    return changed;
}

void GroupObject::objectWritten()
{
    // Jeder Aufruf ist ein Telegramm - genau wie im Stack.
    sim::emit(std::string("{\"ev\":\"ko\",\"t\":") + std::to_string(sim::now) +
              ",\"nr\":" + std::to_string(mNumber) + ",\"dpt\":\"" + sim::dptText(mLastDpt) +
              "\",\"val\":" + sim::koValueJson(*this, mLastDpt) + ",\"raw\":\"" + sim::rawHex(*this, mLastDpt) + "\"}");
}

void GroupObject::value(const KNXValue &value, const Dpt &type)
{
    if (!valueNoSend(value, type))
    {
        sim::warn("DPT-W" + std::to_string(mNumber), "DPT",
                  "Firmware sendet Objekt " + std::to_string(mNumber) + " als DPT " + sim::dptText(type) +
                      " - die KNX-Bibliothek kann diesen DPT nicht kodieren, das Telegramm entfaellt");
        return;
    }
    objectWritten();
}

bool GroupObject::simReceive(const KNXValue &value, const Dpt &type)
{
    std::memset(mData, 0, sizeof(mData));
    if (KNX_Encode_Value(value, mData, lengthFor(type), type))
    {
        mInitialized = true;
        mLastDpt = type;
        return true;
    }
    std::memset(mData, 0, sizeof(mData));
    const Dpt alt = sim::fallbackDpt(type);
    if (alt != type && KNX_Encode_Value(value, mData, lengthFor(alt), alt))
    {
        mInitialized = true;
        mLastDpt = type;
        return true;
    }
    return false;
}

// ------------------------------------------------------------ Parameter

uint8_t SimKnx::paramByte(uint32_t addr)
{
    return addr < sim::params.size() ? sim::params[addr] : 0;
}

uint16_t SimKnx::paramWord(uint32_t addr)
{
    return static_cast<uint16_t>((paramByte(addr) << 8) | paramByte(addr + 1));
}

uint32_t SimKnx::paramInt(uint32_t addr)
{
    return (static_cast<uint32_t>(paramByte(addr)) << 24) |
           (static_cast<uint32_t>(paramByte(addr + 1)) << 16) |
           (static_cast<uint32_t>(paramByte(addr + 2)) << 8) | paramByte(addr + 3);
}

double SimKnx::paramFloat(uint32_t addr, ParameterFloatEncodings enc)
{
    if (enc == Float_Enc_IEEE754Single)
    {
        uint32_t raw = paramInt(addr);
        float f;
        std::memcpy(&f, &raw, sizeof(f));
        return f;
    }
    if (enc == Float_Enc_DPT9)
    {
        // DPT 9: S EEEE MMMMMMMMMMM, Wert = 0,01 * M * 2^E, M mit Vorzeichen
        const uint16_t raw = paramWord(addr);
        const int exp = (raw >> 11) & 0x0F;
        int mant = raw & 0x07FF;
        if (raw & 0x8000)
            mant -= 2048;
        return 0.01 * mant * static_cast<double>(1 << exp);
    }
    return 0.0;
}

uint8_t *SimKnx::paramData(uint32_t addr)
{
    return addr < sim::params.size() ? &sim::params[addr] : nullptr;
}

char *SimKnx::paramString(uint32_t addr, uint32_t length)
{
    static std::string buffer;
    buffer.assign(reinterpret_cast<const char *>(paramData(addr)), length);
    return buffer.data();
}

GroupObject &SimKnx::getGroupObject(uint16_t number)
{
    if (number >= sim::kos.size())
        return sim::kos[0];
    return sim::kos[number];
}

bool SimKnx::configured() { return sim::configured(); }

bool delayCheck(uint32_t last, uint32_t duration) { return millis() - last >= duration; }

// ------------------------------------------------------------ Logger, Konsole

namespace OpenKNX
{
    static void emitLog(const char *level, const std::string &prefix, const std::string &text, int indent)
    {
        std::string line(static_cast<size_t>(indent) * 2, ' ');
        line += text;
        sim::emit(std::string("{\"ev\":\"log\",\"t\":") + std::to_string(sim::now) + ",\"level\":\"" + level +
                  "\",\"src\":" + sim::esc(prefix) + ",\"text\":" + sim::esc(line) + "}");
    }

    void SimLogger::logMacroWrapper(int color, const char *prefix, const char *fmt, ...)
    {
        char buf[512];
        va_list ap;
        va_start(ap, fmt);
        std::vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        emitLog(color == 31 ? "error" : (color == 90 ? "debug" : "info"), prefix ? prefix : "", buf, mIndent);
    }

    void SimLogger::logWithPrefixAndValues(const char *prefix, const char *fmt, ...)
    {
        char buf[512];
        va_list ap;
        va_start(ap, fmt);
        std::vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        emitLog("info", prefix ? prefix : "", buf, mIndent);
    }

    void SimLogger::logWithPrefixAndValues(const std::string &prefix, const char *fmt, ...)
    {
        char buf[512];
        va_list ap;
        va_start(ap, fmt);
        std::vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        emitLog("info", prefix, buf, mIndent);
    }

    void SimLogger::log(const char *text) { emitLog("info", "", text, mIndent); }

    std::string SimLogger::buildUptime()
    {
        const uint32_t s = millis() / 1000;
        char b[32];
        std::snprintf(b, sizeof(b), "%ud %02u:%02u:%02u", s / 86400, (s / 3600) % 24, (s / 60) % 60, s % 60);
        return b;
    }

    void SimConsole::printHelpLine(const char *cmd, const char *text)
    {
        char buf[256];
        std::snprintf(buf, sizeof(buf), "%-20s %s", cmd, text);
        emitLog("info", "Hilfe", buf, 0);
    }

    void SimConsole::writeDiagenoseKo(const char *fmt, ...)
    {
        char buf[64];
        va_list ap;
        va_start(ap, fmt);
        std::vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
        buf[14] = '\0'; // DPT 16: 14 Zeichen
#ifdef BASE_KoDiagnose
        knx.getGroupObject(BASE_KoDiagnose).value(buf, Dpt(16, 1));
#endif
    }

    // ---------------------------------------------------------------- Flash

    void SimFlash::save(bool) { sim::flashSaveRequested = true; }
    void SimFlash::writeByte(uint8_t v)
    {
        if (writeTarget != nullptr)
            writeTarget->push_back(v);
    }
    void SimFlash::writeWord(uint16_t v)
    {
        writeByte(static_cast<uint8_t>(v >> 8));
        writeByte(static_cast<uint8_t>(v));
    }
    void SimFlash::writeInt(uint32_t v)
    {
        writeWord(static_cast<uint16_t>(v >> 16));
        writeWord(static_cast<uint16_t>(v));
    }
    void SimFlash::writeFloat(float v)
    {
        uint32_t raw;
        std::memcpy(&raw, &v, sizeof(raw));
        writeInt(raw);
    }
    void SimFlash::writeDouble(double v)
    {
        uint64_t raw;
        std::memcpy(&raw, &v, sizeof(raw));
        writeInt(static_cast<uint32_t>(raw >> 32));
        writeInt(static_cast<uint32_t>(raw));
    }
    uint8_t SimFlash::readByte()
    {
        if (readPtr == nullptr || readPtr >= readEnd)
            return 0;
        return *readPtr++;
    }
    uint16_t SimFlash::readWord()
    {
        const uint16_t hi = readByte();
        return static_cast<uint16_t>((hi << 8) | readByte());
    }
    uint32_t SimFlash::readInt()
    {
        const uint32_t hi = readWord();
        return (hi << 16) | readWord();
    }
    float SimFlash::readFloat()
    {
        const uint32_t raw = readInt();
        float f;
        std::memcpy(&f, &raw, sizeof(f));
        return f;
    }
    double SimFlash::readDouble()
    {
        const uint64_t hi = readInt();
        const uint64_t raw = (hi << 32) | readInt();
        double d;
        std::memcpy(&d, &raw, sizeof(d));
        return d;
    }

    // ---------------------------------------------------------------- Info

    std::string SimInfo::firmwareName() { return MAIN_FirmwareName; }
    std::string SimInfo::humanIndividualAddress() { return sim::individualAddress; }

    // ---------------------------------------------------------------- Uhrzeit

    /// Tage seit 1970-01-01 -> Datum (H. Hinnant, civil_from_days).
    static void civilFromDays(int64_t z, int &y, int &m, int &d)
    {
        z += 719468;
        const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
        const unsigned doe = static_cast<unsigned>(z - era * 146097);
        const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        const int64_t yy = static_cast<int64_t>(yoe) + era * 400;
        const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        const unsigned mp = (5 * doy + 2) / 153;
        d = static_cast<int>(doy - (153 * mp + 2) / 5 + 1);
        m = static_cast<int>(mp < 10 ? mp + 3 : mp - 9);
        y = static_cast<int>(yy + (m <= 2));
    }

    /// Datum -> Tage seit 1970-01-01 (H. Hinnant, days_from_civil).
    static int64_t daysFromCivil(int y, int m, int d)
    {
        y -= m <= 2;
        const int64_t era = (y >= 0 ? y : y - 399) / 400;
        const unsigned yoe = static_cast<unsigned>(y - era * 400);
        const unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + static_cast<unsigned>(d) - 1;
        const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + static_cast<int64_t>(doe) - 719468;
    }

    static int64_t localEpoch() { return sim::timeStartEpoch + static_cast<int64_t>(sim::now / 1000); }

    bool SimTime::isValid() { return sim::timeValid; }

    SimDateTime SimTime::getLocalTime()
    {
        const int64_t e = localEpoch();
        const int64_t days = e >= 0 ? e / 86400 : (e - 86399) / 86400;
        const int64_t sec = e - days * 86400;
        SimDateTime t;
        civilFromDays(days, t.year, t.month, t.day);
        t.hour = static_cast<int>(sec / 3600);
        t.minute = static_cast<int>((sec / 60) % 60);
        t.second = static_cast<int>(sec % 60);
        return t;
    }

    // ---------------------------------------------------------------- Sonnenstand

    /// Sonnenauf-/-untergang (Sunrise Equation, Almanac for Computers 1990) in
    /// lokaler Zeit, Mitteleuropa mit Sommerzeit wie die OpenKNX-Vorgabe.
    static bool sunTime(bool rise, TimeOnly &out)
    {
        double lat = 50.11538, lon = 8.683764;
#ifdef ParamBASE_Latitude
        if (sim::configured())
        {
            lat = ParamBASE_Latitude;
            lon = ParamBASE_Longitude;
        }
#endif
        const SimDateTime d = openknx.time.getLocalTime();
        static const int cum[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        const bool leap = (d.year % 4 == 0 && d.year % 100 != 0) || d.year % 400 == 0;
        const int n = cum[d.month - 1] + d.day + (leap && d.month > 2 ? 1 : 0);
        const double rad = M_PI / 180.0;
        const double lngHour = lon / 15.0;
        const double t = n + ((rise ? 6.0 : 18.0) - lngHour) / 24.0;
        const double M = 0.9856 * t - 3.289;
        double L = M + 1.916 * std::sin(M * rad) + 0.020 * std::sin(2 * M * rad) + 282.634;
        L = std::fmod(L + 360.0, 360.0);
        double RA = std::atan(0.91764 * std::tan(L * rad)) / rad;
        RA = std::fmod(RA + 360.0, 360.0);
        RA += std::floor(L / 90.0) * 90.0 - std::floor(RA / 90.0) * 90.0;
        RA /= 15.0;
        const double sinDec = 0.39782 * std::sin(L * rad);
        const double cosDec = std::cos(std::asin(sinDec));
        const double cosH = (std::cos(90.833 * rad) - sinDec * std::sin(lat * rad)) / (cosDec * std::cos(lat * rad));
        if (cosH > 1.0 || cosH < -1.0)
            return false; // Polarnacht / Mitternachtssonne
        double H = std::acos(cosH) / rad;
        if (rise)
            H = 360.0 - H;
        H /= 15.0;
        const double T = H + RA - 0.06571 * t - 6.622;
        double UT = std::fmod(T - lngHour + 48.0, 24.0);
        // Sommerzeit: letzter Sonntag im Maerz bis letzter Sonntag im Oktober (vereinfacht am Datum)
        auto lastSunday = [&](int month) {
            const int64_t days = daysFromCivil(d.year, month, 31);
            const int wd = static_cast<int>(((days % 7) + 11) % 7); // 1970-01-01 = Donnerstag, 0 = Sonntag
            return 31 - wd;
        };
        const int ds = lastSunday(3), de = lastSunday(10);
        const bool dst = (d.month > 3 && d.month < 10) || (d.month == 3 && d.day >= ds) || (d.month == 10 && d.day < de);
        double local = std::fmod(UT + (dst ? 2.0 : 1.0), 24.0);
        const int minutes = static_cast<int>(std::lround(local * 60.0)) % 1440;
        out.hour = static_cast<uint8_t>(minutes / 60);
        out.minute = static_cast<uint8_t>(minutes % 60);
        out.second = 0;
        return true;
    }

    bool SimSun::isSunCalculatioValid() { return sim::timeValid; }

    TimeOnly SimSun::sunRiseLocalTime()
    {
        TimeOnly t;
        sunTime(true, t);
        return t;
    }

    TimeOnly SimSun::sunSetLocalTime()
    {
        TimeOnly t;
        sunTime(false, t);
        return t;
    }
} // namespace OpenKNX
