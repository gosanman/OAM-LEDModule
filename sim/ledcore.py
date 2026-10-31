"""Python-Anbindung an den Simulatorkern (build/ledsim_<Board>.exe).

Zwei Bausteine, beide ohne Fremdpakete:

  ParamImage  der ETS-Parameterspeicher: Werte per Parametername setzen, das
              Abbild wird genau so kodiert wie von der ETS (Big Endian, Bitlage
              ab MSB, DPT 9 fuer die Alarmgrenzen, RGB als 3 Byte). Grundlage
              sind die Vorgabewerte aus meta.json.
  Core        startet den Kern einer Hardware-Variante, schickt Befehle,
              sammelt die Ereignisse.

Beispiel (ohne Oberflaeche, etwa fuer einen Regressionstest):

    meta = load_meta()
    img = ParamImage(meta)
    img.set('APP_Hardware', 4)                    # LED-DK-12x24V V1.2
    img.set('APP_OperatingModeCh12', 10)          # 12x EK
    core = Core('DK_12_V12')
    core.cmd('load ' + img.device_hex())          # mit den Assign-Werten der ETS
    core.cmd('bus 1')
    core.cmd('ko 57 1.1 1')                       # EK1 ein
    core.cmd('run 2000')
    print(core.state()['sim']['duty'])
"""

import json
import os
import struct
import subprocess
import threading

HERE = os.path.dirname(os.path.abspath(__file__))
BUILD = os.path.join(HERE, 'build')
BOARDS = {
    'DK_06_V10': 'LED-DK-6x24V, Hardware V1.0',
    'DK_06_V12': 'LED-DK-6x24V, Hardware V1.2 (Bedienfeld)',
    'DK_12_V10': 'LED-DK-12x24V, Hardware V1.0',
    'DK_12_V12': 'LED-DK-12x24V, Hardware V1.2 (Bedienfeld)',
    'APP_06_V01': 'LED-APP-6x24V (Reg1)',
}


def load_meta(path=None):
    with open(path or os.path.join(BUILD, 'meta.json'), encoding='utf-8') as f:
        return json.load(f)


def dpt9(v):
    m = int(round(float(v) * 100))
    e = 0
    while m < -2048 or m > 2047:
        m = int(round(m / 2))
        e += 1
    return (0x8000 if m < 0 else 0) | (e << 11) | (m & 0x7FF)


