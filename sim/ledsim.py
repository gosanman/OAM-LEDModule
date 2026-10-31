"""LED-Simulator - Python-App mit grafischer Oberflaeche im Browser.

Startet den Simulatorkern der gewaehlten Hardware-Variante
(build/ledsim_<Board>.exe, der UNVERAENDERTE Firmware-Code des LED-Dimmers),
treibt die Simulationszeit und stellt die Oberflaeche unter
http://127.0.0.1:8766 bereit. Nur Python-Standardbibliothek.

    python sim/ledsim.py [--port 8766] [--no-browser] [--project "Name"]

Was die App tut:
  - Parameter wie in der ETS setzen und "programmieren" (ETS-Download mit
    Neustart, genau wie am Geraet),
  - Eingangsobjekte senden, auf Wunsch zyklisch,
  - KNX-Bus und LED-Versorgung schalten, Busspannungseinbruch, Funktionstaste,
    Bedienfeld und ETS-Funktionstests,
  - Last je Port, Kurzschluss, Bausteinausfaelle und Uhrzeit einstellen,
  - Ausgaenge, Licht, Messwerte, Display, Telegramme, Konsole und Waechter zeigen.
"""

import argparse
import datetime
import json
import os
import re
import threading
import time
import webbrowser
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, unquote, urlparse

from ledcore import BOARDS, BUILD, HERE, Core, ParamImage, exe_for, load_meta

WEB = os.path.join(HERE, 'web')
PROJECTS = os.path.join(HERE, 'projects')
TICK_S = 0.1          # Takt der Echtzeit
MAX_EVENTS = 6000
MAX_PWM = 400000

# ETS-Platinenauswahl je Hardware-Variante (APP_Hardware = LED_BOARD_ID in include/hardware.h).
# Die Geraeteklasse APP_ControllerType setzt die ETS daraus per Assign.
BOARD_HW = {'DK_06_V10': 1, 'DK_12_V10': 2, 'DK_06_V12': 3, 'DK_12_V12': 4, 'APP_06_V01': 5}

HW_DEFAULT = dict(portW=[12.0] * 12, short=[False] * 12, supplyV=24.0, supplyOn=True, shunt=0.010,
                  ambient=25.0, chips=dict(pca=True, ina=True, tmp=True, oled=True),
                  time='', timeValid=True, pa='1.1.36')


def safe_name(name):
    return re.sub(r'[^\w äöüÄÖÜß.+()-]', '_', name).strip() or 'Projekt'


def epoch_of(iso):
    """Lokale Zeit 'JJJJ-MM-TT hh:mm' -> Sekunden (lokal, ohne Zeitzone)."""
    try:
        dt = datetime.datetime.strptime(iso, '%Y-%m-%d %H:%M')
    except (TypeError, ValueError):
        now = datetime.datetime.now().replace(second=0, microsecond=0)
        dt = now
    return int((dt - datetime.datetime(1970, 1, 1)).total_seconds())


