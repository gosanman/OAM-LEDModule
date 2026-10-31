#ifndef DIAGCORE1MAILBOX_H
#define DIAGCORE1MAILBOX_H

#include <Arduino.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

// Cross-Core-Briefkasten für Diagnose-KO-Meldungen.
//
// Hintergrund: openknx.console.writeDiagenoseKo() ruft intern knx.loop() auf. Der KNX-Stack ist
// nicht reentrant und wird von Core 0 getrieben - ein Aufruf aus loop1()/Core 1 wäre ein
// Data Race. Code auf Core 1 stellt seine Meldung daher hier ein (post()), Core 0 holt sie in
// loop() ab (take()) und gibt sie erst dann per writeDiagenoseKo() aus.
//
// SPSC-Ringpuffer: Core 1 ist alleiniger Producer (schreibt nur _head), Core 0 alleiniger
// Consumer (schreibt nur _tail). Deshalb ist er ohne Lock korrekt; die DMB-Barrieren ordnen
// Slot-Inhalt und Index. Mehrere Meldungen in einem Durchlauf (z.B. mehrere INA-Alert-Bits)
// gehen dadurch nicht mehr verloren - ein Single-Slot hätte nur die letzte behalten.
class DiagCore1Mailbox
{
  public:
    static constexpr uint8_t SLOTS = 8;  // Zweierpotenz -> Modulo wird zur Maske; 7 Meldungen nutzbar
    static constexpr uint8_t LEN = 16;   // DPT16: 14 Zeichen + NUL

    // Von Core 1 aufrufen STATT writeDiagenoseKo(): Meldung formatieren und einstellen.
    // Bei vollem Puffer wird die neueste verworfen - das Log (logErrorP/logInfoP) hat sie ohnehin.
    void post(const char *fmt, ...)
    {
        uint8_t head = _head;
        uint8_t next = (uint8_t)((head + 1) % SLOTS);
        if (next == _tail)
            return; // voll
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(_buf[head], LEN, fmt, ap);
        va_end(ap);
        __sync_synchronize(); // Slot-Inhalt vor dem Index sichtbar machen
        _head = next;
    }

    // Von Core 0 (loop()): nächste Meldung abholen, true wenn eine vorlag. Pro Durchlauf nur eine
    // abholen, damit writeDiagenoseKo() (samt knx.loop()) den Bus nicht in einem Rutsch flutet.
    bool take(char *out, uint8_t outLen)
    {
        uint8_t tail = _tail;
        if (tail == _head)
            return false; // leer
        strncpy(out, _buf[tail], outLen);
        out[outLen - 1] = '\0';
        __sync_synchronize(); // Slot fertig gelesen, bevor er freigegeben wird
        _tail = (uint8_t)((tail + 1) % SLOTS);
        return true;
    }

  private:
    char _buf[SLOTS][LEN] = {};
    volatile uint8_t _head = 0; // nur Core 1 schreibt
    volatile uint8_t _tail = 0; // nur Core 0 schreibt
};

#endif
