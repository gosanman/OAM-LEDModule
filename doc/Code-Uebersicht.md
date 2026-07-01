# OAM-LEDModule – Code-Übersicht

Zusammenfassung der Firmware-Architektur des OpenKNX LED-Dimmer-Moduls
(Repository `OAM-LEDModule`, App-Version LED `0.4.0`).

> Diese Datei beschreibt den Aufbau des Quellcodes unter `src/` und die
> Verzahnung der einzelnen Bausteine. Sie ist als Einstieg für Entwickler
> gedacht. Bilder und ETS-Definitionen liegen ebenfalls im Ordner `doc/`.

---

## 1. Was ist das Gerät?

Ein KNX-fähiger **LED-Konstantspannungs-Dimmer** (24 V) auf Basis eines
**RP2040** (Raspberry Pi Pico Familie), gebaut mit dem **OpenKNX**-Framework.
Die PWM-Ausgänge werden über einen **PCA9685** (16-Kanal-PWM, 12 Bit) erzeugt.
Optional sind ein **INA226/INA228** (Strom-/Spannungs-/Leistungsmessung), ein
**TMP100** (Temperatur) und ein **SSD1306**-OLED-Frontpanel bestückt.

### Hardware-Varianten (`include/hardware.h`)

| Board-Define | Gerät | HW-Kanäle | Frontpanel |
|---|---|---|---|
| `BOARD_KNXLED_DK_06_V10` | LED-DK-06 | 6 | nein |
| `BOARD_KNXLED_DK_12_V10` | LED-DK-12 | 12 | nein |
| `BOARD_KNXLED_DK_06_V12` | LED-DK-06 | 6 | ja |
| `BOARD_KNXLED_DK_12_V12` | LED-DK-12 | 12 | ja |
| `BOARD_KNXLED_APP_06_V01` | LED-APP-06 (REG1) | 6 | nein |

Die HW-Kanalzahl (`LED_HW_CHANNEL_COUNT`) steuert per Präprozessor die
maximalen Kanalzahlen je Typ:

| Konstante | 6-Kanal | 12-Kanal |
|---|---|---|
| `MAXCHANNELSHW` (Hardware-Ports) | 6 | 12 |
| `MAXCHANNELSEK` (Einzelkanal) | 6 | 12 |
| `MAXCHANNELSTW` (Tunable White) | 3 | 6 |
| `MAXCHANNELSRGB` (RGB) | 2 | 4 |
| `MAXCHANNELSHCL` (HCL-Kanäle) | 3 | 3 |

### I²C-Bus (Wire1, GPIO14 SDA / GPIO15 SCL, 100 kHz)

| Chip | Adresse | Funktion |
|---|---|---|
| PCA9685 | `0x40` | 12-Bit-PWM-Treiber für die LED-Ausgänge |
| INA226 / INA228 | `0x41` | Strom-/Spannungs-/Leistungs-/Energiemessung |
| TMP100 | `0x48` | Temperatursensor |
| SSD1306 | `0x3C` | OLED-Frontpanel 128×32 |

---

## 2. Modulstruktur (OpenKNX-Module)

Der Einstieg ist [`src/main.cpp`](../src/main.cpp). Dort wird der I²C-Bus
initialisiert und es werden vier OpenKNX-Module registriert:

```cpp
openknx.addModule(1, openknxLEDModule);          // LED-Dimmer
openknx.addModule(2, openknxMeasuringModule);    // Messung
openknx.addModule(8, openknxFrontPanelModule);   // OLED-Frontpanel
openknx.addModule(9, openknxFileTransferModule); // Datei-Transfer (extern)
```

Der RP2040 hat zwei Kerne. `setup()`/`loop()` laufen auf Kern 0 (KNX-Stack),
`setup1()`/`loop1()` auf Kern 1. Zeitkritische Aufgaben (PWM-Ansteuerung,
I²C, Display) laufen bewusst in `loop1()`.

```
                    ┌───────────────────────────────────────────┐
                    │                 main.cpp                    │
                    │      Kern 0: loop()   Kern 1: loop1()       │
                    └───────────────────────────────────────────┘
                       │            │             │
       ┌───────────────┘            │             └──────────────┐
       ▼                            ▼                            ▼
┌──────────────┐         ┌───────────────────┐        ┌──────────────────┐
│  LEDModule   │         │  MeasuringModule  │        │ FrontPanelModule │
│  (Dimmer)    │◄────────│  (INA/TMP)        │        │  (SSD1306)       │
└──────┬───────┘  save   └─────────┬─────────┘        └────────┬─────────┘
       │          Power            │                           │
       │                           │   liest Messwerte + Status│
       ▼                           └───────────────────────────┘
┌─────────────────────────────────────────┐
│ DimChannel_EK / _TW / _RGB   +  HclChannel│
│ (Kanal-Objekte, KNX-Logik, Dimm-Rampen)   │
└──────────────────┬────────────────────────┘
                   ▼
         setHwChannelValue() → PCA9685 (PWM)
```