class Simulator:
    def __init__(self, project):
        self.meta = load_meta()
        self.meta_json = json.dumps(self.meta, ensure_ascii=False).encode('utf-8')
        self.lock = threading.RLock()
        self.speed = 1.0
        self.events = []        # (seq, ev)
        self.seq = 0
        self.pwm = []           # [t, d0, d1, ...]
        self.kos = {}           # nr -> {val, raw, t, dir, n, dpt}
        self.kos_version = 0
        self.cyclic = {}        # nr -> {dpt, value, period, next}
        self.state = None
        self.disp = None        # {'on', 'hex', 't'}
        self.fp_result = None
        self.pending = ParamImage(self.meta)
        self.active_values = {}
        self.project = ''
        self.board = 'DK_12_V12'
        self.hw = json.loads(json.dumps(HW_DEFAULT))
        self.core = None
        self.load_project(project)

    # ------------------------------------------------------------ Kern
    def _on_event(self, ev):
        kind = ev.get('ev')
        if kind == 'pwm':
            self.pwm.append([ev['t']] + ev['d'])
            if len(self.pwm) > MAX_PWM:
                del self.pwm[: MAX_PWM // 4]
            return
        if kind == 'state':
            self.state = ev
            return
        if kind == 'disp':
            self.disp = dict(on=ev['on'], hex=ev['hex'], t=ev['t'])
            return
        if kind == 'fp':
            self.fp_result = ev
        if kind in ('ko', 'rx'):
            k = self.kos.setdefault(ev['nr'], {'n': 0})
            k.update(val=ev['val'], raw=ev.get('raw', ''), t=ev['t'], dir='tx' if kind == 'ko' else 'rx', dpt=ev['dpt'])
            k['n'] += 1
            self.kos_version += 1
        self.seq += 1
        self.events.append((self.seq, ev))
        if len(self.events) > MAX_EVENTS:
            del self.events[: MAX_EVENTS // 4]

    def _start_core(self):
        if self.core:
            try:
                self.core.close()
            except Exception:
                pass
        self.core = Core(self.board, on_event=self._on_event)
        self.pwm.clear()
        self.kos.clear()
        self.kos_version += 1
        self.cyclic.clear()
        self.disp = None

    def cmd(self, line):
        with self.lock:
            return self.core.cmd(line)

    def _apply_hw(self):
        hw = self.hw
        for p, w in enumerate(hw['portW']):
            self.core.cmd(f'portw {p} {float(w)}')
        for p, s in enumerate(hw['short']):
            self.core.cmd(f'short {p} {1 if s else 0}')
        self.core.cmd(f"supplyv {float(hw['supplyV'])}")
        self.core.cmd(f"supply {1 if hw['supplyOn'] else 0}")
        self.core.cmd(f"shunt {float(hw['shunt'])}")
        self.core.cmd(f"ambient {float(hw['ambient'])}")
        for c, on in hw['chips'].items():
            self.core.cmd(f'chip {c} {1 if on else 0}')
        self.core.cmd(f"timevalid {1 if hw['timeValid'] else 0}")
        self.core.cmd(f"pa {hw['pa']}")

    def run(self, ms):
        """Zeit fortschreiben; zyklische Telegramme zur rechten Zeit einstreuen."""
        with self.lock:
            now = self.state['t'] if self.state else 0
            end = now + int(ms)
            while now < end:
                due = [c for c in self.cyclic.values() if c['next'] <= now]
                for c in due:
                    self.core.cmd(f"ko {c['nr']} {c['dpt']} {c['value']}")
                    c['next'] = now + c['period']
                nxt = min([c['next'] for c in self.cyclic.values()] + [end])
                step = max(1, min(nxt, end) - now)
                self.core.cmd(f'run {step}')
                now += step
            self.core.cmd('state')

    def tick_loop(self):
        while True:
            t0 = time.time()
            if self.speed > 0:
                try:
                    self.run(TICK_S * 1000 * self.speed)
                except Exception as e:  # Kern abgestuerzt: melden, neu starten
                    self._on_event({'ev': 'error', 't': 0, 'text': f'Kern: {e} - Neustart des Simulators'})
                    with self.lock:
                        self.load_project(self.project)
            time.sleep(max(0.0, TICK_S - (time.time() - t0)))

    # ------------------------------------------------------------ Projekte
    def project_path(self, name):
        return os.path.join(PROJECTS, safe_name(name) + '.json')

    def list_projects(self):
        if not os.path.isdir(PROJECTS):
            return []
        return sorted(f[:-5] for f in os.listdir(PROJECTS) if f.endswith('.json'))

    def load_project(self, name, board=None):
        """Projekt laden: Hardware-Variante, Parameter und Last setzen und das
        Geraet frisch starten - als waere es so programmiert und eingeschaltet."""
        data = {'values': {}}
        if name and os.path.exists(self.project_path(name)):
            with open(self.project_path(name), encoding='utf-8') as f:
                data = json.load(f)
        with self.lock:
            self.board = board or data.get('board') or self.board
            if self.board not in BOARDS:
                self.board = 'DK_12_V12'
            self.hw = json.loads(json.dumps(HW_DEFAULT))
            for k, v in (data.get('hw') or {}).items():
                if k in self.hw:
                    self.hw[k] = v
            self.hw['portW'] = (list(self.hw['portW']) + HW_DEFAULT['portW'])[:12]
            self.hw['short'] = (list(self.hw['short']) + HW_DEFAULT['short'])[:12]
            self._start_core()
            self.project = name or ''
            self.pending.load_values(data.get('values', {}))
            if 'APP_Hardware' not in data.get('values', {}):
                self.pending.set('APP_Hardware', BOARD_HW.get(self.board, 0))
            self.active_values = dict(self.pending.values)
            self._apply_hw()
            self.core.cmd(f"time {epoch_of(self.hw['time'])}")
            self.core.cmd('load ' + self.pending.device_hex())
            self.core.cmd('bus 1')
            self.core.cmd('run 500')
            self.core.cmd('state')

    def save_project(self, name):
        os.makedirs(PROJECTS, exist_ok=True)
        with open(self.project_path(name), 'w', encoding='utf-8') as f:
            json.dump({'name': name, 'board': self.board, 'values': self.pending.values, 'hw': self.hw},
                      f, ensure_ascii=False, indent=1)
        self.project = name

    # ------------------------------------------------------------ Aktionen
    def action(self, a):
        op = a.get('action')
        if op == 'speed':
            self.speed = float(a['value'])
        elif op == 'step':
            self.run(int(a['ms']))
        elif op == 'power':
            if a['which'] == 'bus':
                self.cmd(f"bus {1 if a['on'] else 0}")
            else:
                self.hw['supplyOn'] = bool(a['on'])
                self.cmd(f"supply {1 if a['on'] else 0}")
            self.cmd('state')
        elif op == 'busdip':
            self.cmd(f"busdip {int(a.get('ms', 200))}")
            self.cmd('state')
        elif op == 'ko':
            nr, dpt, value = int(a['nr']), a['dpt'], str(a['value']).strip()
            self.cmd(f'ko {nr} {dpt} {value}')
            period = float(a.get('cyclic') or 0)
            with self.lock:
                if period > 0:
                    now = self.state['t'] if self.state else 0
                    self.cyclic[nr] = dict(nr=nr, dpt=dpt, value=value, period=int(period * 1000),
                                           next=now + int(period * 1000))
                else:
                    self.cyclic.pop(nr, None)
            self.cmd('state')
        elif op == 'cyclic_off':
            with self.lock:
                self.cyclic.pop(int(a['nr']), None)
        elif op == 'btn':
            self.cmd(f"btn {a['kind']}")
            self.cmd('state')
        elif op == 'panel':
            self.cmd(f"panel {int(a['button'])}")
            self.run(120)
        elif op == 'fp':
            # ETS-Funktionstest: [1, Kanal-ID, Aktion] an ObjectIndex 160 / PropertyId 6 (LED.script.js)
            data = bytes([1, int(a['port']), 1 if a['on'] else 0]).hex()
            self.fp_result = None
            self.cmd(f'fp 160 6 {data}')
            self.cmd('state')
            return {'ok': True, 'fp': self.fp_result}
        elif op == 'fp_raw':
            # ETS-Skript (Schaltflaeche in der ETS-Ansicht): online.invokeFunctionProperty
            data = bytes(int(x) & 0xFF for x in a.get('data', [])).hex()
            self.fp_result = None
            self.cmd(f"fp {int(a['obj'])} {int(a['prop'])} {data or '00'}")
            self.cmd('state')
            r = self.fp_result or {}
            return {'ok': True, 'handled': bool(r.get('handled')), 'text': r.get('text', ''),
                    'res': list(bytes.fromhex(r.get('res') or ''))}
        elif op == 'console':
            self.cmd(('diag ' if a.get('diag') else 'cmd ') + a['text'])
        elif op == 'param':
            with self.lock:
                self.pending.set(a['name'], a['value'])
        elif op == 'params_revert':
            with self.lock:
                self.pending.load_values(self.active_values)
        elif op == 'params_defaults':
            with self.lock:
                self.pending.reset()
                self.pending.set('APP_Hardware', BOARD_HW.get(self.board, 0))
        elif op == 'program':
            with self.lock:
                self.core.cmd('pimg ' + self.pending.device_hex())
                self.core.cmd(f"ets {int(a.get('ms', 3000))}")
                self.active_values = dict(self.pending.values)
                self.core.cmd('state')
        elif op == 'restart':
            self.cmd('restart')
            self.cmd('state')
        elif op == 'unload':
            self.cmd('unload')
            self.cmd('state')
        elif op == 'project_load':
            self.load_project(a['name'])
        elif op == 'project_save':
            self.save_project(a['name'])
        elif op == 'reset':
            self.load_project(self.project)
        elif op == 'board':
            # andere Hardware: Parameter behalten, Platinenauswahl passend setzen, neu starten
            with self.lock:
                values = dict(self.pending.values)
                values['APP_Hardware'] = BOARD_HW.get(a['board'], 0)
                self.board = a['board']
                hw = self.hw
                self._start_core()
                self.hw = hw
                self.pending.load_values(values)
                self.active_values = dict(self.pending.values)
                self._apply_hw()
                self.core.cmd(f"time {epoch_of(self.hw['time'])}")
                self.core.cmd('load ' + self.pending.device_hex())
                self.core.cmd('bus 1')
                self.core.cmd('run 500')
                self.core.cmd('state')
        elif op == 'hw':
            with self.lock:
                patch = a.get('patch', {})
                for k, v in patch.items():
                    if k in ('portW', 'short'):
                        for p, x in v.items():
                            self.hw[k][int(p)] = x
                    elif k == 'chips':
                        self.hw['chips'].update(v)
                    elif k in self.hw:
                        self.hw[k] = v
                self._apply_hw()
                if 'time' in patch:
                    self.core.cmd(f"time {epoch_of(self.hw['time'])}")
                self.core.cmd('state')
        elif op == 'clear':
            with self.lock:
                self.events.clear()
        else:
            raise ValueError(f'unbekannte Aktion {op}')
        return {'ok': True}

    def poll(self, since, pwm_since, kos_version, disp_t):
        with self.lock:
            ev = [e for s, e in self.events if s > since] if since >= 0 else [e for _, e in self.events[-500:]]
            ev = ev[-1500:]
            pwm_since = max(0, min(pwm_since, len(self.pwm)))
            out = dict(seq=self.seq, events=ev, state=self.state, speed=self.speed,
                       pwm=self.pwm[pwm_since:], pwmIdx=len(self.pwm),
                       project=self.project, projects=self.list_projects(),
                       board=self.board, boards=BOARDS, hw=self.hw,
                       hello=self.core.hello if self.core else {},
                       pending=self.pending.values, active=self.active_values,
                       dirty=self.pending.values != self.active_values,
                       cyclic=[dict(nr=c['nr'], dpt=c['dpt'], value=c['value'],
                                    period=c['period'] / 1000) for c in self.cyclic.values()],
                       kosVersion=self.kos_version)
            if kos_version != self.kos_version:
                out['kos'] = self.kos
            if self.disp and self.disp['t'] != disp_t:
                out['disp'] = self.disp
            return out


SIM = None


class Server(ThreadingHTTPServer):
    allow_reuse_address = False
    daemon_threads = True


class Handler(BaseHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def _send(self, code, body, ctype='application/json; charset=utf-8'):
        if isinstance(body, (dict, list)):
            body = json.dumps(body, ensure_ascii=False).encode('utf-8')
        elif isinstance(body, str):
            body = body.encode('utf-8')
        self.send_response(code)
        self.send_header('Content-Type', ctype)
        self.send_header('Content-Length', str(len(body)))
        self.send_header('Cache-Control', 'no-store')
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        url = urlparse(self.path)
        if url.path == '/api/meta':
            return self._send(200, SIM.meta_json)
        if url.path == '/api/poll':
            q = parse_qs(url.query)
            return self._send(200, SIM.poll(int(q.get('since', ['-1'])[0]),
                                            int(q.get('pwm', ['0'])[0]),
                                            int(q.get('kv', ['-1'])[0]),
                                            int(q.get('dt', ['-1'])[0])))
        name = 'index.html' if url.path in ('/', '') else url.path.lstrip('/')
        base = WEB
        if name.startswith('baggage/'):       # Bilder, Icons, Hilfetexte aus der knxprod
            base = os.path.join(BUILD, 'baggage')
            name = name[len('baggage/'):]
        path = os.path.normpath(os.path.join(base, unquote(name)))
        if not path.startswith(base) or not os.path.isfile(path):
            return self._send(404, {'error': 'nicht gefunden'})
        ctype = {'.html': 'text/html; charset=utf-8', '.js': 'text/javascript; charset=utf-8',
                 '.css': 'text/css; charset=utf-8', '.svg': 'image/svg+xml', '.png': 'image/png',
                 '.txt': 'text/plain; charset=utf-8'}.get(
            os.path.splitext(path)[1], 'application/octet-stream')
        with open(path, 'rb') as f:
            self._send(200, f.read(), ctype)

    def do_POST(self):
        if urlparse(self.path).path != '/api/action':
            return self._send(404, {'error': 'nicht gefunden'})
        try:
            data = json.loads(self.rfile.read(int(self.headers.get('Content-Length', 0))) or b'{}')
            self._send(200, SIM.action(data))
        except Exception as e:
            self._send(400, {'ok': False, 'error': str(e)})


def main():
    global SIM
    ap = argparse.ArgumentParser(description='LED-Simulator (OpenKNX LED-Dimmer)')
    ap.add_argument('--port', type=int, default=8766)
    ap.add_argument('--no-browser', action='store_true')
    ap.add_argument('--project', default='DK-12 V1.2 - 4x TW + 4x EK')
    args = ap.parse_args()

    if not os.path.exists(os.path.join(BUILD, 'meta.json')):
        raise SystemExit('build/meta.json fehlt - zuerst: powershell -File sim/build.ps1')
    try:
        server = Server(('127.0.0.1', args.port), Handler)
    except OSError:
        raise SystemExit(f'Port {args.port} ist belegt - läuft der Simulator schon? '
                         f'Sonst mit --port einen anderen wählen.')
    SIM = Simulator(args.project)
    threading.Thread(target=SIM.tick_loop, daemon=True).start()
    url = f'http://127.0.0.1:{args.port}/'
    print(f'LED-Simulator läuft: {url}   (Beenden mit Strg+C)')
    if not args.no_browser:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        SIM.core.close()


if __name__ == '__main__':
    main()
