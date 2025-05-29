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
void LEDHelper::calcGammaTable(float gamma)
{
    for (size_t i = 0; i < 256; i++)
    {
        gammaT[i] = (int)(powf((float)i / 255.0f, gamma) * 255.0f + 0.5f);
    }
}

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
        return time * 100;
    case TIMEBASE_SECONDS:
        return time * 1000;
    case TIMEBASE_MINUTES:
        return time * 60000;
    case TIMEBASE_HOURS:
        return time * 3600000;
    default:
        return 0;
    }
}

//----------------------Time Calculator ------------------------------