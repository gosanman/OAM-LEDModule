"""Selbsttest des LED-Simulators: typische Abläufe ohne Oberfläche.

Prüft, dass der Kern den echten Firmware-Code ausführt und Ausgänge,
Rückmeldungen und Schutzfunktionen so liefert, wie die
Applikationsbeschreibung es beschreibt. Läuft nach dem Bauen:

    python sim/selftest.py
"""

import json
import os
import sys

from ledcore import Core, ParamImage, load_meta

HERE = os.path.dirname(os.path.abspath(__file__))
FAILS = []

# Objektnummern: Block je Kanal 28 Objekte, HCL 2 (siehe include/knxprod.h)
EK, TW, RGB, HCL = 57, 393, 561, 673


def ek(n, off=0):
    return EK + (n - 1) * 28 + off


def tw(n, off=0):
    return TW + (n - 1) * 28 + off


def rgb(n, off=0):
    return RGB + (n - 1) * 28 + off


def check(label, cond, detail=''):
    print(('  ok    ' if cond else '  FEHLER ') + label + (f'  [{detail}]' if detail != '' else ''))
    if not cond:
        FAILS.append(label)


def project(name):
    with open(os.path.join(HERE, 'projects', name + '.json'), encoding='utf-8') as f:
        return json.load(f)


class Device:
    """Ein Kern mit Projekt, wie in der App: Last setzen, laden, Bus ein."""

    def __init__(self, meta, name, values=None):
        p = project(name)
        self.events = []
        self.core = Core(p['board'], on_event=self.events.append)
        img = ParamImage(meta)
        img.load_values(dict(p['values'], **(values or {})))
        hw = p.get('hw', {})
        for i, w in enumerate(hw.get('portW', [])):
            self.core.cmd(f'portw {i} {w}')
        self.core.cmd('time 1793349000')      # 2026-10-30 10:30 Ortszeit
        self.core.cmd('load ' + img.device_hex())
        self.core.cmd('bus 1')
        self.core.cmd('run 1000')

    def cmd(self, line):
        return self.core.cmd(line)

    def sim(self):
        return self.core.state()['sim']

    def duty(self, port):
        return self.sim()['duty'][port]

    def sent(self, nr):
        """Letzter gesendeter Rohwert eines Objekts (Hex) oder None."""
        for ev in reversed(self.events):
            if ev.get('ev') == 'ko' and ev['nr'] == nr:
                return ev['raw']
        return None

    def kinds(self, kind):
        return [e for e in self.events if e.get('ev') == kind]

    def close(self):
        self.core.close()


def t_ek(meta):
    print('1. Einzelkanal: schalten, absolut und relativ dimmen, Rückmeldung')
    d = Device(meta, 'DK-12 V1.2 - 4x TW + 4x EK')
    s = d.sim()
    check('8 Kanäle, PCA verbunden', len(s['channels']) == 8 and s['led']['pcaConn'], len(s['channels']))
    port = [c for c in s['channels'] if c['type'] == 'EK' and c['no'] == 1][0]['ports'][0]
    d.cmd(f'ko {ek(1)} 1.1 1')
    d.cmd('run 1500')
    check('EK1 ein: Port voll an', d.duty(port) > 0.99, d.duty(port))
    check('Status An/Aus = 1', d.sent(ek(1, 16)) == '01', d.sent(ek(1, 16)))
    check('Status Helligkeit = 100 %', d.sent(ek(1, 17)) == 'ff', d.sent(ek(1, 17)))
    d.cmd(f'ko {ek(1, 2)} 5.1 50')
    d.cmd('run 2000')
    check('EK1 50 %: Rückmeldung 50 %', d.sent(ek(1, 17)) in ('7f', '80'), d.sent(ek(1, 17)))
    check('EK1 50 %: Tastgrad nach Dimmkurve unter 50 %', 0.05 < d.duty(port) < 0.5, d.duty(port))
    d.cmd(f'ko {ek(1, 10)} 3.7 1')         # dunkler starten
    d.cmd('run 600')
    d.cmd(f'ko {ek(1, 10)} 3.7 0')         # Stopp
    d.cmd('run 500')
    b = int(d.sent(ek(1, 17)), 16)
    check('relativ dunkler und Stopp: zwischen 0 und 50 %', 0 < b < 127, f'{b * 100 / 255:.0f} %')
    d.cmd(f'ko {ek(1)} 1.1 0')
    d.cmd('run 1500')
    check('EK1 aus: Port aus, Status 0', d.duty(port) == 0 and d.sent(ek(1, 16)) == '00', d.duty(port))
    d.close()


