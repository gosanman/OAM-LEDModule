#pragma once

// Ersatz fuer Arduino.h in der nativen Testumgebung (pio test -e native).
// Nur das, was die hardwareunabhaengigen Quellen (LEDHelper.cpp) brauchen.

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

using std::max;
using std::min;
