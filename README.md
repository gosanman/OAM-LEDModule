# OpenKNX LED-Modul (OAM-LEDModule)

KNX-fähiger **LED-Konstantspannungs-Dimmer** (24 V) auf Basis eines **RP2040**,
gebaut mit dem [OpenKNX](https://github.com/OpenKNX)-Framework. Die PWM-Ausgänge
werden über einen **PCA9685** (16-Kanal, 12 Bit) erzeugt. Optional sind Strom-/
Leistungsmessung (**INA226/INA228**), ein Temperatursensor (**TMP100**) und ein
**SSD1306**-OLED-Frontpanel bestückt.

> Ausführliche Beschreibung der Firmware-Architektur:
> [doc/Code-Uebersicht.md](doc/Code-Uebersicht.md)

## Funktionen

- **6 Betriebsarten** mit frei gemischten Kanaltypen:
  - **EK** – Einzelkanal (nur Helligkeit)
  - **TW** – Tunable White (Warm-/Kaltweiß, Helligkeit + Farbtemperatur)
  - **RGB** – Farbe über RGB und HSV
- Softdimmen mit **5 Dimmkurven** (linear, Gamma 2.8, Gamma 3.8, CIE, DALI)
- **Szenen**, relatives/absolutes Dimmen, Tag-/Nacht-Umschaltung
- **Human Centric Lighting (HCL)** – tageszeitabhängige Farbtemperatur und
  Helligkeit (Sonnenstand- oder Zeittabellen-basiert)
- **Messung** von Spannung, Strom, Leistung, Energie und Temperatur mit
  KNX-Meldung und Alarmschwellen (inkl. Schutzabschaltung bei Überstrom)
- Optionales **OLED-Frontpanel** mit Menü, Statusanzeige und manueller Steuerung
- Phasenverschobene PWM bei Tunable White (flacker- und EMV-optimiert)

## Hardware-Varianten

| Board-Define | Gerät | HW-Kanäle | Frontpanel |
|---|---|---|---|
| `BOARD_KNXLED_DK_06_V10` | LED-DK-06 | 6 | nein |
| `BOARD_KNXLED_DK_12_V10` | LED-DK-12 | 12 | nein |
| `BOARD_KNXLED_DK_06_V12` | LED-DK-06 | 6 | ja |
| `BOARD_KNXLED_DK_12_V12` | LED-DK-12 | 12 | ja |
| `BOARD_KNXLED_APP_06_V01` | LED-APP-06 (REG1) | 6 | nein |

I²C-Peripherie (Wire1): PCA9685 `0x40`, INA226/228 `0x41`, TMP100 `0x48`,
SSD1306 `0x3C`.

## Software-Aufbau

Vier OpenKNX-Module (registriert in [`src/main.cpp`](src/main.cpp)):

| Modul | Aufgabe |
|---|---|
| `LEDModule` | Dimmer-Kern: Betriebsart, PWM-Ausgabe, KNX-Verteilung, I²C |
| `MeasuringModule` | Messung von V/A/W/Wh/°C, Alarme, Schutzabschaltung |
| `FrontPanelModule` | OLED-Anzeige und Taster |
| `FileTransferModule` | Datei-Transfer (externes OpenKNX-Modul) |

Der RP2040 arbeitet mit zwei Kernen: `loop()` bedient den KNX-Stack (Kern 0),
`loop1()` die zeitkritische Hardware – PWM, I²C, Display (Kern 1).

Details siehe [doc/Code-Uebersicht.md](doc/Code-Uebersicht.md).

## Bauen (PlatformIO)

Das Projekt nutzt PlatformIO. Die Build-Umgebungen sind in
[`platformio.custom.ini`](platformio.custom.ini) definiert – je Board eine
Variante für USB-Upload, J-Link-Debug und Release, z. B.:

```bash
# Beispiel: 6-Kanal-Board V1.2 über USB flashen
pio run -e develop_LEDDK_06_USB_V12 -t upload
```

Abhängigkeiten (`knx`, `OGM-Common`, `OFM-FileTransferModule`, Adafruit-Libs)
werden über die `lib/`-Symlinks bzw. `lib_deps` eingebunden; siehe
[`dependencies.txt`](dependencies.txt) und [`include/versions.h`](include/versions.h).

Hilfsskripte für Restore, ETS-Produktdaten und Release liegen unter
[`scripts/`](scripts/) und [`restore/`](restore/).

## Diagnose (Konsole)

Über die OpenKNX-Konsole verfügbare Kommandos (Auswahl):

- `chon <ch>` / `choff <ch>` / `chval <ch> <0-4095>` – HW-Port direkt setzen
- `show con` – Anschlussplan (Kanal → HW-Port)
- `i2c` – I²C-Bus scannen
- `voltage` / `current` / `power` / `energy` / `temp` / `all` – Messwerte
- `ccenergy` – Energiezähler zurücksetzen

## Dokumentation

- [doc/Code-Uebersicht.md](doc/Code-Uebersicht.md) – Firmware-Architektur
- `doc/` – Dimmkurven, Kelvin-Diagramm, ETS-App-Screenshots, Definitionen

## Lizenz

Siehe [LICENSE](LICENSE).
