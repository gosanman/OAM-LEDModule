# ============================================================================
# Simulator bauen: je Hardware-Variante ein Kern (ledsim_<Board>.exe) und die
# Metadaten aus der ETS-Applikation (meta.json).
#
# Voraussetzung: include/knxprod.h und src/LEDModule.debug.xml existieren, also
#   OpenKNXproducer create --Debug -h include/knxprod.h src/LEDModule   (Task "OpenKNXproducer")
# ist gelaufen, und die Adafruit-Bibliotheken liegen in libdeps (einmal eine
# Firmware mit PlatformIO bauen).
#
# Aufruf aus dem Projektordner:  powershell -File sim/build.ps1 [-Board DK_12_V12]
# ============================================================================
param(
    # Nur eine Variante bauen, z. B. DK_12_V12. Ohne Angabe alle.
    [string]$Board = '',
    # Anderer Dateiname-Zusatz, z. B. wenn die App laeuft und die exe sperrt
    [string]$Suffix = ''
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$sim = $PSScriptRoot
$out = Join-Path $sim 'build'
New-Item -ItemType Directory -Force $out | Out-Null

foreach ($f in @('include/knxprod.h', 'src/LEDModule.debug.xml')) {
    if (-not (Test-Path (Join-Path $root $f))) {
        throw "$f fehlt - zuerst OpenKNXproducer laufen lassen (siehe Kopf dieser Datei)."
    }
}

# --- Compiler: MinGW-GCC aus winget, sonst g++ im PATH ----------------------
$gpp = Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages" -Recurse -Filter g++.exe -ErrorAction SilentlyContinue |
    Select-Object -First 1 -ExpandProperty FullName
if (-not $gpp) { $gpp = (Get-Command g++ -ErrorAction SilentlyContinue).Source }
if (-not $gpp) { throw 'Kein g++ gefunden (winget install BrechtSanders.WinLibs.POSIX.UCRT).' }

# --- Adafruit-Bibliotheken aus den PlatformIO-libdeps ------------------------
$libdepsRoots = @($env:PLATFORMIO_LIBDEPS_DIR, 'E:\PIO\libdeps', (Join-Path $root '.pio\libdeps')) | Where-Object { $_ -and (Test-Path $_) }
$libdeps = $null
foreach ($r in $libdepsRoots) {
    $hit = Get-ChildItem $r -Directory -ErrorAction SilentlyContinue |
        Where-Object { Test-Path (Join-Path $_.FullName 'Adafruit SSD1306') } | Select-Object -First 1
    if ($hit) { $libdeps = $hit.FullName; break }
}
if (-not $libdeps) { throw 'Adafruit-Bibliotheken nicht gefunden - einmal eine Firmware mit PlatformIO bauen.' }
$pwmLib = Join-Path $libdeps 'Adafruit PWM Servo Driver Library'
$busLib = Join-Path $libdeps 'Adafruit BusIO'
$gfxLib = Join-Path $libdeps 'Adafruit GFX Library'
$oledLib = Join-Path $libdeps 'Adafruit SSD1306'

$src = Join-Path $root 'src'
$knx = Join-Path $root 'lib/knx/src/knx'

# Firmware-Quellen - UNVERAENDERT. main.cpp fehlt bewusst: an seiner Stelle
# steht sim/core/sim_main.cpp.
$fwSources = Get-ChildItem $src -Filter *.cpp | Where-Object { $_.Name -ne 'main.cpp' } | ForEach-Object FullName
$libSources = @(
    (Join-Path $pwmLib 'Adafruit_PWMServoDriver.cpp'),
    (Join-Path $busLib 'Adafruit_I2CDevice.cpp'),
    (Join-Path $gfxLib 'Adafruit_GFX.cpp'),
    (Join-Path $oledLib 'Adafruit_SSD1306.cpp')
)
$knxSources = @('dpt', 'dptconvert', 'knx_value', 'bits') | ForEach-Object { Join-Path $knx "$_.cpp" }
$simSources = Get-ChildItem (Join-Path $sim 'core') -Filter *.cpp | ForEach-Object FullName

$boards = @('DK_06_V10', 'DK_12_V10', 'DK_06_V12', 'DK_12_V12', 'APP_06_V01')
if ($Board) { $boards = @($Board) }

Write-Host "g++:     $gpp"
Write-Host "libdeps: $libdeps"
foreach ($b in $boards) {
    $define = "BOARD_KNXLED_$b"
    $exe = Join-Path $out "ledsim_$b$Suffix.exe"
    $gppArgs = @('-std=gnu++17', '-O2', '-static', '-Wall', '-Wno-unused-function', '-Wno-unused-variable',
        '-Wno-sign-compare', '-Wno-unused-but-set-variable', '-Wno-format-truncation', '-Wno-stringop-truncation',
        # DimChannel::setup(uint8_t*) verdeckt Base::setup(bool) - so gewollt
        '-Wno-overloaded-virtual',
        '-DARDUINO=10819', "-D$define", '-DOPENKNX_DUALCORE', '-DLED_SIMULATOR', '-D_POSIX_THREAD_SAFE_FUNCTIONS', '-DSSD1306_NO_SPLASH',
        # sim/core zuerst: Arduino.h, Wire.h, SPI.h, OpenKNX.h, Adafruit_SPIDevice.h des Simulators
        "-I$(Join-Path $sim 'core')", "-I$(Join-Path $root 'include')", "-I$src", "-I$knx",
        "-I$pwmLib", "-I$busLib", "-I$gfxLib", "-I$oledLib",
        '-o', $exe) + $simSources + $fwSources + $libSources + $knxSources
    Write-Host "Kern $b ..."
    & $gpp @gppArgs
    if ($LASTEXITCODE -ne 0) { throw "Uebersetzen fehlgeschlagen fuer $b ($LASTEXITCODE)" }
    Write-Host "  gebaut: $exe"
}

# --- Metadaten aus der ETS-Applikation ----------------------------------------
$py = Join-Path $env:USERPROFILE '.platformio\penv\Scripts\python.exe'
if (-not (Test-Path $py)) { $py = 'python' }
& $py (Join-Path $sim 'gen_meta.py') (Join-Path $root 'src/LEDModule.debug.xml') (Join-Path $out 'meta.json')
if ($LASTEXITCODE -ne 0) { throw "Metadaten fehlgeschlagen ($LASTEXITCODE)" }
