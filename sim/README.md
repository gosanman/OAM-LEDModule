# LED-Simulator

Simuliert den LED-Dimmer am Schreibtisch: Parameter wie in der ETS setzen,
Eingangsobjekte senden, die zwölf (bzw. sechs) PWM-Ausgänge, das Licht, die
Messwerte und das Display beobachten. Ohne Platine, ohne Bus, ohne ETS.

**Es läuft der echte Firmware-Code.** Der Simulator rechnet die Logik nicht nach.
Er übersetzt die unveränderten Quellen aus `src/` (ohne `main.cpp`) samt den
Adafruit-Bibliotheken für den PC und ersetzt nur, was die Firmware von außen
aufruft: Parameterspeicher, Gruppenobjekte, Logger, Flash, LEDs,
Funktionstaste, Uhrzeit/Sonne und den I²C-Bus. Am I²C-Bus hängen Modelle der
Bausteine auf Registerebene: PCA9685 (PWM), INA226 (Strom/Spannung), TMP100
(Temperatur) und SSD1306 (Display). Was hier falsch läuft, läuft auch im Gerät
falsch.

## Starten

Einmalig bzw. nach jeder Änderung an Firmware oder ETS-Applikation:

```
OpenKNXproducer create --Debug -h include/knxprod.h src/LEDModule   (Task "OpenKNXproducer")
powershell -NoProfile -File sim/build.ps1                                       (Task "Simulator bauen")
```

`build.ps1` braucht MinGW-GCC (`winget install BrechtSanders.WinLibs.POSIX.UCRT`)
und die Adafruit-Bibliotheken aus den PlatformIO-`libdeps` (einmal irgendeine
Firmware mit PlatformIO bauen). Es baut je Hardware-Variante einen Kern
`sim/build/ledsim_<Board>.exe` und liest die ETS-Applikation aus
`src/LEDModule.debug.xml` nach `sim/build/meta.json`; Bilder, Icons und
Hilfetexte kommen aus `src/LEDModule.knxprod` nach `sim/build/baggage/`. Mit `-Board DK_12_V12`
nur eine Variante.

Dann:

```
~/.platformio/penv/Scripts/python.exe sim/ledsim.py                  (Task "Simulator starten")
```

Die Oberfläche öffnet sich unter <http://127.0.0.1:8766>. Optionen: `--port`,
`--no-browser`, `--project "Name"`. Der aktive Reiter steht in der Adresse, etwa
`#params` oder `#hw`, und bleibt beim Neuladen erhalten. Python-Pakete braucht es
keine.

## Hardware-Varianten

| Board | Gerät | Ausgänge | Bedienfeld |
|---|---|---|---|
| `DK_06_V10` | LED-DK-6x24V, Hardware V1.0 | 6 | – |
| `DK_06_V12` | LED-DK-6x24V, Hardware V1.2 | 6 | ja |
| `DK_12_V10` | LED-DK-12x24V, Hardware V1.0 | 12 | – |
| `DK_12_V12` | LED-DK-12x24V, Hardware V1.2 | 12 | ja |
| `APP_06_V01` | LED-APP-6x24V (Reg1) | 6 | – |

Die Auswahl in der Kopfleiste startet den Kern der anderen Variante; die
Parameter bleiben, die ETS-Geräteauswahl (`APP_Hardware`) wird passend gesetzt.

## Bedienung

| Bereich | Was er tut |
|---|---|
| Kopfleiste | Zeit (anhalten, 1× bis 600×, Einzelschritte), Ortszeit des Geräts, KNX-Bus, LED-Versorgung, Busdip, Programmieren, Neustart, Entladen, die vier LEDs (Prog, Info 1–3), Funktionstaste kurz/lang/doppelt, Hardware-Variante, Projekte |
| Bedienung | Ein Unterreiter je Kanal (EK, TW, RGB, HCL) und für Gerät/OpenKNX, oder „Alle“. Oben die Leuchte mit Ports, Zustand und Kennlinie, darunter alle Eingangsobjekte mit passendem Feld: Schalten, Schieber für 5.001/5.003, ▲ ■ ▼ und ±25 % für 3.007, Kelvin für 7.600, Farbwähler für RGB (232.600), H/S/V-Felder für HSV, Szenenabruf. Zahlenwerte lassen sich zyklisch senden. Darunter die Ausgangsobjekte mit ihrem letzten Wert |
| Parameter | ETS-Ansicht, siehe unten. Änderungen werden erst mit **Programmieren** wirksam, mit Download und Neustart wie am Gerät |
| Objekte | Alle Objekte, die die ETS mit der programmierten Parametrierung einblendet |
| Telegramme | Jedes gesendete und empfangene Telegramm mit Zeit, DPT, Rohwert und Wert |
| Konsole | Dieselben Befehle wie an der seriellen Konsole (`help`, `show con`, `i2c`, `resetfault`, `test walk`, `all` …), wahlweise über das Diagnose-KO |
| Hardware | Versorgungsspannung, Shunt, Umgebungstemperatur, Ausfall einzelner I²C-Bausteine, Ortszeit und „Uhrzeit gültig“, physikalische Adresse, Busdip-Dauer, je Port Last in Watt und Kurzschluss, ETS-Funktionstest je Port, Kanaltest der Firmware |
| Prüfung | Wächter und Hinweise, siehe unten |
| rechte Spalte | Leuchten (Farbe und Helligkeit aus den Tastgraden), Ports A…L mit Tastgrad, Rolle und Strom, Verlauf der Tastgrade, Messung (Firmware-Wert und echter Wert), Alarme und „Fehler zurücksetzen“, Bedienfeld mit Display und vier Tasten |