class ParamImage:
    def __init__(self, meta):
        self.meta = meta
        self.params = meta['params']
        self.image = bytearray.fromhex(meta['defaults'])
        self.values = {}   # nur geaenderte Werte: Name -> Wert

    def default(self, name):
        return self.params[name]['d']

    def get(self, name):
        return self.values.get(name, self.params[name]['d'])

    def set(self, name, value, clamp=True):
        p = self.params[name]
        if p['k'] in ('text', 'color'):
            value = str(value)
            if p['k'] == 'color' and not value.startswith('#'):
                value = '#' + value
            if p['k'] == 'color':
                value = value.upper()
        elif p['k'] == 'float':
            value = float(value)
            if clamp and 'min' in p:
                value = max(p['min'], min(p['max'], value))
        else:
            value = int(float(value))
            if clamp and 'min' in p and p['k'] in ('uint', 'int'):
                value = max(int(p['min']), min(int(p['max']), value))
        default = p['d'].upper() if p['k'] == 'color' else p['d']
        if value == default:
            self.values.pop(name, None)
        else:
            self.values[name] = value
        self._encode(p, value)

    def reset(self):
        self.image = bytearray.fromhex(self.meta['defaults'])
        self.values = {}

    def load_values(self, values):
        self.reset()
        for name, value in values.items():
            if name in self.params:
                self.set(name, value)

    def hex(self):
        return self.image.hex()

    def device_hex(self):
        """Abbild, wie es die ETS herunterlaedt: mit den Werten der <Assign>-
        Ziele (z.B. APP_OperatingMode aus der sichtbaren Betriebsmodus-Auswahl)."""
        img = ParamImage(self.meta)
        img.load_values(self.values)
        for name, value in assigned_values(self.meta, self.values).items():
            img.set(name, value)
        return img.hex()

    def _encode(self, p, value):
        if p['o'] is None:
            return          # reiner ETS-Wert (Kanalname, Betriebsmodus-Auswahl): nicht im Geraet
        o, b, s = p['o'], p['b'], p['s']
        if p['k'] == 'text':
            raw = value.encode('latin-1', 'replace')[: s // 8]
            self.image[o: o + s // 8] = raw.ljust(s // 8, b'\0')
            return
        if p['k'] == 'color':
            self.image[o: o + 3] = int(value.lstrip('#') or '0', 16).to_bytes(3, 'big')
            return
        if p['k'] == 'float':
            if p.get('enc') == 'dpt9':
                self.image[o: o + 2] = dpt9(value).to_bytes(2, 'big')
            else:
                self.image[o: o + 4] = struct.pack('>f', value)
            return
        v = value & ((1 << s) - 1)
        for i in range(s):
            pos = o * 8 + b + i
            mask = 0x80 >> (pos % 8)
            if (v >> (s - 1 - i)) & 1:
                self.image[pos // 8] |= mask
            else:
                self.image[pos // 8] &= ~mask & 0xFF


def exe_for(board):
    exe = os.environ.get('LEDSIM_EXE')
    if exe:
        return exe
    return os.path.join(BUILD, f'ledsim_{board}.exe')


class Core:
    """Ein laufender Simulatorkern. Thread-sicher fuer einen Aufrufer je Befehl."""

    def __init__(self, board='DK_12_V12', on_event=None):
        exe = exe_for(board)
        if not os.path.exists(exe):
            raise FileNotFoundError(f'{exe} fehlt - zuerst sim/build.ps1 ausfuehren')
        self.board = board
        self.proc = subprocess.Popen([exe], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                     stderr=subprocess.STDOUT, text=True, encoding='utf-8',
                                     errors='replace', bufsize=1)
        self.lock = threading.Lock()
        self.on_event = on_event
        self.hello = self._read_until_ok(first=True)[-1]

    def _read_until_ok(self, first=False):
        events = []
        while True:
            line = self.proc.stdout.readline()
            if not line:
                raise RuntimeError('Simulatorkern beendet')
            line = line.strip()
            if not line:
                continue
            try:
                ev = json.loads(line)
            except json.JSONDecodeError:
                ev = {'ev': 'raw', 'text': line}
            if ev.get('ev') == 'ok':
                return events
            if self.on_event:
                self.on_event(ev)
            events.append(ev)
            if first and ev.get('ev') == 'hello':
                return events

    def cmd(self, line):
        """Einen Befehl schicken; liefert alle Ereignisse bis zum ok."""
        with self.lock:
            self.proc.stdin.write(line.replace('\n', ' ') + '\n')
            self.proc.stdin.flush()
            return self._read_until_ok()

    def state(self):
        for ev in self.cmd('state'):
            if ev.get('ev') == 'state':
                return ev
        return None

    def close(self):
        try:
            self.proc.stdin.write('quit\n')
            self.proc.stdin.flush()
        except OSError:
            pass
        try:
            self.proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            self.proc.kill()


# ------------------------------------------------------------------ ETS-Sichtbarkeit
# Dieselbe Auswertung wie web/app.js (evalTest, condOk, effectiveValues,
# visibleKos), damit der Server ohne Browser weiss, welche Objekte die ETS
# einblendet.

def eval_test(v, test):
    import re
    test = str(test).strip()
    try:
        x = float(v)
    except (TypeError, ValueError):
        x = None
    m = re.match(r'^(<=|>=|!=|<|>|=)\s*(-?[\d.]+)$', test)
    if m:
        if x is None:
            return False
        y = float(m.group(2))
        op = m.group(1)
        return {'<': x < y, '>': x > y, '<=': x <= y, '>=': x >= y, '!=': x != y}.get(op, x == y)
    for t in test.split():
        try:
            if x is not None and float(t) == x:
                return True
        except ValueError:
            pass
        if t == str(v):
            return True
    return False


def cond_ok(conds, vals):
    for c in conds:
        v = vals.get(c[0])
        if c[1] is None:
            if any(eval_test(v, t) for t in (c[2] if len(c) > 2 else [])):
                return False
        elif not eval_test(v, c[1]):
            return False
    return True


def effective_values(meta, changed):
    vals = {n: p['d'] for n, p in meta['params'].items()}
    vals.update(changed)
    for _ in range(3):
        for pg in meta['pages']:
            if not cond_ok(pg['cond'], vals):
                continue
            for it in pg['items']:
                if it['type'] == 'a' and it.get('target') and cond_ok(it['cond'], vals):
                    vals[it['target']] = vals.get(it['source']) if it.get('source') else float(it['value'])
    return vals


def assigned_values(meta, changed):
    """Werte der <Assign>-Ziele, die ins Geraet muessen (z.B. APP_OperatingMode).
    Die ETS rechnet sie beim Parametrieren aus; der Simulator tut dasselbe vor
    jedem Download."""
    vals = effective_values(meta, changed)
    out = {}
    for pg in meta['pages']:
        for it in pg['items']:
            if it['type'] == 'a' and it.get('target') in meta['params']:
                p = meta['params'][it['target']]
                if p['o'] is not None:
                    out[it['target']] = vals[it['target']]
    return out


def visible_kos(meta, vals):
    """Objektnummer -> DPT der sichtbaren Variante."""
    out = {}
    for pg in meta['pages']:
        if not cond_ok(pg['cond'], vals):
            continue
        for it in pg['items']:
            if it['type'] == 'k' and it['nr'] not in out and cond_ok(it['cond'], vals):
                out[it['nr']] = it.get('dpt', '')
    return out