---

## 3. LEDModule – der Dimmer-Kern

Dateien: [`src/LEDModule.h`](../src/LEDModule.h),
[`src/LEDModule.cpp`](../src/LEDModule.cpp)

Zentrale Verantwortlichkeiten:

- **Betriebsart wählen** (`setup()`): Aus den ETS-Parametern
  `ParamAPP_ControllerType`, `ParamAPP_OperatingMode` und
  `ParamAPP_PwmFrequenz` wird bestimmt, wie die HW-Ports auf logische Kanäle
  aufgeteilt werden. Es gibt 6 Betriebsarten (`operatinModeSelect` 0–5):

  | Modus | 6-Kanal-Board | 12-Kanal-Board |
  |---|---|---|
  | 0 | 6× EK | 12× EK |
  | 1 | 3× TW | 6× TW |
  | 2 | 2× TW + 2× EK | 4× TW + 4× EK |
  | 3 | 2× RGB | 4× RGB |
  | 4 | 1× RGB + 1× TW + 1× EK | 2× RGB + 2× TW + 2× EK |
  | 5 | 1× RGB + 3× EK | 1× RGB + 4× TW + 1× EK |

  Für jeden logischen Kanal wird ein passendes Kanal-Objekt (`DimChannel_EK`,
  `_TW`, `_RGB`) angelegt und über `setup(hwchannel)` mit den zugehörigen
  PCA9685-Ports verbunden.

- **PWM-Hardware-Abstraktion**: Die einzige Stelle, die tatsächlich in den
  PCA9685 schreibt:
  - `setHwChannelValue(channel, value, curve)` → `_pwm.setPin(channel, curves[value][curve])`
  - `setHwChannelValuePWM(channel, start, end, curve)` → `_pwm.setPWM(...)`
    (für die phasenverschobene TW-Ansteuerung)

- **KNX-Eingang verteilen** (`processInputKo`): Anhand des KO-Index
  (`asap()`) wird auf Basis der Offsets/Blockgrößen
  (`EK_KoOffset`, `TW_KoOffset`, `RGB_KoOffset` …) der richtige Kanal
  bestimmt und `channel->processInputKo(ko)` aufgerufen. Zusätzlich wird das
  globale **Tag/Nacht-Objekt** (`APP_KoDayNight`) behandelt und per
  `setDayNight()` an alle Kanäle verteilt.

- **HCL-Verteilung** (`loop()`): Alle 60 s (`HCL_TIMER_BROADCAST`) berechnet
  jeder `HclChannel` seine Werte und verteilt Kelvin/Helligkeit per
  `channel->setHcl(...)` an die zugeordneten Dimmkanäle.

- **I²C-Überwachung** (`loop1()`): Alle 10 s (`LED_CHECK_I2C`) prüft
  `checkI2cConnection()` MODE1/MODE2-Register des PCA9685 und initialisiert
  den Chip bei Verlust neu (`initI2cConnection()`). Nur bei gültiger
  Verbindung wird `channel->task()` (Dimm-Rampen) ausgeführt.

- **Not-Aus / Stromsparen**: `processBeforeRestart()` bzw. `savePower()`
  setzen per ALL_LED_OFF-Register alle Ausgänge sofort ab. `savePower()` wird
  vom MeasuringModule bei Hardware-Alarmen (Überstrom o. Ä.) aufgerufen.

- **Funktionstaste** (nur mit `FUNC1_BUTTON_PIN`): Kurz/Lang/Doppelklick
  schalten per `handleFunc1()` alle Kanäle an/aus/um.

- **Konsolen-Kommandos** (`showHelp` / `processCommand`): `chon`/`choff`/
  `chval` (Ports direkt setzen), `init_pca`, `state_pca`, `test_pwm`,
  `show con` (Anschlussplan), `i2c` (Bus-Scan).

- **Function Properties** (`processFunctionProperty`): ETS-„Kanal schalten"
  über ObjectIndex 160 / PropertyId 6.

