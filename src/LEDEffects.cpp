#include <Arduino.h>
#include "LEDEffects.h"

typedef uint32_t (*EffectFunction)();

LEDEffect::LEDEffect(uint16_t numLeds) : numLeds(numLeds), currentEffect(0), color(0) {
    effects[0] = &staticColor;
    effects[1] = &blinkingEffect;
    effects[2] = &breathingEffect;
    effects[3] = &colorWipeEffect;
    effects[4] = &rainbowEffect;
    effects[5] = &theaterChaseEffect;
    effects[6] = &strobeEffect;
    effects[7] = &sparkleEffect;
    effects[8] = &runningLightEffect;
}

void LEDEffect::setEffect(uint8_t effect) {
    if (effect < NUM_EFFECTS) {
        currentEffect = effect;
        _running = true;
    }
}

uint8_t LEDEffect::getEffect() {
    return currentEffect;
}

void LEDEffect::stopEffect() {
    _running = false;
    color = 0x000000;
}

void LEDEffect::update() {
    if (_running) {
        color = effects[currentEffect]();
    }
}

uint32_t LEDEffect::getColor() {
    return color;
}

uint32_t LEDEffect::staticColor() {
    return 0xFF0000; // Rot
}

uint32_t LEDEffect::blinkingEffect() {
    return (millis() / 500) % 2 == 0 ? 0x00FF00 : 0x000000; // Grün blinkend
}

uint32_t LEDEffect::breathingEffect() {
    uint8_t brightness = (sin(millis() / 1000.0 * PI) * 127) + 128;
    return (brightness << 16) | (brightness << 8) | brightness; // Weiß mit variierender Helligkeit
}

uint32_t LEDEffect::colorWipeEffect() {
    uint8_t step = (millis() / 200) % 3;
    switch (step) {
        case 0: return 0xFF0000; // Rot
        case 1: return 0x00FF00; // Grün
        case 2: return 0x0000FF; // Blau
        default: return 0x000000;
    }
}

uint32_t LEDEffect::rainbowEffect() {
    uint8_t pos = (millis() / 10) % 256;
    return ((255 - pos) << 16) | (pos << 8) | (128 - pos / 2); // Farbverlauf durch Regenbogenfarben
}

uint32_t LEDEffect::theaterChaseEffect() {
    return (millis() / 250) % 2 == 0 ? 0xFFFFFF : 0x000000; // Weiß blinkend wie ein Theaterlicht
}

uint32_t LEDEffect::strobeEffect() {
    return (millis() / 100) % 2 == 0 ? 0xFFFFFF : 0x000000; // Schnell blinkendes Weiß
}

uint32_t LEDEffect::sparkleEffect() {
    return random(10) > 7 ? 0xFFFFFF : 0x000000; // Zufällige weiße Blitze
}

uint32_t LEDEffect::runningLightEffect() {
    uint8_t pos = (millis() / 50) % 3;
    return pos == 0 ? 0xFF0000 : pos == 1 ? 0x00FF00 : 0x0000FF; // Rot-Grün-Blau laufend
}
