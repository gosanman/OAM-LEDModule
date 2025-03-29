#ifndef LEDEFFECT_H
#define LEDEFFECT_H

#include <Arduino.h>

typedef uint32_t (*EffectFunction)();

class LEDEffect {
public:
    LEDEffect(uint16_t numLeds);
    void setEffect(uint8_t effect);
    uint8_t getEffect();
    void stopEffect();
    void update();
    uint32_t getColor();

private:
    static const uint8_t NUM_EFFECTS = 9;
    uint16_t numLeds;
    uint8_t currentEffect;
    uint32_t color;
    EffectFunction effects[NUM_EFFECTS];

    bool _running = false;

    static uint32_t staticColor();
    static uint32_t blinkingEffect();
    static uint32_t breathingEffect();
    static uint32_t colorWipeEffect();
    static uint32_t rainbowEffect();
    static uint32_t theaterChaseEffect();
    static uint32_t strobeEffect();
    static uint32_t sparkleEffect();
    static uint32_t runningLightEffect();
};

#endif
