# CLAUDE.md

OpenKNX LED dimmer: RP2040, PCA9685 PWM driver, INA226/228 power sensor, TMP100 temperature sensor, optional SSD1306 front panel. Single-repo form: the module code lives directly in `src/`, there is no separate OFM repo. The ETS application is built with the OpenKNXproducer from `src/LEDModule.xml`.

## Boards and board selection

- Pins, I²C addresses and the board ID live only in `include/hardware.h`, never in module code.
- `LED_BOARD_ID` must match the ETS parameter `APP_Hardware` (`PT-Hardware` in `src/LED.share.xml`). IDs are never reused; a new board gets the next free ID, a new enum entry, an entry in `BOARD_HW` in `sim/ledsim.py` and a simulator project.
- If the ETS selection does not match the firmware, `LEDModule::setup()` creates no channels and every direct output path is locked. Any new code that drives outputs directly must check `isHardwareMismatch()` and `isPowerFault()`.
- Hardware-bound parameters (board, shunt, sensor and front panel fitted) carry `op:configTransfer="never"`.

## ETS application

- Changing parameters or object flags needs a new application version: `src/LEDModule.conf.xml` and `library.json`, old version added to `ReplacesVersions`.
- The KO layout and the parameter memory layout are frozen: only append or reuse free bits, and comment it in the XML.
- Help texts come from `doc/Applikationsbeschreibung-LED.md` (task "Hilfetexte erzeugen"); update both together when behaviour changes.
- An OpenKNXproducer run is only done when it ends without warnings.

## Embedded Constraints

This code runs on microcontrollers with severe resource limits — treat every byte and cycle as precious:

- **RAM:** RP2040 has 264 KB total (shared with stack, heap, lwIP, KNX stack). No dynamic allocation and no `std::string`/`String` construction in `loop()`/`loop1()`.
- **Flash:** Use `const` for read-only data — the linker places `.rodata` in flash automatically. `PROGMEM` does not exist on RP2040.
- **No STL bloat:** Avoid `std::map`, `std::function`, `std::stringstream` — prefer arrays, raw function pointers, `snprintf`.
- **Dual core:** `OPENKNX_DUALCORE`. Output updates run on core 1 (`loop1`); code on core 0 sets requests or flags instead of writing to the PCA9685 (exceptions: `setup()` and the all-off paths). `writeDiagenoseKo()` only on core 0 — core 1 posts to the `DiagCore1Mailbox`.
- **CPU:** No FPU on Cortex-M0+. Avoid `float` where integer math suffices.
- **When in doubt:** measure before adding, and prefer the smaller solution.

## Coding Conventions

- No `delay()` — everything non-blocking, state machines with `millis()`.
- Lifecycle: `setup()` writes all outputs off first; `processBeforeRestart()`, `savePower()` and over-current trips switch all outputs off; `restorePower()` only requests a resend.
- No external libraries in `library.json` — only Arduino framework builtins.
- Comments in German or English (mixed OK, but consistent per file).

## Formatting (Allman style, `.clang-format`)

- Always follow `.clang-format` exactly — it is the authoritative style definition.
- Curly braces always on their own line (classes, functions, `if`, `else`, `for`, `while`, `case`, `enum`, `struct`, `namespace`, `extern`).
- No column limit — no artificial line breaks. 4 spaces indentation, no tabs.
- Namespace content and case labels indented; preprocessor directives not indented.
- `if` without braces only for a single statement (`OnlyFirstIf`) — no single-line `else`.
- Short functions, lambdas, enums and case labels may stay on one line (`AllowShort*`).
- When writing new code, match the style of the surrounding file exactly.

## Simulator

- `sim/` compiles the unchanged firmware for the PC (port 8766, see `sim/README.md`).
- Every firmware bug found by the simulator or a review gets a case in `sim/selftest.py` that fails before the fix and passes after it.
- Hardware-independent calculations (`LEDHelper`: dim curves, colour conversion, time base) have native tests in `test/` (`pio test -e native`, task "native Tests"); `test/native_shim` replaces `Arduino.h`/`OpenKNX.h` there. New pure logic goes there too.
- Before calling work done: OpenKNXproducer without warnings, firmware build without warnings, native tests green, simulator selftest "Alles in Ordnung".
