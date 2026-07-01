#include "LEDHelper.h"

//----------------------Color Converter ------------------------------
// extended lib from https://github.com/ratkins/RGBConverter
// WTFPL license

void LEDHelper::hsvToRGB(uint16_t in_h, uint16_t in_s, uint16_t in_v, uint8_t &out_r, uint8_t &out_g, uint8_t &out_b)
{
    float h = in_h / 255.0;
    float s = in_s / 255.0;
    float v = in_v / 255.0;

    double rt = 0;
    double gt = 0;
    double bt = 0;

    int i = int(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    switch (i % 6)
    {
    case 0:
        rt = v, gt = t, bt = p;
        break;
    case 1:
        rt = q, gt = v, bt = p;
        break;
    case 2:
        rt = p, gt = v, bt = t;
        break;
    case 3:
        rt = p, gt = q, bt = v;
        break;
    case 4:
        rt = t, gt = p, bt = v;
        break;
    case 5:
        rt = v, gt = p, bt = q;
        break;
    }
    out_r = rt * 255;
    out_g = gt * 255;
    out_b = bt * 255;
}

void LEDHelper::rgbToHSV(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint16_t &out_h, uint16_t &out_s, uint16_t &out_v)
{
    double rd = (double)in_r / 255;
    double gd = (double)in_g / 255;
    double bd = (double)in_b / 255;
    double max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
    double h = 0, s, v = max;

    double d = max - min;
    s = max == 0 ? 0 : d / max;

    if (max != min)
    {
        if (max == rd)
        {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        }
        else if (max == gd)
        {
            h = (bd - rd) / d + 2;
        }
        else if (max == bd)
        {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }
    /*
    h = h * 360;
    if (h >= 0 && h <= 255) {
        out_h = static_cast<uint8_t>(h);
    } else if (h > 255 && h <= 360) {
        double converted_h = ((h - 256.0) / 104.0) * 255.0;
        converted_h = constrain(converted_h, 0.0, 255.0);
        out_h = static_cast<uint8_t>(round(converted_h));
    }
    */
    out_h = h * 360;
    out_s = s * 100;
    out_v = v * 100;
}

void LEDHelper::kelvinToRGB(uint16_t kelvin, uint8_t brightness, uint8_t &out_r, uint8_t &out_g, uint8_t &out_b)
{
    float temp = kelvin / 100.0f;
    float r, g, b;

    if (temp <= 66.0f)
    {
        r = 255.0f;
        g = 99.4708025861f * log(temp) - 161.1195681661f;

        if (temp <= 19.0f)
            b = 0.0f;
        else
            b = 138.5177312231f * log(temp - 10.0f) - 305.0447927307f;
    }
    else
    {
        r = 329.698727446f * pow(temp - 60.0f, -0.1332047592f);
        g = 288.1221695283f * pow(temp - 60.0f, -0.0755148492f);
        b = 255.0f;
    }

    // Helligkeit anwenden
    float brightness_factor = brightness / 100.0f;
    r *= brightness_factor;
    g *= brightness_factor;
    b *= brightness_factor;

    // Werte auf 0-255 begrenzen
    r = (r > 255.0f) ? 255.0f : (r < 0.0f) ? 0.0f : r;
    g = (g > 255.0f) ? 255.0f : (g < 0.0f) ? 0.0f : g;
    b = (b > 255.0f) ? 255.0f : (b < 0.0f) ? 0.0f : b;

    // Runden und zuweisen
    out_r = static_cast<uint8_t>(round(r));
    out_g = static_cast<uint8_t>(round(g));
    out_b = static_cast<uint8_t>(round(b));
}

void LEDHelper::adjustRGBBrightness(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness, uint8_t &out_r, uint8_t &out_g, uint8_t &out_b) {
    float brightness_factor = brightness / 100.0f;
    out_r = static_cast<uint8_t>(std::round(r * brightness_factor));
    out_g = static_cast<uint8_t>(std::round(g * brightness_factor)); 
    out_b = static_cast<uint8_t>(std::round(b * brightness_factor));
}

double LEDHelper::threeway_max(double a, double b, double c)
{
    return max(a, max(b, c));
}

double LEDHelper::threeway_min(double a, double b, double c)
{
    return min(a, min(b, c));
}

//----------------------Color Converter ------------------------------

//----------------------Gamma Converter ------------------------------

// re-calculates & fills gamma table
void LEDHelper::calcGammaTable(float gamma, uint8_t *table)
{
    for (size_t i = 0; i < 256; i++)
    {
        table[i] = (int)(powf((float)i / 255.0f, gamma) * 255.0f + 0.5f);
    }
}

void LEDHelper::calcGammaTable(float gamma)
{
    calcGammaTable(gamma, gammaT);
}

uint8_t gammaT[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

//----------------------Gamma Converter ------------------------------

//----------------------Time Calculator ------------------------------

uint32_t LEDHelper::getTimeWithPattern(uint16_t time, uint8_t base)
{
    if (base == TIMEBASE_HOURS && time > 1000)
    {
        time = 1000; // Begrenzung auf maximal 1000 Stunden
    }

    switch (base)
    {
    case TIMEBASE_TENTH_SECONDS:
        return (uint32_t)time * 100UL;
    case TIMEBASE_SECONDS:
        return (uint32_t)time * 1000UL;
    case TIMEBASE_MINUTES:
        return (uint32_t)time * 60000UL;
    case TIMEBASE_HOURS:
        return (uint32_t)time * 3600000UL;
    default:
        return 0;
    }
}

//----------------------Time Calculator ------------------------------