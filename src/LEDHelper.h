#pragma once

#include <Arduino.h>
#include "OpenKNX.h"

#define TIMEBASE_SECONDS        0
#define TIMEBASE_MINUTES        1
#define TIMEBASE_HOURS          2
#define TIMEBASE_TENTH_SECONDS  3

class LEDHelper
{
public:
    static void hsvToRGB(uint16_t in_h, uint16_t in_s, uint16_t in_v, uint8_t &out_r, uint8_t &out_g, uint8_t &out_b);
    static void rgbToHSV(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint16_t &out_h, uint16_t &out_s, uint16_t &out_v);
    static void kelvinToRGB(uint16_t kelvin, uint8_t brightness, uint8_t &out_r, uint8_t &out_g, uint8_t &out_b);
    static void adjustRGBBrightness(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness, uint8_t &out_r, uint8_t &out_g, uint8_t &out_b);
    static void calcGammaTable(float gamma, uint8_t *table); // füllt eine übergebene Tabelle (z.B. per Kanal)
    static uint32_t getTimeWithPattern(uint16_t time, uint8_t base);
private:
    static double threeway_max(double a, double b, double c);
    static double threeway_min(double a, double b, double c);
};

// all dim curves (12-bit): 0(A)=linear, 1(B)=gamma2.8, 2(C)=gamma3.8, 3(D)=CIE, 4(E)=DALI - Definition in LEDHelper.cpp
extern const uint16_t curves[256][5];