def t_tw(meta):
    print('2. Tunable White: Farbtemperatur und Helligkeit')
    d = Device(meta, 'DK-12 V1.2 - 4x TW + 4x EK')
    c = [c for c in d.sim()['channels'] if c['type'] == 'TW' and c['no'] == 1][0]
    ww, cw = c['ports']
    d.cmd(f'ko {tw(1)} 1.1 1')
    d.cmd('run 1500')
    d.cmd(f'ko {tw(1, 3)} 7.600 2700')
    d.cmd('run 2000')
    a, b = d.duty(ww), d.duty(cw)
    check('2700 K: überwiegend Warmweiß', a > b > 0, f'WW {a:.2f} KW {b:.2f}')
    check('Status Farbtemperatur 2700 K', d.sent(tw(1, 18)) == '0a8c', d.sent(tw(1, 18)))
    d.cmd(f'ko {tw(1, 3)} 7.600 6500')
    d.cmd('run 8000')                        # Uebergang mit der Dimmzeit absolut
    check('6500 K: nur Kaltweiß', d.duty(cw) > 0.95 and d.duty(ww) < 0.02, f'WW {d.duty(ww):.2f} KW {d.duty(cw):.2f}')
    d.cmd(f'ko {tw(1, 2)} 5.1 20')
    d.cmd('run 2000')
    check('20 %: Rückmeldung 20 %', d.sent(tw(1, 17)) == '33', d.sent(tw(1, 17)))
    d.close()


def t_rgb_hcl(meta):
    print('3. RGB und HCL (DK-6 mit RGB + TW + EK)')
    d = Device(meta, 'DK-6 V1.2 - RGB + TW (HCL) + EK')
    s = d.sim()
    chans = {(c['type'], c['no']): c for c in s['channels']}
    check('RGB1, TW1, EK1 angelegt', set(chans) == {('RGB', 1), ('TW', 1), ('EK', 1)}, sorted(chans))
    r, g, b = chans[('RGB', 1)]['ports']
    d.cmd(f'ko {rgb(1, 2)} 232.600 0xFF0000')
    d.cmd('run 2000')
    check('Farbe rot: nur der R-Port', d.duty(r) > 0.9 and d.duty(g) == 0 and d.duty(b) == 0,
          [d.duty(p) for p in (r, g, b)])
    check('Status RGB = FF0000', d.sent(rgb(1, 17)) == 'ff0000', d.sent(rgb(1, 17)))
    d.cmd(f'ko {rgb(1, 6)} 5.1 50')         # Helligkeit V 50 %
    d.cmd('run 2000')
    check('V 50 %: rot bleibt, dunkler', 0 < d.duty(r) < 0.9 and d.duty(g) == 0, d.duty(r))
    check('HCL1 eingerichtet', any(h['no'] == 1 and h['configured'] for h in d.sim()['hcl']), d.sim()['hcl'])
    d.cmd('run 61000')                       # HCL rechnet einmal je Minute (HCL_TIMER_BROADCAST)
    d.cmd(f'ko {tw(1)} 1.1 1')
    d.cmd('run 3000')
    t = [c for c in d.sim()['channels'] if c['type'] == 'TW'][0]
    check('TW1 mit HCL: Farbtemperatur zwischen 2000 und 5000 K', t['hcl'] and 2000 <= t['k'] <= 5000, f"{t['k']} K, hcl {t['hcl']}")
    d.close()


