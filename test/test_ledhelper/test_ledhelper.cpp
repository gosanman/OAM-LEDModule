// Tests der hardwareunabhaengigen Rechnungen in LEDHelper:
// Dimmkurven, Farbumrechnung HSV/RGB, Farbtemperatur, Helligkeit, Gamma, Zeitbasis.
//
// Laeuft auf dem PC: pio test -e native (Task "native Tests").

#include <unity.h>

#include "LEDHelper.h"

void setUp(void) {}
void tearDown(void) {}

// ------------------------------------------------------------ Dimmkurven

static void test_kurven_von_aus_bis_voll(void)
{
    // 0 % ist aus, 100 % ist voller 12-Bit-Wert - bei jeder Kurve
    for (int c = 0; c < 5; c++)
    {
        TEST_ASSERT_EQUAL_UINT16_MESSAGE(0, curves[0][c], "Stufe 0 muss aus sein");
        TEST_ASSERT_EQUAL_UINT16_MESSAGE(4095, curves[255][c], "Stufe 255 muss voll sein");
    }
}

static void test_kurven_steigen_monoton(void)
{
    // heller dimmen darf nie dunkler werden
    for (int c = 0; c < 5; c++)
        for (int i = 1; i < 256; i++)
            TEST_ASSERT_TRUE_MESSAGE(curves[i][c] >= curves[i - 1][c], "Kurve faellt");
}

static void test_kurven_im_wertebereich(void)
{
    for (int c = 0; c < 5; c++)
        for (int i = 0; i < 256; i++)
            TEST_ASSERT_TRUE(curves[i][c] <= 4095);
}

static void test_lineare_kurve_halbe_helligkeit(void)
{
    // Kurve A (linear): Stufe 128 liegt bei etwa halbem Tastgrad
    TEST_ASSERT_UINT16_WITHIN(32, 2056, curves[128][0]);
}

static void test_gammakurven_unter_linear(void)
{
    // Gamma- und CIE-Kurven sind in der Mitte dunkler als linear (Augenempfindlichkeit)
    for (int c = 1; c < 5; c++)
        TEST_ASSERT_TRUE(curves[128][c] < curves[128][0]);
}

// ------------------------------------------------------------ HSV -> RGB

static void hsv(uint16_t h, uint16_t s, uint16_t v, uint8_t er, uint8_t eg, uint8_t eb)
{
    uint8_t r, g, b;
    LEDHelper::hsvToRGB(h, s, v, r, g, b);
    TEST_ASSERT_EQUAL_UINT8(er, r);
    TEST_ASSERT_EQUAL_UINT8(eg, g);
    TEST_ASSERT_EQUAL_UINT8(eb, b);
}

static void test_hsv_grundfarben(void)
{
    // Farbton 0..255 entspricht 0..360 Grad
    hsv(0, 255, 255, 255, 0, 0);   // rot
    hsv(85, 255, 255, 0, 255, 0);  // gruen (120 Grad)
    hsv(170, 255, 255, 0, 0, 255); // blau (240 Grad)
    hsv(255, 255, 255, 255, 0, 0); // 360 Grad = rot
}

static void test_hsv_ohne_saettigung_ist_grau(void)
{
    hsv(40, 0, 255, 255, 255, 255);
    hsv(200, 0, 0, 0, 0, 0);
}

// ------------------------------------------------------------ RGB -> HSV

static void rgb(uint8_t r, uint8_t g, uint8_t b, uint16_t eh, uint16_t es, uint16_t ev)
{
    uint16_t h, s, v;
    LEDHelper::rgbToHSV(r, g, b, h, s, v);
    TEST_ASSERT_EQUAL_UINT16(eh, h);
    TEST_ASSERT_EQUAL_UINT16(es, s);
    TEST_ASSERT_EQUAL_UINT16(ev, v);
}

static void test_rgb_grundfarben(void)
{
    // Ergebnis: Farbton in Grad, Saettigung und Helligkeit in Prozent
    rgb(255, 0, 0, 0, 100, 100);
    rgb(0, 255, 0, 120, 100, 100);
    rgb(0, 0, 255, 240, 100, 100);
    rgb(0, 0, 0, 0, 0, 0);
    rgb(255, 255, 255, 0, 0, 100);
}

// ------------------------------------------------------------ Farbtemperatur

static void test_kelvin_6600_ist_weiss(void)
{
    uint8_t r, g, b;
    LEDHelper::kelvinToRGB(6600, 100, r, g, b);
    TEST_ASSERT_EQUAL_UINT8(255, r);
    TEST_ASSERT_EQUAL_UINT8(255, g);
    TEST_ASSERT_UINT8_WITHIN(5, 252, b);
}