---

## 4. Dimmkanäle – Klassenhierarchie

Dateien: `DimChannel.*`, `DimChannel_EK.*`, `DimChannel_TW.*`,
`DimChannel_RGB.*`

Polymorphe Basisklasse mit drei Spezialisierungen (alle erben von
`OpenKNX::Channel`):

```
DimChannel (abstrakte Basis)
├── DimChannel_EK   Einzelkanal – nur Helligkeit          (1 HW-Port)
├── DimChannel_TW   Tunable White – Helligkeit + Kelvin   (2 HW-Ports: WW/KW)
└── DimChannel_RGB  RGB-Farbe – RGB & HSV                 (3 HW-Ports: R/G/B)
```

**Gemeinsames Schnittstellen-Muster** (`DimChannel`):
`name()`, `getHWPorts()`, `getChannelIndex()`, `getChannelType()`,
`setup(hwchannel)`, `processInputKo(ko)`, `task()`, `setDayNight(value)`,
`setHcl(channel, kelvin, brightness)`.

**Ablauf einer Dimmung** (bei allen Typen gleich):
1. KNX-Telegramm trifft ein → `processInputKo()` dekodiert den DPT.
2. Ziel wird gesetzt (`_newValue…`) und ein Zustand der Dimm-State-Machine
   (`_currentTask`) aktiviert.
3. `task()` → `dimmerTask()` führt die State-Machine zyklisch aus.
4. `handleDimGeneric(...)` interpoliert Schritt für Schritt (ms pro Stufe aus
   Dauer/Delta).
5. `sendDimValue()` wendet Kurve/Gamma an und schreibt über das LEDModule in
   den PCA9685.
6. Am Ziel sendet `updateDimValue()` die Status-KOs (An/Aus, Helligkeit,
   Farbe …).

### 4.1 DimChannel_EK (Einzelkanal)

- Ein PWM-Ausgang, reine Helligkeit (0–255).
- Merkt sich Tag-/Nachtwert (`_lastDayValue`, `_lastNightValue`).
- KOs (Block ab `EK_KoOffset`): Schalten, absolut dimmen (`DPT_Percent_U8`),
  relativ dimmen (`DPT 3.7`), Szene (`DPT_SceneNumber`), Status An/Aus,
  Status Helligkeit.
- State-Machine: `EK_DIM_SOFT_ON/OFF`, `EK_DIM_B_SET/UP/DOWN`, `STOP`, `IDLE`.

### 4.2 DimChannel_TW (Tunable White)

- Zwei Ausgänge Warmweiß (WW) + Kaltweiß (KW); Endpunkt-Temperaturen aus
  `ParamTW_ColorTempWW`/`…KW`.
- Zustand hält Helligkeit **und** Kelvin (`_currentValueTW[2]`).
- Besonderheit **phasenverschobene PWM** in `sendDimValue()`: WW und KW werden
  im 4096-Takt-Zyklus zeitlich versetzt geschaltet (KW startet nach WW-Ende).
  Das reduziert Flackern, Stromspitzen und EMV-Probleme.
- KOs (ab `TW_KoOffset`): Schalten, Helligkeit abs./rel., Kelvin abs./rel.
  (`DPT 7.600`), Szene, Status An/Aus, Helligkeit, Kelvin.
- Szenen-Aktionen: nur Helligkeit, nur Kelvin, beides, aus.

### 4.3 DimChannel_RGB (Farbe)

- Drei Ausgänge (R/G/B), RGB- und HSV-Bedienung.
- **Proportionale Überblendung**: Beim Farbwechsel werden alle drei Kanäle so
  gesteuert, dass sie gleichzeitig am Ziel ankommen (Zwischenfarben werden
  vermieden). Realisiert über `_dimIncrement[]`/`_dimAcc[]`.
- **Gammakorrektur**: `sendDimValue()` schreibt `gammaT[wert]` (Standard
  Gamma 2.8, per `ParamRGB_GammaCorrection` einstellbar → `calcGammaTable()`).
- Viele KOs (ab `RGB_KoOffset`): 24-Bit-RGB, 24-Bit-HSV, Einzel-R/G/B
  (`DPT_Scaling`), Einzel-H (`DPT_Angle`)/S/V, jeweils relativ, Szene sowie
  umfangreiche Status-KOs (RGB, HSV, Einzelwerte).

---

## 5. HCL – Human Centric Lighting

Dateien: [`src/HclChannel.h`](../src/HclChannel.h),
[`src/HclChannel.cpp`](../src/HclChannel.cpp)