def t_overcurrent(meta):
    print('4. Überstrom: Abschaltung, Latch, Reaktivierung')
    d = Device(meta, 'DK-12 V1.2 - 4x TW + 4x EK')
    port = [c for c in d.sim()['channels'] if c['type'] == 'EK' and c['no'] == 1][0]['ports'][0]
    d.cmd(f'ko {ek(1)} 1.1 1')
    d.cmd('run 1500')
    d.cmd(f'short {port} 1')
    d.cmd('run 1500')
    s = d.sim()
    check('Kurzschluss: Latch gesetzt', s['led']['powerFault'], s['meas'])
    check('Kurzschluss: alle Ausgänge aus', max(s['duty']) == 0, s['duty'])
    check('kein Wächter-Alarm (Abschaltung < 1,5 s)', not d.kinds('warn'), [w['text'] for w in d.kinds('warn')][:2])
    d.cmd(f'ko {ek(1, 2)} 5.1 80')
    d.cmd('run 1000')
    check('im Latch bleibt der Ausgang aus', d.duty(port) == 0, d.duty(port))
    d.cmd(f'short {port} 0')
    d.cmd('run 6000')
    d.cmd('cmd resetfault')
    d.cmd('run 3000')
    s = d.sim()
    check('nach resetfault: Latch gelöst, Ausgang wieder an', not s['led']['powerFault'] and d.duty(port) > 0, d.duty(port))
    d.close()


def t_bus(meta):
    print('5. Busspannung: kurzer Einbruch und Wiederkehr nach Ausfall')
    d = Device(meta, 'DK-12 V1.2 - 4x TW + 4x EK')
    port = [c for c in d.sim()['channels'] if c['type'] == 'EK' and c['no'] == 2][0]['ports'][0]
    d.cmd(f'ko {ek(2, 2)} 5.1 60')
    d.cmd('run 2000')
    before = d.duty(port)
    d.cmd('busdip 200')
    d.cmd('run 2000')
    dip = [e for e in d.kinds('busdip') if e['phase'] == 'end']
    check('Busdip 200 ms: kein Neustart', dip and not dip[-1]['reboot'], dip[-1] if dip else '')
    check('Busdip: Helligkeit wieder wie vorher', abs(d.duty(port) - before) < 0.01, f'{before:.3f} -> {d.duty(port):.3f}')
    d.cmd('bus 0')
    d.cmd('run 1000')
    d.cmd('bus 1')
    d.cmd('run 3000')
    check('nach Busausfall: Neustart mit Parametern', d.kinds('boot')[-1]['configured'], '')
    d.close()


def t_func1_program(meta):
    print('6. Funktionstaste, ETS-Download und Funktionstest')
    d = Device(meta, 'DK-12 V1.2 - 4x TW + 4x EK')
    d.cmd('btn short')                       # Projekt: Klick = alle umschalten
    d.cmd('run 500')
    s = d.sim()
    check('Func1 kurz: alle Ausgänge an', min(s['duty']) > 0.99, s['duty'])
    check('Info-LED 1 an', s['leds'][1][0] == 1, s['leds'])
    d.cmd('btn short')
    d.cmd('run 500')
    check('Func1 nochmal: alle aus', max(d.sim()['duty']) == 0, d.sim()['duty'])
    img = ParamImage(meta)
    img.load_values(dict(project('DK-12 V1.2 - 4x TW + 4x EK')['values'], EK_1UseOnValue=1, EK_1OnBrightness=40))
    d.cmd('pimg ' + img.device_hex())
    d.cmd('ets 3000')
    port = [c for c in d.sim()['channels'] if c['type'] == 'EK' and c['no'] == 1][0]['ports'][0]
    d.cmd(f'ko {ek(1)} 1.1 1')
    d.cmd('run 2000')
    check('nach Download: Einschalthelligkeit 40 %', d.sent(ek(1, 17)) == '66', d.sent(ek(1, 17)))
    d.cmd('fp 160 6 010b01')                 # Funktionstest Port L ein
    d.cmd('run 100')
    check('ETS-Funktionstest Port L ein', d.duty(11) > 0.99, d.duty(11))
    d.close()