static void test_kelvin_warmweiss(void)
{
    // 2700 K: Rot voll, Gruen deutlich weniger, Blau am wenigsten
    uint8_t r, g, b;
    LEDHelper::kelvinToRGB(2700, 100, r, g, b);
    TEST_ASSERT_EQUAL_UINT8(255, r);
    TEST_ASSERT_TRUE(g > b);
    TEST_ASSERT_TRUE(g < 200);
}

static void test_kelvin_helligkeit_null_ist_aus(void)
{
    uint8_t r, g, b;
    LEDHelper::kelvinToRGB(4000, 0, r, g, b);
    TEST_ASSERT_EQUAL_UINT8(0, r);
    TEST_ASSERT_EQUAL_UINT8(0, g);
    TEST_ASSERT_EQUAL_UINT8(0, b);
}

// ------------------------------------------------------------ Helligkeit und Gamma

static void test_rgb_helligkeit_halbiert(void)
{
    uint8_t r, g, b;
    LEDHelper::adjustRGBBrightness(255, 128, 0, 50, r, g, b);
    TEST_ASSERT_EQUAL_UINT8(128, r);
    TEST_ASSERT_EQUAL_UINT8(64, g);
    TEST_ASSERT_EQUAL_UINT8(0, b);
}

static void test_gamma_eins_ist_identitaet(void)
{
    uint8_t table[256];
    LEDHelper::calcGammaTable(1.0f, table);
    for (int i = 0; i < 256; i++)
        TEST_ASSERT_EQUAL_UINT8(i, table[i]);
}

static void test_gamma_kurve_randwerte_und_monoton(void)
{
    uint8_t table[256];
    LEDHelper::calcGammaTable(2.8f, table);
    TEST_ASSERT_EQUAL_UINT8(0, table[0]);
    TEST_ASSERT_EQUAL_UINT8(255, table[255]);
    for (int i = 1; i < 256; i++)
        TEST_ASSERT_TRUE(table[i] >= table[i - 1]);
    TEST_ASSERT_TRUE(table[128] < 128);
}

// ------------------------------------------------------------ Zeitbasis

static void test_zeitbasis(void)
{
    TEST_ASSERT_EQUAL_UINT32(300, LEDHelper::getTimeWithPattern(3, TIMEBASE_TENTH_SECONDS));
    TEST_ASSERT_EQUAL_UINT32(5000, LEDHelper::getTimeWithPattern(5, TIMEBASE_SECONDS));
    TEST_ASSERT_EQUAL_UINT32(120000, LEDHelper::getTimeWithPattern(2, TIMEBASE_MINUTES));
    TEST_ASSERT_EQUAL_UINT32(7200000, LEDHelper::getTimeWithPattern(2, TIMEBASE_HOURS));
}

static void test_zeitbasis_stunden_begrenzt_ohne_ueberlauf(void)
{
    // mehr als 1000 h wird auf 1000 h begrenzt; 1000 h passen noch in 32 Bit
    TEST_ASSERT_EQUAL_UINT32(3600000000UL, LEDHelper::getTimeWithPattern(1001, TIMEBASE_HOURS));
    TEST_ASSERT_EQUAL_UINT32(3600000000UL, LEDHelper::getTimeWithPattern(65535, TIMEBASE_HOURS));
}

static void test_zeitbasis_unbekannt_ist_null(void)
{
    TEST_ASSERT_EQUAL_UINT32(0, LEDHelper::getTimeWithPattern(10, 9));
}

int main(int, char **)
{
    UNITY_BEGIN();
    RUN_TEST(test_kurven_von_aus_bis_voll);
    RUN_TEST(test_kurven_steigen_monoton);
    RUN_TEST(test_kurven_im_wertebereich);
    RUN_TEST(test_lineare_kurve_halbe_helligkeit);
    RUN_TEST(test_gammakurven_unter_linear);
    RUN_TEST(test_hsv_grundfarben);
    RUN_TEST(test_hsv_ohne_saettigung_ist_grau);
    RUN_TEST(test_rgb_grundfarben);
    RUN_TEST(test_kelvin_6600_ist_weiss);
    RUN_TEST(test_kelvin_warmweiss);
    RUN_TEST(test_kelvin_helligkeit_null_ist_aus);
    RUN_TEST(test_rgb_helligkeit_halbiert);
    RUN_TEST(test_gamma_eins_ist_identitaet);
    RUN_TEST(test_gamma_kurve_randwerte_und_monoton);
    RUN_TEST(test_zeitbasis);
    RUN_TEST(test_zeitbasis_stunden_begrenzt_ohne_ueberlauf);
    RUN_TEST(test_zeitbasis_unbekannt_ist_null);
    return UNITY_END();
}