Berechnet tageszeitabhängig **Farbtemperatur** und **Helligkeit** und gibt sie
an die zugeordneten Dimmkanäle. Es gibt bis zu `MAXCHANNELSHCL` (3) HCL-Kanäle.

Zwei Betriebsarten:
- **Sonnenstand** (`PT_hclType_sun`): Fenster von Sonnenauf- bis
  -untergang (aus `openknx.sun`), verschiebbar per Offset.
- **Zeittabelle** (`PT_hclType_time`): festes Start-/Endfenster (auch über
  Mitternacht).

Innerhalb des Fensters liefert `getCircadianValue()` ein asymmetrisch
exponentielles Profil (morgens schneller Anstieg, abends sanfteres Abfallen)
zwischen den Grenzwerten `ParamHCL_colorTempMin/Max` bzw. `briMin/Max`.
Ergebnis wird per Status-KOs gemeldet und über `LEDModule::loop()` zyklisch an
die Dimmkanäle verteilt.

---

## 6. LEDHelper – Farb- und Kurven-Utilities

Dateien: [`src/LEDHelper.h`](../src/LEDHelper.h),
[`src/LEDHelper.cpp`](../src/LEDHelper.cpp)

Statische Hilfsklasse, von allen Kanaltypen genutzt:

- **Farbraum-Konvertierung**: `hsvToRGB()`, `rgbToHSV()`,
  `kelvinToRGB()` (Farbtemperatur → RGB), `adjustRGBBrightness()`.
- **Gammatabelle**: `calcGammaTable(gamma)` füllt `gammaT[256]`.
- **Zeitberechnung**: `getTimeWithPattern(time, base)` → Millisekunden
  (Zehntel/Sek./Min./Std.).
- **Dimmkurven** `curves[256][5]` (12-Bit, `PROGMEM`): die zentrale
  Wert-→-PWM-Tabelle mit 5 Kurven:

  | Index | Kurve |
  |---|---|
  | 0 (A) | linear |
  | 1 (B) | Gamma 2.8 |
  | 2 (C) | Gamma 3.8 |
  | 3 (D) | CIE 1931 |
  | 4 (E) | DALI |

  (siehe auch `doc/DimmKurven.png`, `doc/Kelvin.png`).

---

## 7. MeasuringModule – Strom, Spannung, Leistung, Temperatur

Dateien: [`src/MeasuringModule.h`](../src/MeasuringModule.h),
[`src/MeasuringModule.cpp`](../src/MeasuringModule.cpp),
[`src/LIB_INA.*`](../src/LIB_INA.h), [`src/LIB_TEMP100.*`](../src/LIB_TEMP100.h)

Überwacht die Versorgung und meldet Messwerte/Alarme an KNX (Version `0.4.0`).

- **Sensoren**: `LIB_INA` unterstützt **INA226** und **INA228** und erkennt den
  Chip automatisch (`detectChipType()` über DIE-/DEVICE-ID). `LIB_TEMP100`
  bedient den optionalen **TMP100**.
- **Messgrößen**: Busspannung (V), Strom (A), Leistung (W), kumulierte Energie
  (Wh), Temperatur (°C). Der Shunt-Wert (`ParamAPP_ShuntValue`, 10 mΩ→8 A oder
  5 mΩ→16 A) bestimmt den Messbereich.
- **Energie**: INA228 nutzt das Hardware-Energieregister; beim INA226 wird
  Energie per V·I·t integriert. Der Zähler wird im Flash persistiert
  (`readFlash()`/`writeFlash()`, 9 Byte).
- **Timing** (`loop1()`):
  - alle 5 s: Messwerte lesen + Alarmschwellen prüfen,
  - alle 60 s (konfigurierbar): Messwerte auf KNX senden,
  - alle 6 s: Hardware-Alert-Flags des INA prüfen,
  - alle 10 s: I²C-Verbindung prüfen/neu aufsetzen.
- **KNX-KOs**: `KoAPP_VoltageV`, `KoAPP_CurrentA`, `KoAPP_PowerW`,
  `KoAPP_ActivePowerWh`, `KoAPP_TempC` (gerundet je nach DPT). Alarme:
  `APP_KoAlarmOverTemp`, `…OverVoltage`, `…UnderVoltage`, `…OverCurrent`.
- **Schutz**: Bei Hardware-Alerts (Über-/Unterspannung, Überstrom,
  Übertemperatur) ruft `getAlertValues()` `openknxLEDModule.savePower()` auf.