def t_app_board(meta):
    print('7. LED-APP-6x24V: 6 Einzelkanäle, ohne Bedienfeld')
    d = Device(meta, 'APP-6 - 6x EK')
    s = d.sim()
    check('6 Einzelkanäle', len(s['channels']) == 6 and all(c['type'] == 'EK' for c in s['channels']), len(s['channels']))
    check('kein Bedienfeld gemeldet', not d.core.hello['panel'], d.core.hello)
    d.cmd(f'ko {ek(6)} 1.1 1')
    d.cmd('run 1500')
    p = s['channels'][5]['ports'][0]
    check('EK6 ein', d.duty(p) > 0.99, d.duty(p))
    d.cmd('unload')
    d.cmd('run 500')
    check('nach Entladen: keine Kanäle', not d.sim()['channels'], '')
    d.close()


def t_wrong_board(meta):
    print('8. Falsche Platine: ETS-Auswahl passt nicht zur gebauten Firmware')
    d = Device(meta, 'DK-12 V1.2 - 4x TW + 4x EK', dict(APP_Hardware=3))   # gebaut DK-12 V1.2, gewaehlt DK-6 V1.2
    s = d.sim()
    check('Abweichung erkannt, keine Kanäle angelegt', s['led'].get('hwMismatch') and not s['channels'], s['led'])
    check('Fehler im Log', any('passt nicht zur Firmware' in e['text'] for e in d.kinds('log')), '')
    d.cmd(f'ko {ek(1)} 1.1 1')               # Kanal-KO
    d.cmd('btn short')                       # Funktionstaste (Projekt: alle umschalten)
    d.cmd('cmd chon 0')                      # Konsole
    d.cmd('cmd test walk')                   # Testmodus
    d.cmd('fp 160 6 010b01')                 # ETS-Funktionstest Port L
    d.cmd('run 2000')
    check('kein Ausgang an (KO, Funktionstaste, Konsole, Test, Funktionstest)', max(d.sim()['duty']) == 0, d.sim()['duty'])
    d.close()
    d = Device(meta, 'DK-12 V1.2 - 4x TW + 4x EK', dict(APP_Hardware=0))   # keine Platine gewaehlt
    check('ohne Geräteauswahl: ebenfalls gesperrt', d.sim()['led'].get('hwMismatch') and not d.sim()['channels'], d.sim()['led'])
    d.close()
    d = Device(meta, 'APP-6 - 6x EK', dict(APP_Hardware=4))   # LED-APP gebaut, DK-12 V1.2 gewaehlt
    d.cmd('run 3000')
    check('LED-APP: Fehler-LED (Info2) bleibt an', d.sim()['leds'][2][0] == 1, d.sim()['leds'])
    d.close()
    d = Device(meta, 'APP-6 - 6x EK')   # richtige Platine, alle Bausteine da
    d.cmd('run 3000')
    check('LED-APP richtig gewaehlt: Fehler-LED aus', d.sim()['leds'][2][0] == 0, d.sim()['leds'])
    d.close()


def main():
    meta = load_meta()
    for t in (t_ek, t_tw, t_rgb_hcl, t_overcurrent, t_bus, t_func1_program, t_app_board, t_wrong_board):
        try:
            t(meta)
        except Exception as e:   # ein abgestuerzter Test soll die anderen nicht verhindern
            check(f'{t.__name__} ohne Ausnahme', False, repr(e))
    print()
    print('Alles in Ordnung.' if not FAILS else f'{len(FAILS)} Fehler: ' + '; '.join(FAILS))
    sys.exit(1 if FAILS else 0)


if __name__ == '__main__':
    main()