Werte zeigt der Simulator nach dem DPT der ETS an, dekodiert aus dem
Rohtelegramm. Sendet die Firmware ein Objekt mit einem anderen Untertyp
(etwa Helligkeit als 5.004 statt 5.001), steht im Reiter Telegramme beides.

Projekte liegen in `sim/projects/` als JSON mit Hardware-Variante, den
geänderten Parametern und den Hardware-Einstellungen. Mitgeliefert:

- *DK-12 V1.2 - 4x TW + 4x EK* – mit Kanalnamen, Bedienfeld, Überstromalarm, Funktionstaste
- *DK-6 V1.2 - RGB + TW (HCL) + EK* – HCL 1 nach Sonnenauf-/-untergang auf TW 1
- *APP-6 - 6x EK*
- *Neues Gerät (Vorgaben)*

## ETS-Ansicht der Parameter

Der Reiter **Parameter** zeigt die Applikation so wie die ETS 6 (Vorbild: der
[OpenKNX Viewer](https://tools.openknx.de/viewer/)), gebaut aus derselben
Quelle wie die knxprod:

- Kanalbaum mit den Icons der Applikation, Unterseiten, Seitentitel mit Kanalnamen (`{{0:...}}`)
- Sichtbarkeit über `choose`/`when` wie in der ETS, `Assign` (z. B. Geräteklasse aus der Geräteauswahl)
- Raster-Layouts (`Layout="Grid"`, Zeilen/Spalten mit Breiten), Überschriften, Linien, Hinweis- und Fehlerboxen, Einrückung
- Eingaben nach Parametertyp: Optionsfelder bei höchstens zwei Werten (sonst Liste, `UIHint="DropDown"` erzwingt die Liste),
  Zahlen mit Bereichsprüfung und Einheit, Gleitkomma mit Komma, Zeit `hh:mm`, Text, Farbe, Kontrollkästchen;
  `Access="Read"` gesperrt, `Access="None"` ausgeblendet
- Bilder der Applikation (Anschlusspläne, Dimmkurven, HCL-Verlauf)
- **Kontexthilfe** aus den Hilfetexten der knxprod: zur Seite, und zum angeklickten Parameter
- **Schaltflächen** führen das ETS-Skript der Applikation aus. Der Funktionstest unter
  LED › Funktionstests schaltet so wirklich den gewählten Port – über dieselbe Function
  Property wie aus der ETS
- „Änderungen hervorheben“ (gelb, auch im Baum), orange Marke = noch nicht programmiert,
  „Standardwerte“, „Verwerfen“, „breit“ für die ganze Fensterbreite

## Physik

Einfach gehalten, nur so genau, wie die Firmware es braucht:

- **Last:** je Port eine Leistung bei 100 % an 24 V; der Strom ist Tastgrad × Leistung ÷ 24 V.
  Kurzschluss: 10 A bei 100 %. Dazu 15 mA Eigenbedarf.
- **Versorgung:** Leerlaufspannung (Vorgabe 24 V) mit 50 mΩ Innenwiderstand.
- **INA226:** Register wie im Datenblatt, Mittelung und Wandlungszeit aus dem
  Konfigurationsregister, Shunt- und Busspannung aus Strom und Spannung, Alert-Flag
  (AFF) mit Latch bis zum Lesen von MASK/ENABLE.
- **Temperatur:** erster Ordnung, 0,25 K/W über der Umgebung, Zeitkonstante 2 min.
- **PCA9685:** Einschaltzustand, Sleep/Restart, Vorteiler, Auto-Increment,
  ALL_LED-Register, FULL_ON/FULL_OFF, Ausgangsinvertierung.
- **SSD1306:** Befehle mit ihrer Parameterzahl, Spalten-/Seitenadressierung, 128×32 Pixel.

## Was der Simulator prüft

Nach jedem Schritt:

| Prüfung | Meldet |
|---|---|
| Überstrom | Strom über 6,5 A länger als 1,5 s, ohne dass die Firmware abschaltet |
| Latch | Ein Ausgang leuchtet, obwohl der Überstrom-Latch gesetzt ist |
| DPT | Die Firmware liest oder sendet ein Objekt mit einem DPT, den die KNX-Bibliothek nicht kodieren kann. Am Gerät passiert das stumm |
| I²C | Zugriffe ohne Antwort (NACK), solange alle Bausteine bestückt sind |

`sim/selftest.py` spielt typische Abläufe ohne Oberfläche durch (Task
„Simulator-Selbsttest“): Schalten und Dimmen mit Rückmeldung, Tunable White,
RGB und HCL, Überstrom mit Latch und Reaktivierung, Busdip und Busausfall,
Funktionstaste, ETS-Download und Funktionstest, LED-APP, falsche oder fehlende
Geräteauswahl (keine Kanäle, kein Ausgang). Ein rotes Ergebnis
heißt: Die Firmware weicht von der Erwartung ab.

## Wie es gebaut ist

```
Browser (web/)  ⇄  ledsim.py (HTTP, Zeit, Projekte)  ⇄  build/ledsim_<Board>.exe (stdin/stdout, JSON-Zeilen)
                                                         ├─ Firmware: src/*.cpp ohne main.cpp, unverändert
                                                         ├─ Adafruit PWM Servo Driver, BusIO, GFX, SSD1306
                                                         ├─ DPT-Kodierung: lib/knx/src/knx (dpt, dptconvert, knx_value)
                                                         └─ sim/core: OpenKNX-, Arduino- und Wire-Ersatz, Bausteinmodelle
```

| Datei | Inhalt |
|---|---|
| `core/Arduino.h`, `Wire.h`, `SPI.h` | Arduino-Ersatz; `Wire` reicht jede Transaktion an die Bausteinmodelle |
| `core/OpenKNX.h` | Ersatz für OpenKNX und die KNX-Stack-Schnittstelle, soweit die Module sie benutzen |
| `core/sim_runtime.cpp` | Parameterspeicher, Gruppenobjekte (Verhalten wie `group_object.cpp`), Logger, Konsole, Flash, Uhrzeit, Sonnenstand |
| `core/sim_hw.cpp` | PCA9685, INA226, TMP100, SSD1306, Last, Versorgung, Temperatur, Bedienfeldtasten |
| `core/sim_inspect.cpp` | Zustand für die Anzeige, Wächter |
| `core/sim_main.cpp` | Befehlsschleife, Start, ETS-Download, Bus-Ausfall und Busdip |
| `gen_meta.py` | Liest `src/LEDModule.debug.xml`: Vorgabe-Parameterabbild, Parameter, Objekte, ETS-Seiten und -Baum, ETS-Skript; entpackt die Baggages der knxprod |
| `ledcore.py` | Python-Anbindung an den Kern, auch für eigene Testskripte |
| `ledsim.py` | Die App: HTTP-Server, Simulationszeit, Projekte |
| `selftest.py` | Selbsttest |
| `web/` | Oberfläche, ohne Fremdbibliotheken; `ets.js`/`ets.css` ist die ETS-Ansicht der Parameter |

Der Kern lässt sich auch direkt mit einer Befehlsdatei füttern, etwa für
Regressionstests: `build/ledsim_DK_12_V12.exe < szenario.txt`. Die Befehle
stehen im Kopf von `core/sim_main.cpp`.

Läuft die App und sperrt die exe, baut man den Kern unter anderem Namen und
gibt ihn per Umgebungsvariable vor:

```
powershell -NoProfile -File sim/build.ps1 -Board DK_12_V12 -Suffix _test
$env:LEDSIM_EXE = "sim\build\ledsim_DK_12_V12_test.exe"
```

## Grenzen

- **Module:** LED, Messung und Bedienfeld laufen mit. Dateiübertragung,
  Konfigurationstransfer und die OpenKNX-Grundfunktionen (Heartbeat,
  Startverzögerung, Uhrzeit über die Objekte) nicht; die Uhrzeit stellt der Reiter Hardware.
- **Zwei Kerne nacheinander:** `loop()` und `loop1()` laufen abwechselnd in
  Schritten von 1 ms. Echte Nebenläufigkeit (Data Races zwischen Kern 0 und 1)
  zeigt der Simulator nicht.
- **Licht:** Die Leuchten-Anzeige rechnet Farbe und Helligkeit aus den Tastgraden;
  LED-Streifen, Netzteil-Strombegrenzung und Wärme der Streifen sind nicht modelliert.
- **HCL:** Die Firmware rechnet HCL einmal je Minute. Nach dem Start dauert es
  daher bis zu 60 s, bis HCL gilt – auch am Gerät.
- **Lesezugriffe der Private-Felder:** `sim_inspect.cpp` öffnet sie für die
  Anzeige per `#define private public`. Das ist ein Kniff für Testwerkzeuge; der
  Firmware-Code bleibt unverändert.
- **ETS-Ansicht:** nachgebildet ist, was die LED-Applikation nutzt (siehe oben). Der
  Kanal „Konfigurationstransfer“ fehlt, er betrifft nur die ETS. Von der ETS-Skript-API
  gibt es `device.getParameterByName`, `online.invokeFunctionProperty` und
  `progress.setText`; Schaltflächen, die mehr brauchen, melden einen Fehler.