- **Konsole**: `temp`, `voltage`, `current`, `power`, `energy`, `all`,
  `ccenergy` (Energiezähler löschen), `set <wert>`.

---

## 8. FrontPanelModule – OLED-Bedienoberfläche

Dateien: [`src/FrontPanelModule.h`](../src/FrontPanelModule.h),
[`src/FrontPanelModule.cpp`](../src/FrontPanelModule.cpp)

Steuert das **SSD1306**-OLED (128×32) und vier Taster (nur bei
`FRONT_PANEL_PRESENT`, Version V1.2). Version `0.4.0`.

- **Taster** (IO1–IO4, `INPUT_PULLUP`, 300 ms Entprellung):
  BACK (■, zurück zur Info), LEFT (◀), RIGHT (▶), SELECT (⬤, auswählen/
  Kanal schalten).
- **Bildschirme** (`currentscreen`): Info (Logo/Gerät/PA), Status
  (I²C-Zustand Temp/Power/Dimmer), Messung (V/A/W), Temperatur, Spannung,
  Strom, Leistung, Energie, Datum/Uhrzeit/Uptime sowie Unterseite
  „Anschlüsse" (Kanal→HW-Port-Zuordnung, RGB/WW-KW/EK).
- **Ablauf** (`loop1()`, alle 250 ms Update): Start-Animation (OpenKNX-Logo),
  Timeout-Abschaltung (`ParamAPP_DisplayTimeOut`), blinkende **Warnseite** bei
  I²C-Fehler eines der drei kritischen Sensoren.
- **Steuerung**: Bei aktivem `ParamAPP_FrontPanelControl` schaltet SELECT auf
  der Anschluss-Seite den Kanal per `openknxLEDModule.toggleChannelHWPort()`.
- Das Modul definiert **keine eigenen KNX-KOs**; es liest nur Werte/Status aus
  LEDModule und MeasuringModule (`getMeasurementValue()`,
  `getPcaI2cConnectionState()`, `getInaI2cConnectionState()`,
  `getTempI2cConnectionState()`).

---

## 9. Abhängigkeiten & Build

- **Framework/Bibliotheken** (`platformio.custom.ini`, `dependencies.txt`):
  OpenKNX `knx`, `OGM-Common`, `OFM-FileTransferModule`, Adafruit BusIO,
  Adafruit PWM Servo Driver (PCA9685), Adafruit SSD1306. Siehe
  [`include/versions.h`](../include/versions.h).
- **Plattform**: RP2040, Dual-Core (`OPENKNX_DUALCORE`).
- **Build-Umgebungen**: pro Board je `develop_…_USB`, `develop_…_JLINK` und
  `release_…` (siehe `platformio.custom.ini`). ETS-Produktdaten in
  `include/knxprod.h` (generiert via `scripts/Build-knxprod.ps1`).
- **Symlinks** unter `lib/` zeigen auf die Nachbar-Repos `knx`, `OGM-Common`,
  `OFM-FileTransferModule` (zusätzliche Arbeitsverzeichnisse des Workspace).

---

## 10. Verzeichnis-Landkarte (`src/`)

| Datei | Inhalt |
|---|---|
| `main.cpp` | Einstieg, Modulregistrierung, Dual-Core-Loops |
| `LEDModule.*` | Dimmer-Kern: Betriebsart, PWM-HW, KO-Verteilung, I²C |
| `DimChannel.*` | Abstrakte Basisklasse der Kanäle |
| `DimChannel_EK.*` | Einzelkanal (Helligkeit) |
| `DimChannel_TW.*` | Tunable White (WW/KW, phasenversetzte PWM) |
| `DimChannel_RGB.*` | RGB/HSV, proportionale Überblendung, Gamma |
| `HclChannel.*` | Human Centric Lighting (Circadian-Profil) |
| `LEDHelper.*` | Farbkonvertierung, Dimmkurven, Zeit-Utilities |
| `MeasuringModule.*` | Messung & Alarme |
| `LIB_INA.*` | Treiber INA226/INA228 |
| `LIB_TEMP100.*` | Treiber TMP100 |
| `FrontPanelModule.*` | OLED-Frontpanel & Taster |

---

*Erstellt als Code-Überblick; Detailtiefe auf Architektur-/Einstiegsebene.
Für exakte Registerwerte, KO-Nummern und Parameter siehe den jeweiligen
Quellcode und `include/knxprod.h`.*
