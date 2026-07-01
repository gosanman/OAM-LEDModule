# CLAUDE.md

## Embedded Constraints

This code runs on microcontrollers with severe resource limits — treat every byte and cycle as precious:

- **RAM:** RP2040 has 264 KB total (shared with stack, heap, lwIP, KNX stack). ESP32 has ~320 KB free heap typical. RP2350 and ESP32 can use PSRAM via helper macros (`HS_MALLOC`, `PS_DATA`, `PS_CODE`) when `OPENKNX_PSRAM`. No dynamic allocation in hot paths.
- **Flash:** Use `const` for read-only data — the linker places `.rodata` in flash automatically. `PROGMEM` does not exist on ESP32/RP2040. Avoid duplicating string literals. On RP2350/ESP32, use `PS_CODE` to move large functions to PSRAM and free flash.
- **No heap churn:** No `new`/`delete` or `std::string` construction in `loop()` — use fixed buffers, stack locals, or pre-allocated members.
- **No STL bloat:** Avoid `std::map`, `std::function`, `std::stringstream` — prefer arrays, raw function pointers, `snprintf`.
- **Stack depth:** RP2040 has a single stack (no RTOS by default). Keep recursion and large stack frames out of callbacks.
- **CPU:** Single-core RP2040 @ 125 MHz, no FPU on Cortex-M0+. Avoid `float` where integer math suffices.
- **When in doubt:** measure before adding, and prefer the smaller solution.

## Coding Conventions

- No `delay()` — everything non-blocking, state machines with `millis()`
- Platform guards: `#ifdef ARDUINO_ARCH_ESP32` / `#ifdef ARDUINO_ARCH_RP2040` / `#ifdef ARDUINO_ARCH_RP2350`
- **PSRAM:** For ESP32/RP2350, use `PSRAM_MALLOC`/`PSRAM_CALLOC`/`PSRAM_REALLOC` and `psram_new()` for dynamic allocation. Use `psram_delete()` as the counterpart to `psram_new()`. Use `PsramAllocator<T>` for STL containers. Use `PSRAM_DATA` for large static buffers and `PSRAM_CODE` for heavy functions. Controlled by `OPENKNX_PSRAM` (auto-detected, can disable with `OPENKNX_DISABLE_PSRAM`). Defined in `src/OpenKNX/helper.h`.
- No external libraries in `library.json` — only Arduino framework builtins
- Comments in German or English (mixed OK, but consistent per file)

## Formatting (Allman style, `.clang-format`)

- Always follow `.clang-format` exactly — it is the authoritative style definition
- Curly braces always on their own line (classes, functions, `if`, `else`, `for`, `while`, `case`, `enum`, `struct`, `namespace`, `extern`)
- No column limit — no artificial line breaks
- 4 spaces indentation, no tabs
- namespace content indented, case labels indented
- `if` without braces only for a single statement (`OnlyFirstIf`) — no single-line `else`
- Preprocessor directives not indented
- Short functions, lambdas, enums, and case labels may stay on one line (as per `AllowShort*` rules)
- When writing new code, match the style of the surrounding file exactly
