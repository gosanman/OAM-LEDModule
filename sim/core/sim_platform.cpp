// ============================================================================
// Simulator: Plattformfunktionen, die lib/knx/src/knx/bits.cpp erwartet.
// Die KNX-Bibliothek gibt darueber Hex-Dumps aus; im Simulator verworfen, damit
// stdout nur JSON-Ereignisse traegt.
// ============================================================================

#include "bits.h"

void print(const char[]) {}
void print(char) {}
void print(unsigned char, int) {}
void print(int, int) {}
void print(unsigned int, int) {}
void print(long, int) {}
void print(unsigned long, int) {}
void print(long long, int) {}
void print(unsigned long long, int) {}
void print(double) {}
void println(const char[]) {}
void println(char) {}
void println(unsigned char, int) {}
void println(int, int) {}
void println(unsigned int, int) {}
void println(long, int) {}
void println(unsigned long, int) {}
void println(long long, int) {}
void println(unsigned long long, int) {}
void println(double) {}
void println(void) {}
