'use strict';
// LED-Simulator - Oberflaeche. Holt Metadaten einmal, fragt den Zustand alle
// 250 ms ab und zeichnet nur, was sich geaendert hat. Eingaben gehen als
// Aktionen an die Python-App, die sie an den Kern weiterreicht.

const $ = (s, el = document) => el.querySelector(s);
const $$ = (s, el = document) => [...el.querySelectorAll(s)];
const h = (tag, attrs = {}, ...kids) => {
  const e = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (v === undefined || v === null || v === false) continue;
    if (k === 'class') e.className = v;
    else if (k === 'style') e.setAttribute('style', v);
    else if (k.startsWith('on')) e.addEventListener(k.slice(2), v);
    else if (k === 'html') e.innerHTML = v;
    else if (k === 'value') e.value = v;
    else e.setAttribute(k, v === true ? '' : v);
  }
  for (const kid of kids.flat()) if (kid !== null && kid !== undefined && kid !== false)
    e.append(kid instanceof Node ? kid : document.createTextNode(String(kid)));
  return e;
};
const de = (v, d = 2) => (v === null || v === undefined || Number.isNaN(v)) ? '—'
  : Number(v).toLocaleString('de-DE', { minimumFractionDigits: d, maximumFractionDigits: d });
const fmtT = ms => {
  const s = Math.floor(ms / 1000);
  return `T+${String(Math.floor(s / 3600)).padStart(2, '0')}:${String(Math.floor(s / 60) % 60).padStart(2, '0')}:${String(s % 60).padStart(2, '0')}`;
};
const fmtTms = ms => fmtT(ms) + ',' + String(Math.floor(ms % 1000)).padStart(3, '0');
const fmtShortT = ms => {
  const s = ms / 1000;
  if (s < 120) return de(s, 1) + ' s';
  if (s < 7200) return de(s / 60, 1) + ' min';
  return de(s / 3600, 2) + ' h';
};
const hex2 = n => n.toString(16).toUpperCase().padStart(2, '0');

const PORTS = 'ABCDEFGHIJKL';
// Farben der Ausgaenge nach Rolle, und eine Palette fuer den Verlauf
const ROLE_COLOR = { ek: '#f5c451', ww: '#ff9f43', cw: '#7cc4ff', r: '#ef4444', g: '#22c55e', b: '#3b82f6', '': '#9ca3af' };
const ROLE_NAME = { ek: 'EK', ww: 'WW', cw: 'KW', r: 'R', g: 'G', b: 'B' };
const PALETTE = ['#2563eb', '#ea580c', '#16a34a', '#9333ea', '#e11d48', '#0891b2', '#65a30d', '#a16207', '#db2777', '#4f46e5', '#0d9488', '#b45309'];
const TYPE_RANK = { RGB: 0, TW: 100, EK: 200, HCL: 300, 'Gerät': 400, OpenKNX: 500 };

const S = {
  meta: null, params: {}, kos: {}, pages: [],
  poll: null, since: -1, pwmIdx: 0, kv: -1, dt: -1,
  pwm: [], kovals: {},
  tab: 'inputs', page: null, win: 60000,
  sel: (() => { try { return localStorage.getItem('ledsim.sel'); } catch (e) { return null; } })(),
  values: {}, activeValues: {}, visKos: new Map(),
  logLines: [], busRows: [], checks: [], warnCount: 0,
  sigInputs: '', sigParams: '', sigKos: '', sigHw: '',
};

// ------------------------------------------------------------ Server
async function act(body) {
  let j;
  try {
    const r = await fetch('/api/action', { method: 'POST', body: JSON.stringify(body) });
    j = await r.json();
  } catch (e) { j = { ok: false, error: String(e) }; }
  if (!j.ok) alert('Fehler: ' + j.error);
  setTimeout(poll, 30);
  return j;
}

// ------------------------------------------------------------ ETS-Sichtbarkeit
function evalTest(v, test) {
  test = String(test).trim();
  const m = test.match(/^(<=|>=|!=|<|>|=)\s*(-?[\d.]+)$/);
  const x = Number(v);
  if (m) {
    const y = Number(m[2]);
    switch (m[1]) {
      case '<': return x < y; case '>': return x > y; case '<=': return x <= y;
      case '>=': return x >= y; case '!=': return x != y; default: return x == y;
    }
  }
  return test.split(/\s+/).some(t => Number(t) === x || t === String(v));
}

function condOk(conds, vals) {
  for (const c of conds) {
    const v = vals[c[0]];
    if (c[1] === null || c[1] === undefined) {
      if ((c[2] || []).some(t => evalTest(v, t))) return false;
    } else if (!evalTest(v, c[1])) return false;
  }
  return true;
}

/// Werte wie die ETS sie sieht: Vorgaben, darueber die Projektwerte, dann die
/// <Assign>-Zuweisungen (z. B. Betriebsmodus aus der Betriebsmodus-Auswahl, Geraeteklasse aus der Platine).
function effectiveValues(changed) {
  const v = {};
  for (const [n, p] of Object.entries(S.params)) v[n] = p.d;
  Object.assign(v, changed);
  for (let pass = 0; pass < 3; pass++)
    for (const pg of S.pages) {
      if (!condOk(pg.cond, v)) continue;
      for (const it of pg.items)
        if (it.type === 'a' && it.target && condOk(it.cond, v))
          v[it.target] = it.source ? v[it.source] : Number(it.value);
    }
  return v;
}

function visibleKos(vals) {
  const m = new Map();
  for (const pg of S.pages) {
    if (!condOk(pg.cond, vals)) continue;
    for (const it of pg.items)
      if (it.type === 'k' && !m.has(it.nr) && condOk(it.cond, vals)) {
        m.set(it.nr, it.dpt);
        const k = S.kos[it.nr];
        if (k) {
          if (k.t0 === undefined) { k.t0 = k.t; k.f0 = k.f; }
          k.t = it.t || k.t0; k.f = it.f || k.f0;
        }
      }
  }
  return m;
}

// ------------------------------------------------------------ Abfrage
async function poll() {
  let j;
  try {
    const r = await fetch(`/api/poll?since=${S.since}&pwm=${S.pwmIdx}&kv=${S.kv}&dt=${S.dt}`);
    j = await r.json();
  } catch (e) { return; }
  S.poll = j;
  S.since = j.seq;
  if (j.pwmIdx < S.pwmIdx) S.pwm = [];   // Kern neu gestartet
  S.pwm.push(...j.pwm);
  if (S.pwm.length > 200000) S.pwm.splice(0, 50000);
  S.pwmIdx = j.pwmIdx;
  if (j.kos) { S.kovals = j.kos; S.kv = j.kosVersion; }
  if (j.disp) { S.disp = j.disp; S.dt = j.disp.t; drawOled(); }
  for (const ev of j.events) onEvent(ev);

  S.values = effectiveValues(j.pending);
  S.activeValues = effectiveValues(j.active);
  S.visKos = visibleKos(S.activeValues);
  render();
}

function onEvent(ev) {
  const t = ev.t !== undefined ? fmtT(ev.t) : '';
  switch (ev.ev) {
    case 'log':
      pushLog(`${t}  ${ev.src ? ev.src + ': ' : ''}${ev.text}`, ev.level); break;
    case 'ko': case 'rx':
      S.busRows.push(ev);
      if (S.busRows.length > 3000) S.busRows.splice(0, 1000);
      if (ev.ev === 'ko' && ev.nr === 7) pushLog(`${t}  Diagnose-KO: ${ev.val}`, 'diag');
      break;
    case 'warn':
      S.checks.push({ t: ev.t, kind: ev.inv, text: ev.text + (ev.count > 1 ? ` (${ev.count}×)` : ''), cls: 'err' });
      S.warnCount++; break;
    case 'info':
      S.checks.push({ t: ev.t, kind: 'Hinweis', text: ev.text, cls: 'wrn' }); break;
    case 'error':
      S.checks.push({ t: ev.t, kind: 'Fehler', text: ev.text, cls: 'err' });
      pushLog(`${t}  FEHLER: ${ev.text}`, 'error'); break;
    case 'boot':
      pushLog(`${t}  ── Neustart, ${ev.configured ? 'parametriert' : 'ohne Parametrierung'} ──`, 'debug');
      S.checks.push({ t: ev.t, kind: 'Start', text: ev.configured ? 'Neustart mit Parametern' : 'Neustart ohne Parameter', cls: 'muted' });
      break;
    case 'download':
      pushLog(`${t}  ETS-Download ${ev.phase === 'start' ? 'beginnt' : 'fertig'}`, 'debug'); break;
    case 'supply':
      S.checks.push({ t: ev.t, kind: 'LED 24 V', text: ev.on ? '24 V ein' : '24 V aus', cls: 'muted' }); break;
    case 'mcu':
      S.checks.push({ t: ev.t, kind: 'KNX-Bus', text: 'Bus aus, Prozessor aus', cls: 'muted' }); break;
    case 'busdip':
      S.checks.push({ t: ev.t, kind: 'Busdip', text: ev.phase === 'start' ? `Busspannung bricht ${ev.ms} ms ein (SAVE)` :
        (ev.reboot ? 'Busspannung zurück - Neustart' : 'Busspannung zurück - weiter ohne Neustart'), cls: 'muted' }); break;
    case 'flash':
      pushLog(`${t}  Flash gespeichert`, 'debug'); break;
  }
  if (S.checks.length > 2000) S.checks.splice(0, 500);
}

function pushLog(text, cls = '') {
  S.logLines.push({ text, cls });
  if (S.logLines.length > 3000) S.logLines.splice(0, 1000);
  S.logDirty = true;
}

// ------------------------------------------------------------ Kanaele und Licht
/// Kanal eines Objekts: "EK 1", "TW 2", "RGB 1", "HCL 1", "Gerät", "OpenKNX".
function channelOfKo(k) {
  const m = k.name.match(/^(EK|TW|RGB|HCL)_(\d+)/);
  if (m) return `${m[1]} ${m[2]}`;
  return k.name.startsWith('APP_') ? 'Gerät' : 'OpenKNX';
}
/// "EK 1" -> "EK 1 · Küche", wenn in der ETS ein Name steht.
function chName(ch) {
  const [type, no] = ch.split(' ');
  const n = no && S.activeValues[`${type}_${no}Name`];
  return n && String(n).trim() ? `${ch} · ${n}` : ch;
}
function chRank(ch) {
  const chans = S.poll?.state?.sim?.channels || [];
  const i = chans.findIndex(c => `${c.type} ${c.no}` === ch);
  if (i >= 0) return i;
  const [type, no] = ch.split(' ');
  return 1000 + (TYPE_RANK[type] ?? 900) + Number(no || 0);
}
/// Kanalobjekte heissen in der ETS "EK1: ..." - dort sagt die Funktion mehr.
const koLabel = k => /^(EK|TW|RGB|HCL)_/.test(k.name) ? (k.f || k.t) : k.t;

/// Port -> {role, ch} aus dem Zustand des Kerns.
function portRoles() {
  const out = {};
  for (const c of S.poll?.state?.sim?.channels || []) {
    const roles = c.type === 'EK' ? ['ek'] : c.type === 'TW' ? ['ww', 'cw'] : ['r', 'g', 'b'];
    c.ports.forEach((p, i) => { out[p] = { role: roles[i], ch: `${c.type} ${c.no}` }; });
  }
  return out;
}

const mix = (a, b, f) => a.map((x, i) => Math.round(x + (b[i] - x) * f));
const WARM = [255, 166, 87], COOL = [214, 228, 255], EKW = [255, 236, 200];
/// Farbe und Helligkeit, die eine Leuchte am Ausgang abgibt (aus den Tastgraden).
function lightOf(c, duty) {
  const d = p => duty[p] || 0;
  let rgb = EKW, level = 0;
  if (c.type === 'EK') level = d(c.ports[0]);
  else if (c.type === 'TW') {
    const w = d(c.ports[0]), k = d(c.ports[1]), s = w + k;
    level = Math.min(1, s);
    if (s > 0) rgb = mix(WARM, COOL, k / s);
  } else {
    const v = c.ports.map(d), m = Math.max(...v);
    level = m;
    if (m > 0) rgb = v.map(x => Math.round(x / m * 255));
  }
  const lo = level > 0.0005 ? 0.12 + 0.88 * Math.pow(level, 0.45) : 0;
  return { css: `rgb(${rgb.join(',')})`, level, lo };
}

function lampEl(c, duty, cls = 'lamp') {
  const l = lightOf(c, duty);
  return { l, el: h('div', { class: 'bulb', style: `--lc:${l.css};--lo:${l.lo.toFixed(3)}` }) };
}

function chStateText(c) {
  if (!c) return '';
  const parts = [c.on ? 'ein' : 'aus'];
  if (c.type === 'EK' || c.type === 'TW') parts.push(`${de(c.b * 100 / 255, 0)} %`);
  if (c.type === 'TW') parts.push(`${c.k} K`);
  if (c.type === 'RGB') parts.push('#' + c.rgb.map(hex2).join(''));
  if (c.night) parts.push('Nacht');
  if (c.hcl) parts.push('HCL');
  return parts.join(' · ');
}

// ------------------------------------------------------------ Kopfleiste
function renderBar() {
  const st = S.poll.state || {};
  const sim = st.sim || {};
  $('#clock').textContent = fmtT(st.t || 0);
  $('#uptime').textContent = st.mcu ? 'Laufzeit ' + fmtShortT(st.uptime || 0) : 'Prozessor aus';
  const tod = $('#tod');
  tod.textContent = sim.time ? sim.time.local.slice(0, 16) : '';
  tod.classList.toggle('invalid', !sim.time?.valid);
  tod.title = sim.time ? `Ortszeit des Geräts${sim.time.valid ? '' : ' (ungültig: Uhrzeit nicht gesetzt)'} · Sonnenaufgang ${sim.time.sunrise} · Sonnenuntergang ${sim.time.sunset}` : '';
  $$('#speed [data-speed]').forEach(b => b.classList.toggle('active', Number(b.dataset.speed) === S.poll.speed));
  for (const [id, on] of [['#pwr-bus', st.bus], ['#pwr-supply', st.supply]]) {
    $(id).classList.toggle('on', !!on); $(id).classList.toggle('off', !on);
  }
  const dirty = Object.keys(diffValues()).length;
  $('#btn-program').classList.toggle('dirty', dirty > 0);
  $('#btn-program').textContent = dirty ? `Programmieren (${dirty})` : 'Programmieren';
  $('#dirty-count').textContent = dirty || '';
  $('#warn-count').textContent = S.warnCount || '';
  const hello = S.poll.hello || {};
  $('#device-sub').textContent = `${hello.device || 'LED-Dimmer'} · ${hello.ports || '?'} Ausgänge · echter Firmware-Code`;
  $('#func1').classList.toggle('hidden', !hello.func1);

  const b = [];
  const badge = (txt, cls, tip) => b.push(h('span', { class: 'badge ' + cls, title: tip }, txt));
  badge(st.mcu ? 'Prozessor läuft' : 'Prozessor aus', st.mcu ? 'ok' : 'bad');
  badge(st.paramsValid ? 'programmiert' : 'nie programmiert', st.paramsValid ? 'ok' : 'warn');
  if (st.downloading) badge('ETS-Download', 'warn');
  const led = sim.led || {};
  if (st.mcu && st.configured) {
    const mode = (S.params.APP_OperatingMode?.enum || []).find(e => e[0] === Number(S.activeValues.APP_OperatingMode));
    if (mode) badge(mode[1], '', 'Betriebsmodus (aus der ETS)');
  }
  if (st.mcu && st.configured && led.hwMismatch) badge('Gerät in ETS passt nicht zur Firmware', 'bad', 'Keine Kanäle, alle Ausgänge aus - unter LED › Allgemein das Gerät wählen und programmieren');
  if (led.powerFault) badge('Überstrom-Latch', 'bad', 'Ausgänge abgeschaltet - „Fehler zurücksetzen“ unter Messung');
  if (st.mcu && !led.pcaConn) badge('PCA9685 fehlt', 'bad');
  if (led.busPowerSave) badge('SAVE aktiv', 'warn');
  if (led.test?.active) badge(`Kanaltest Port ${PORTS[led.test.port] || led.test.port}`, 'warn');
  if (!st.supply) badge('keine 24 V', 'warn');
  $('#badges').replaceChildren(...b);

  (sim.leds || []).forEach(([mode], i) => {
    const el = $('#led-' + i);
    if (el) el.className = 'led' + (mode === 1 ? ' on' : mode === 2 ? ' blink' : mode === 3 ? ' pulse' : '');
  });

  const bsel = $('#board');
  const bsig = Object.keys(S.poll.boards).join('|') + '#' + S.poll.board;
  if (bsel.dataset.sig !== bsig) {
    bsel.dataset.sig = bsig;
    bsel.replaceChildren(...Object.entries(S.poll.boards).map(([k, t]) => h('option', { value: k, selected: k === S.poll.board }, t)));
  }
  const sel = $('#project');
  const names = S.poll.projects.join('|') + '#' + S.poll.project;
  if (sel.dataset.sig !== names) {
    sel.dataset.sig = names;
    sel.replaceChildren(h('option', { value: '' }, '— Projekt laden —'),
      ...S.poll.projects.map(p => h('option', { value: p, selected: p === S.poll.project }, p)));
  }
}

function diffValues() {
  const p = S.poll.pending || {}, a = S.poll.active || {}, d = {};
  for (const k of new Set([...Object.keys(p), ...Object.keys(a)]))
    if (p[k] !== a[k]) d[k] = p[k];
  return d;
}

// ------------------------------------------------------------ Rechte Spalte
function renderLamps() {
  const sim = S.poll.state?.sim; if (!sim) return;
  const duty = sim.duty || [];
  const cards = sim.channels.map(c => {
    const key = `${c.type} ${c.no}`;
    const { l, el } = lampEl(c, duty);
    const val = c.type === 'TW' && l.level > 0 ? `${de(l.level * 100, 0)} % · ${c.k} K` : `${de(l.level * 100, 0)} %`;
    return h('div', { class: 'lamp' + (S.sel === key ? ' sel' : ''), title: `${chName(key)}\n${chStateText(c)}\nKlicken: Bedienung dieses Kanals`,
      onclick: () => { S.sel = key; saveSel(); $$('#tabs button').find(b => b.dataset.tab === 'inputs').click(); } },
      el, h('div', { class: 'nm' }, chName(key)), h('div', { class: 'vv' }, val));
  });
  $('#lamps').replaceChildren(...(cards.length ? cards : [h('div', { class: 'emptynote' }, S.poll.state?.mcu ? 'Keine Kanäle - Gerät nicht parametriert.' : 'Prozessor aus.')]));
}

function renderOutputs() {
  const sim = S.poll.state?.sim; if (!sim) return;
  const roles = portRoles();
  const cards = [];
  for (let p = 0; p < sim.ports; p++) {
    const d = sim.duty[p] || 0, r = roles[p] || { role: '', ch: '' };
    const w = sim.load.portW[p], sh = sim.load.short[p];
    const amps = sh ? d * 10 : d * w / 24;
    cards.push(h('div', { class: 'out' + (sh ? ' short' : '') + (r.ch ? '' : ' unused'), style: `--c:${ROLE_COLOR[r.role]}`,
      title: `Port ${PORTS[p]} (PCA-Kanal ${p})\nRegister ON ${sim.pwm.regs[p][0]} OFF ${sim.pwm.regs[p][1]}${sim.pwm.regs[p][2] ? ' FULL-ON' : ''}${sim.pwm.regs[p][3] ? ' FULL-OFF' : ''}` },
      h('div', { class: 'ch' }, h('b', {}, PORTS[p]), h('span', {}, r.ch ? `${r.ch} ${ROLE_NAME[r.role]}` : 'frei')),
      h('div', { class: 'v' }, de(d * 100, 1), h('small', {}, ' %')),
      h('div', { class: 'meter' }, h('span', { style: `width:${(d * 100).toFixed(1)}%` })),
      h('div', { class: 'what' }, sh ? h('span', { class: 'err' }, 'Kurzschluss') : `${de(w, 0)} W · ${de(amps, 2)} A`)));
  }
  $('#outputs').replaceChildren(...cards);
}

function renderMeas() {
  const st = S.poll.state, sim = st?.sim; if (!sim) return;
  const m = sim.meas, ld = sim.load;
  const cell = (label, v, simv, tip) => h('div', { title: tip || '' }, h('small', {}, label), h('b', {}, v), h('span', { class: 'simv' }, simv));
  const al = m.alarm;
  const alarm = (on, txt) => h('span', { class: 'badge ' + (on ? 'bad' : '') }, txt);
  $('#meas').replaceChildren(
    h('div', { class: 'cardhead' }, h('h3', {}, 'Messung'),
      h('span', { class: 'muted' }, `INA226 ${m.ina ? 'ok' : 'fehlt'} · TMP100 ${m.tmp ? 'ok' : 'fehlt'}`)),
    h('div', { class: 'measgrid' },
      cell('Spannung', de(m.V, 2) + ' V', `real ${de(ld.V, 2)} V`, 'links: was die Firmware misst · darunter: Wert der Simulation'),
      cell('Strom', de(m.I, 3) + ' A', `real ${de(ld.I, 3)} A`),
      cell('Leistung', de(m.P, 1) + ' W', `real ${de(ld.I * ld.V, 1)} W`),
      cell('Energie', de(m.Wh, 2) + ' Wh', ''),
      cell('Temperatur', de(m.T, 1) + ' °C', `real ${de(ld.tempC, 1)} °C`)),
    h('div', { class: 'alarms' },
      alarm(al.overV, 'Überspannung'), alarm(al.underV, 'Unterspannung'), alarm(al.overI, 'Überstrom'), alarm(al.overT, 'Übertemperatur'),
      sim.led.powerFault ? h('span', { class: 'badge bad' }, 'Latch gesetzt') : null,
      m.verify ? h('span', { class: 'badge warn' }, 'Prüfung nach Reaktivierung') : null,
      h('span', { style: 'flex:1' }),
      h('button', { class: sim.led.powerFault ? 'danger' : '', disabled: !sim.led.powerFault || null,
        title: 'Konsolenbefehl resetfault: Ausgänge wieder ein, danach wird der Strom geprüft',
        onclick: () => act({ action: 'console', text: 'resetfault' }) }, 'Fehler zurücksetzen')));
}

function renderModule() {
  const st = S.poll.state, sim = st?.sim; if (!sim) return;
  const l = sim.led;
  $('#module').textContent = `PWM ${de(sim.pwm.freq, 1)} Hz${sim.pwm.sleep ? ' (PCA schläft)' : ''} · Kanäle ${l.used} · ` +
    `I²C-Frames ${sim.i2c.frames.toLocaleString('de-DE')} · ohne ACK ${sim.i2c.nacks} · ` +
    `Bausteine: PCA ${sim.chips.pca ? '✓' : '✗'} INA ${sim.chips.ina ? '✓' : '✗'} TMP ${sim.chips.tmp ? '✓' : '✗'} OLED ${sim.chips.oled ? '✓' : '✗'}`;
}

function renderPanel() {
  const hello = S.poll.hello || {};
  $('#panel').classList.toggle('hidden', !hello.panel);
  if (!hello.panel) return;
  const p = S.poll.state?.sim?.panel;
  const names = { 0: 'aus', 1: 'Information', 2: 'Status', 3: 'Messung', 4: 'Temperatur', 5: 'Spannung', 6: 'Strom', 7: 'Leistung', 8: 'Energie', 9: 'Datum/Uhrzeit', 20: 'Anschlüsse', 21: 'Test' };
  $('#panel-screen').textContent = p ? `Seite ${p.screen}: ${names[p.screen] ?? '?'}${p.on ? '' : ' · Display aus'}` : '';
}

/// SSD1306: 128 x 32, 4 Seiten zu 8 Zeilen, je Byte eine Spalte (Bit 0 oben).
function drawOled() {
  const c = $('#oled'); if (!c || !S.disp) return;
  const g = c.getContext('2d');
  const img = g.createImageData(128, 32);
  const hex = S.disp.hex || '';
  for (let page = 0; page < 4; page++)
    for (let x = 0; x < 128; x++) {
      const i = page * 128 + x;
      const b = parseInt(hex.substr(i * 2, 2), 16) || 0;
      for (let bit = 0; bit < 8; bit++) {
        const on = S.disp.on && (b >> bit) & 1;
        const o = ((page * 8 + bit) * 128 + x) * 4;
        img.data[o] = on ? 120 : 6; img.data[o + 1] = on ? 210 : 8; img.data[o + 2] = on ? 255 : 12; img.data[o + 3] = 255;
      }
    }
  g.putImageData(img, 0, 0);
}

// ------------------------------------------------------------ Verlauf
function renderChart() {
  const c = $('#chart');
  const dpr = window.devicePixelRatio || 1;
  const w = c.clientWidth, hgt = c.clientHeight || 200;
  if (!w) return;
  // Die Pixelgroesse folgt der CSS-Groesse, nie umgekehrt.
  if (c.width !== Math.round(w * dpr) || c.height !== Math.round(hgt * dpr)) { c.width = Math.round(w * dpr); c.height = Math.round(hgt * dpr); }
  const g = c.getContext('2d');
  g.setTransform(dpr, 0, 0, dpr, 0, 0);
  const css = n => getComputedStyle(document.documentElement).getPropertyValue(n).trim();
  g.clearRect(0, 0, w, hgt);
  const L = 34, R = 8, T = 18, B = 18, pw = w - L - R, ph = hgt - T - B;
  const now = S.poll.state?.t || 0, t0 = Math.max(0, now - S.win);
  const X = t => L + (t - t0) / Math.max(1, now - t0) * pw;
  const Y = v => T + (1 - v) * ph;
  g.font = '11px system-ui'; g.fillStyle = css('--muted'); g.lineWidth = 1;
  for (const v of [0, 0.25, 0.5, 0.75, 1]) {
    g.strokeStyle = css('--line'); g.beginPath(); g.moveTo(L, Y(v)); g.lineTo(L + pw, Y(v)); g.stroke();
    g.fillText(de(v * 100, 0), 4, Y(v) + 4);
  }
  g.fillText(fmtT(t0), L, hgt - 4);
  const lbl = fmtT(now); g.fillText(lbl, L + pw - g.measureText(lbl).width, hgt - 4);

  const n = S.poll.state?.sim?.ports || 12;
  const roles = portRoles();
  const used = [...Array(n).keys()].filter(p => roles[p] || S.pwm.some(r => r[p + 1] > 0));
  let start = 0;
  { let lo = 0, hi = S.pwm.length - 1;
    while (lo < hi) { const mid = (lo + hi + 1) >> 1; if (S.pwm[mid][0] <= t0) lo = mid; else hi = mid - 1; }
    start = Math.max(0, lo); }
  for (const p of used) {
    g.strokeStyle = PALETTE[p % PALETTE.length]; g.lineWidth = 1.5; g.beginPath();
    let prev = null;
    for (let i = start; i < S.pwm.length; i++) {
      const r = S.pwm[i], x = X(Math.max(r[0], t0)), y = Y(r[p + 1]);
      if (prev === null) g.moveTo(x, y); else { g.lineTo(x, prev); g.lineTo(x, y); }
      prev = y;
    }
    if (prev !== null) g.lineTo(L + pw, prev);
    g.stroke();
  }
  let lx = L;
  for (const p of used) {
    g.fillStyle = PALETTE[p % PALETTE.length]; g.fillRect(lx, 5, 10, 3);
    g.fillStyle = css('--text'); g.fillText(PORTS[p], lx + 12, 11); lx += 28;
  }
}

// ------------------------------------------------------------ Objektwerte
/// Wert eines Objekts lesbar, nach dem DPT der ETS. Die Firmware sendet
/// manche Objekte mit einem anderen Untertyp (z. B. Helligkeit als 5.004
/// statt 5.001) - daher wird, wo moeglich, das Rohtelegramm dekodiert.
function fmtKo(nr, v, dpt) {
  if (!v || v.val === undefined || v.val === null) return '—';
  const [m, s] = String(dpt || v.dpt || '').split('.').map(Number);
  const raw = v.raw || '';
  const n = raw ? parseInt(raw, 16) : NaN;
  const b0 = raw ? parseInt(raw.slice(0, 2), 16) : NaN;
  if (raw && !Number.isNaN(n)) {
    if (m === 1) {
      const txt = { 1: ['Aus', 'Ein'], 5: ['kein Alarm', 'Alarm'], 11: ['0', '1'], 17: ['0', 'Auslösen'], 2: ['falsch', 'wahr'] }[s] || ['0', '1'];
      return txt[b0 & 1];
    }
    if (m === 3) {
      const nib = b0 & 0x0F, step = nib & 7;
      return step ? `${nib & 8 ? '▲ heller' : '▼ dunkler'} ${de(100 / (1 << (step - 1)), step > 3 ? 1 : 0)} %` : 'Stopp';
    }
    if (m === 5) {
      if (s === 1) return de(b0 * 100 / 255, 0) + ' %';
      if (s === 3) return de(b0 * 360 / 255, 0) + '°';
      return String(b0);
    }
    if (m === 7) return (s === 600 ? n.toLocaleString('de-DE') + ' K' : String(n));
    if (m === 17) return 'Szene ' + ((b0 & 0x3F) + 1);
    if (m === 18) return (b0 & 0x80 ? 'Lernen ' : 'Szene ') + ((b0 & 0x3F) + 1);
    if (m === 232) {
      const t = raw.toUpperCase().padStart(6, '0');
      const kname = S.kos[nr]?.name || '';
      if (/HSV/.test(kname)) {
        const [hh, ss, vv] = [0, 2, 4].map(i => parseInt(t.substr(i, 2), 16));
        return `H ${de(hh * 360 / 255, 0)}° S ${de(ss * 100 / 255, 0)} % V ${de(vv * 100 / 255, 0)} %`;
      }
      return '#' + t;
    }
  }
  const val = v.val;
  if (typeof val === 'boolean') return val ? '1' : '0';
  if (typeof val !== 'number') return String(val);
  if (m === 14) {
    if (s === 27) return de(val, 2) + ' V';
    if (s === 19) return de(val, 3) + ' A';
    if (s === 56) return de(val, 1) + ' W';
    if (s === 68) return de(val, 1) + ' °C';
    return de(val, 2);
  }
  if (m === 13) return de(val, 0) + (s === 10 ? ' Wh' : '');
  if (m === 9) return de(val, 1);
  return de(val, Number.isInteger(val) ? 0 : 2);
}
const koValText = (nr, dpt) => fmtKo(nr, S.kovals[nr], dpt);
/// 232.600 als RGB: Farbe zum Anzeigen, sonst null.
function koColor(nr) {
  const v = S.kovals[nr];
  if (!v?.raw || /HSV/.test(S.kos[nr]?.name || '')) return null;
  return '#' + v.raw.padStart(6, '0').slice(-6);
}

function sendKo(nr, dpt, value, cyclic) { return act({ action: 'ko', nr, dpt, value, cyclic: cyclic || 0 }); }
function cycPeriod() { try { return Number(localStorage.getItem('ledsim.cyc')) || 60; } catch (e) { return 60; } }

function inputControl(k, dpt) {
  const [main, sub] = dpt.split('.').map(Number);
  const cyc = (S.poll.cyclic || []).find(c => c.nr === k.nr);
  const wrap = h('div', { class: 'ctl' });
  const cur = S.kovals[k.nr];
  if (main === 1) {
    if (sub === 17) { wrap.append(h('button', { onclick: () => sendKo(k.nr, dpt, 1) }, 'Auslösen')); return wrap; }
    const b = cur?.raw ? parseInt(cur.raw.slice(0, 2), 16) & 1 : null;
    const txt = sub === 1 ? ['Aus', 'Ein'] : ['0', '1'];
    wrap.append(h('div', { class: 'seg' },
      h('button', { class: b === 0 ? 'active' : '', onclick: () => sendKo(k.nr, dpt, 0) }, txt[0]),
      h('button', { class: b === 1 ? 'active' : '', onclick: () => sendKo(k.nr, dpt, 1) }, txt[1])));
    return wrap;
  }
  if (main === 3) {
    wrap.append(h('div', { class: 'seg' },
      h('button', { title: 'dunkler (Start, Schritt 100 %)', onclick: () => sendKo(k.nr, dpt, 1) }, '▼'),
      h('button', { title: 'Stopp', onclick: () => sendKo(k.nr, dpt, 0) }, '■'),
      h('button', { title: 'heller (Start, Schritt 100 %)', onclick: () => sendKo(k.nr, dpt, 9) }, '▲')),
      h('div', { class: 'seg' },
        h('button', { title: 'Schritt 25 % dunkler', onclick: () => sendKo(k.nr, dpt, 3) }, '−25'),
        h('button', { title: 'Schritt 25 % heller', onclick: () => sendKo(k.nr, dpt, 11) }, '+25')));
    return wrap;
  }
  if (main === 5 && (sub === 1 || sub === 3)) {
    const max = sub === 1 ? 100 : 360;
    const b0 = cur?.raw ? parseInt(cur.raw.slice(0, 2), 16) : 0;
    const start = Math.round(b0 * max / 255);
    const unit = sub === 1 ? ' %' : '°';
    const out = h('span', { class: 'mono', style: 'width:3.6em;text-align:right' }, start + unit);
    const rng = h('input', { type: 'range', min: 0, max, step: 1, value: start, title: 'Wert beim Loslassen senden',
      oninput: e => { out.textContent = e.target.value + unit; }, onchange: e => sendKo(k.nr, dpt, e.target.value) });
    wrap.append(rng, out,
      sub === 1 ? h('div', { class: 'seg' }, ...[0, 50, 100].map(x => h('button', { onclick: () => sendKo(k.nr, dpt, x) }, x))) : null);
    return wrap;
  }
  if (main === 7 && sub === 600) {
    const inp = h('input', { type: 'number', min: 1000, max: 10000, step: 100, value: cur?.raw ? parseInt(cur.raw, 16) : 4000,
      onkeydown: e => { if (e.key === 'Enter') sendKo(k.nr, dpt, inp.value); } });
    wrap.append(inp, h('span', { class: 'unit' }, 'K'), h('button', { onclick: () => sendKo(k.nr, dpt, inp.value) }, 'senden'));
    return wrap;
  }
  if (main === 17 || main === 18) {
    const inp = h('input', { type: 'number', min: 1, max: 64, value: 1, style: 'width:4em',
      onkeydown: e => { if (e.key === 'Enter') sendKo(k.nr, dpt, Number(inp.value) - 1); } });
    wrap.append(h('span', { class: 'unit' }, 'Szene'), inp, h('button', { onclick: () => sendKo(k.nr, dpt, Number(inp.value) - 1) }, 'abrufen'));
    return wrap;
  }
  if (main === 232) {
    if (/HSV/.test(k.name)) {
      const f = (ph, max) => h('input', { type: 'number', min: 0, max, placeholder: ph, title: ph, style: 'width:4.2em' });
      const hh = f('H°', 360), ss = f('S %', 100), vv = f('V %', 100);
      const send = () => {
        const bytes = [Number(hh.value) * 255 / 360, Number(ss.value) * 255 / 100, Number(vv.value) * 255 / 100].map(x => Math.max(0, Math.min(255, Math.round(x || 0))));
        sendKo(k.nr, dpt, '0x' + bytes.map(hex2).join(''));
      };
      wrap.append(hh, ss, vv, h('button', { onclick: send }, 'senden'));
      return wrap;
    }
    const col = h('input', { type: 'color', value: koColor(k.nr) || '#ff8000' });
    wrap.append(col, h('button', { onclick: () => sendKo(k.nr, dpt, '0x' + col.value.slice(1)) }, 'senden'),
      h('div', { class: 'seg' }, ...[['#FFFFFF', 'weiß'], ['#FF0000', 'rot'], ['#00FF00', 'grün'], ['#0000FF', 'blau'], ['#000000', 'aus']].map(([c, t]) =>
        h('button', { title: t, onclick: () => sendKo(k.nr, dpt, '0x' + c.slice(1)) }, h('span', { class: 'swatch', style: `background:${c}` })))));
    return wrap;
  }
  if (main === 16) {
    const inp = h('input', { placeholder: 'Text', size: 10, maxlength: 14 });
    wrap.append(inp, h('button', { onclick: () => sendKo(k.nr, dpt, inp.value) }, 'senden'));
    return wrap;
  }
  if (main === 10 || main === 11 || main === 19) {
    wrap.append(h('span', { class: 'muted', title: 'Die Uhrzeit des Geräts stellt der Reiter „Hardware“' }, 'über Hardware › Uhrzeit'));
    return wrap;
  }
  const inp = h('input', { type: 'number', step: main === 9 || main === 14 ? 0.1 : 1, value: cyc ? cyc.value : '',
    placeholder: 'Wert', onkeydown: e => { if (e.key === 'Enter') send(false); } });
  const send = cyclic => { if (inp.value !== '') sendKo(k.nr, dpt, inp.value.replace(',', '.'), cyclic ? cycPeriod() : 0); };
  wrap.append(inp, h('button', { onclick: () => send(!!cyc), title: 'Senden (Eingabetaste)' }, 'senden'),
    h('button', { class: 'icon' + (cyc ? ' active' : ''), title: cyc ? `sendet alle ${cyc.period} s – klicken zum Beenden` : `zyklisch senden, alle ${cycPeriod()} s`,
      onclick: () => cyc ? act({ action: 'cyclic_off', nr: k.nr }) : send(true) }, '⟳'));
  return wrap;
}

// ------------------------------------------------------------ Bedienung
const SECTION_ORDER = ['Schalten und Helligkeit', 'Farbtemperatur', 'Farbe', 'Szenen', 'Tag/Nacht', 'Weitere'];
function sectionOf(k) {
  const n = k.name.replace(/^(EK|TW|RGB|HCL)_\d+/, '');
  if (/ColorTemp/.test(n)) return 'Farbtemperatur';
  if (/Color(RGB|HSV)|Shade|Saturation|BrightnessV|(Absolute|Relativ)[RGB]$/.test(n)) return 'Farbe';
  if (/Switch|Dim/.test(n)) return 'Schalten und Helligkeit';
  if (/Scene/.test(n)) return 'Szenen';
  if (/DayNight/.test(n)) return 'Tag/Nacht';
  return 'Weitere';
}

function saveSel() { try { localStorage.setItem('ledsim.sel', S.sel); } catch (e) {} }

function channelContext(ch, c) {
  const sim = S.poll.state?.sim;
  const [type, no] = ch.split(' ');
  if (type === 'HCL') {
    const hc = (sim?.hcl || []).find(x => x.no === Number(no));
    const users = (sim?.channels || []).filter(x => x.hcl).map(x => `${x.type} ${x.no}`);
    return h('div', { class: 'chctx' },
      h('span', {}, hc ? (hc.configured ? 'eingerichtet' : h('span', { class: 'wrn' }, 'nicht eingerichtet')) : '—'),
      h('span', {}, `Typ ${hc ? hc.type : '—'}`),
      h('span', {}, `Ortszeit ${sim?.time?.local?.slice(11, 16) || '—'}`),
      h('span', {}, `Sonne ${sim?.time?.sunrise}–${sim?.time?.sunset}`),
      users.length ? h('span', {}, 'HCL aktiv: ' + users.join(', ')) : null);
  }
  if (!c) return h('div', { class: 'chctx' }, h('span', {}, type === 'Gerät' || type === 'OpenKNX' ? '' : 'Kanal läuft nicht (Betriebsmodus oder nicht programmiert)'));
  const roles = c.type === 'EK' ? ['EK'] : c.type === 'TW' ? ['WW', 'KW'] : ['R', 'G', 'B'];
  const { l, el } = lampEl(c, S.poll.state.sim.duty);
  el.classList.add('lampbig');
  return h('div', { class: 'bigctl' }, h('div', { class: 'lamp', style: 'cursor:default;padding:6px' }, el),
    h('div', { class: 'chctx', style: 'border:0;flex:1' },
      h('span', {}, 'Ports ', h('b', {}, c.ports.map((p, i) => `${PORTS[p]} (${roles[i]})`).join(', '))),
      h('span', {}, 'Zustand ', h('b', { 'data-chstate': ch }, chStateText(c))),
      h('span', {}, `Licht ${de(l.level * 100, 1)} %`),
      c.type === 'TW' ? h('span', {}, `Bereich ${c.ww}–${c.kw} K`) : null,
      c.type === 'RGB' ? h('span', {}, `Gamma ${de(c.gamma, 2)}`) : null,
      h('span', {}, `Kennlinie ${c.curve}`),
      c.night ? h('span', { class: 'wrn' }, 'Nacht') : null,
      c.hcl ? h('span', { class: 'okc' }, 'HCL') : null));
}

function renderInputs(force) {
  const vis = [...S.visKos.entries()];
  const sim = S.poll.state?.sim;
  const chans = sim?.channels || [];
  const sig = vis.map(x => x.join(':')).join(',') + '|' + JSON.stringify(S.poll.cyclic) + '|' + JSON.stringify(S.poll.active) +
    '|' + S.sel + '|' + chans.map(c => c.type + c.no + c.ports.join('')).join(',') + '|' + (S.poll.state?.mcu ? 1 : 0);
  if (!force && sig === S.sigInputs) { updateKoValues(); return; }
  if (!force && document.activeElement && $('#tab-inputs').contains(document.activeElement) &&
      ['INPUT', 'SELECT'].includes(document.activeElement.tagName)) { updateKoValues(); return; }
  S.sigInputs = sig;
  S.closed = S.closed || new Set();
  const groups = new Map();
  for (const [nr, dpt] of vis) {
    const k = S.kos[nr];
    if (!k) continue;
    const ch = channelOfKo(k);
    if (!groups.has(ch)) groups.set(ch, { inp: new Map(), out: [] });
    const g = groups.get(ch);
    if (k.inp) {
      const sec = sectionOf(k);
      if (!g.inp.has(sec)) g.inp.set(sec, []);
      g.inp.get(sec).push([k, dpt || k.dpts[0] || '']);
    } else g.out.push([k, dpt || k.dpts[0] || '']);
  }
  const section = (ch, title, body, count) => {
    const key = ch + '|' + title;
    const d = h('details', { open: !S.closed.has(key) },
      h('summary', {}, title, count ? h('span', { class: 'muted' }, ` (${count})`) : null), body);
    d.addEventListener('toggle', () => d.open ? S.closed.delete(key) : S.closed.add(key));
    return d;
  };
  const card = ch => {
    const g = groups.get(ch);
    const c = chans.find(x => `${x.type} ${x.no}` === ch);
    const row = ([k, dpt]) => h('div', { class: 'korow', title: `${k.t} · ${k.f}\nObjekt ${k.nr} · DPT ${dpt}` },
      h('span', { class: 'nr' }, k.nr), h('span', { class: 'lbl' }, koLabel(k)),
      inputControl(k, dpt), h('span', { class: 'val', 'data-ko': k.nr, 'data-dpt': dpt }, koValText(k.nr, dpt)));
    const secs = SECTION_ORDER.filter(s => g.inp.has(s)).map(s => section(ch, s, h('div', {}, ...g.inp.get(s).map(row)), g.inp.get(s).length));
    const outs = g.out.length ? section(ch, 'Ausgänge', h('div', { class: 'outgrid' }, ...g.out.map(([k, dpt]) =>
      h('div', { class: 'outitem', title: `${k.t} · ${k.f}\nObjekt ${k.nr} · DPT ${dpt}` },
        h('span', { class: 'lbl' }, h('span', { class: 'nr' }, k.nr), ' ', koLabel(k)),
        h('b', { 'data-ko': k.nr, 'data-dpt': dpt }, koValText(k.nr, dpt))))), g.out.length) : null;
    return h('div', { class: 'chcard' }, h('h4', {}, chName(ch)), channelContext(ch, c), ...secs, outs);
  };

  const keys = [...groups.keys()].sort((a, b) => chRank(a) - chRank(b));
  if (!keys.includes(S.sel) && S.sel !== '*') S.sel = keys[0];
  const tabbar = h('div', { class: 'subtabs' },
    h('button', { class: S.sel === '*' ? 'active' : '', onclick: () => { S.sel = '*'; saveSel(); renderInputs(true); } }, 'Alle'),
    ...keys.map(ch => {
      const c = chans.find(x => `${x.type} ${x.no}` === ch);
      const l = c ? lightOf(c, sim.duty) : null;
      return h('button', { class: ch === S.sel ? 'active' : '', onclick: () => { S.sel = ch; saveSel(); renderInputs(true); } },
        l ? h('span', { class: 'dot', 'data-dot': ch, style: `--lc:${l.css};--lo:${l.lo.toFixed(3)}` }) : null, chName(ch));
    }));
  const shown = S.sel === '*' ? keys : keys.filter(k => k === S.sel);
  $('#tab-inputs').replaceChildren(
    h('div', { class: 'tools' },
      h('label', { title: 'Gilt für das ⟳ an jedem Zahlenwert' }, 'Zyklisch senden alle ',
        h('input', { type: 'number', min: 1, value: cycPeriod(), style: 'width:5em',
          onchange: e => { try { localStorage.setItem('ledsim.cyc', e.target.value); } catch (x) {} } }), ' s'),
      h('span', { class: 'muted' }, 'Ein Klick auf eine Leuchte rechts wählt ihren Kanal.')),
    keys.length ? tabbar : null,
    keys.length ? h('div', { class: 'chcards' }, ...shown.map(card))
      : h('div', { class: 'emptynote' }, 'Keine Objekte sichtbar. Im Reiter „Parameter“ unter LED › Allgemein das Gerät und den Betriebsmodus setzen und programmieren.'));
  updateKoValues();
}

function updateKoValues() {
  for (const el of $$('[data-ko]')) {
    const nr = Number(el.dataset.ko);
    const txt = koValText(nr, el.dataset.dpt);
    if (el.dataset.txt === txt) continue;
    const first = el.dataset.txt === undefined;
    el.dataset.txt = txt;
    const c = el.dataset.dpt === '232.600' ? koColor(nr) : null;
    el.replaceChildren(...(c ? [h('span', { class: 'swatch', style: `background:${c}` }), txt] : [txt]));
    if (!first) {
      const rowEl = el.closest('tr, .korow, .outitem');
      rowEl?.classList.remove('flash'); void el.offsetWidth; rowEl?.classList.add('flash');
    }
  }
  const sim = S.poll.state?.sim;
  if (!sim) return;
  for (const el of $$('[data-chstate]')) {
    const c = sim.channels.find(x => `${x.type} ${x.no}` === el.dataset.chstate);
    el.textContent = chStateText(c);
  }
  for (const el of $$('[data-dot]')) {
    const c = sim.channels.find(x => `${x.type} ${x.no}` === el.dataset.dot);
    if (!c) continue;
    const l = lightOf(c, sim.duty);
    el.style.setProperty('--lc', l.css); el.style.setProperty('--lo', l.lo.toFixed(3));
  }
  for (const el of $$('#tab-inputs .bigctl .bulb')) {
    const ch = el.closest('.chcard')?.querySelector('[data-chstate]')?.dataset.chstate;
    const c = sim.channels.find(x => `${x.type} ${x.no}` === ch);
    if (!c) continue;
    const l = lightOf(c, sim.duty);
    el.style.setProperty('--lc', l.css); el.style.setProperty('--lo', l.lo.toFixed(3));
  }
}

// ------------------------------------------------------------ Objekte
function renderKos(force) {
  if (!$('#kos-tools')) {
    $('#tab-kos').replaceChildren(
      h('div', { class: 'tools', id: 'kos-tools' },
        h('input', { id: 'kos-filter', placeholder: 'Filter: Nummer oder Name', oninput: () => renderKos(true) }),
        h('select', { id: 'kos-ch', onchange: () => renderKos(true) }, h('option', { value: '' }, 'alle Kanäle')),
        h('select', { id: 'kos-dir', onchange: () => renderKos(true) },
          h('option', { value: '' }, 'Ein- und Ausgänge'), h('option', { value: 'in' }, 'nur Eingänge'), h('option', { value: 'out' }, 'nur Ausgänge'))),
      h('p', { class: 'hint' }, 'Alle Objekte, die die ETS mit der programmierten Parametrierung einblendet. S = schreiben (Eingang), Ü = übertragen (Ausgang). Wert = zuletzt gesendet oder empfangen, dekodiert nach dem DPT der ETS.'),
      h('div', { class: 'scroll' }, h('table', { class: 'grid', id: 'kos-table' })));
  }
  const f = $('#kos-filter').value.trim().toLowerCase(), chSel = $('#kos-ch').value, dir = $('#kos-dir').value;
  const sig = [...S.visKos.keys()].join(',') + '|' + f + '|' + chSel + '|' + dir;
  if (!force && sig === S.sigKos) { updateKoValues(); return; }
  S.sigKos = sig;
  const chans = [...new Set([...S.visKos.keys()].map(nr => S.kos[nr] && channelOfKo(S.kos[nr])).filter(Boolean))].sort((a, b) => chRank(a) - chRank(b));
  const sel = $('#kos-ch');
  if (sel.dataset.sig !== chans.join()) {
    sel.dataset.sig = chans.join();
    sel.replaceChildren(h('option', { value: '' }, 'alle Kanäle'), ...chans.map(c => h('option', { value: c, selected: c === chSel }, c)));
  }
  const rows = [];
  let lastCh = null;
  const list = [...S.visKos.entries()].filter(([nr]) => S.kos[nr])
    .sort((a, b) => chRank(channelOfKo(S.kos[a[0]])) - chRank(channelOfKo(S.kos[b[0]])) || a[0] - b[0]);
  for (const [nr, dpt] of list) {
    const k = S.kos[nr], ch = channelOfKo(k);
    if (chSel && ch !== chSel) continue;
    if (dir === 'in' && !k.inp) continue;
    if (dir === 'out' && !k.out) continue;
    if (f && !(String(nr) === f || k.t.toLowerCase().includes(f) || k.f.toLowerCase().includes(f) || k.name.toLowerCase().includes(f))) continue;
    if (ch !== lastCh) { rows.push(h('tr', { class: 'grouprow' }, h('td', { colspan: 7 }, chName(ch)))); lastCh = ch; }
    const v = S.kovals[nr];
    rows.push(h('tr', {},
      h('td', { class: 'num' }, nr), h('td', {}, koLabel(k), h('div', { class: 'muted' }, k.name)),
      h('td', { class: 'mono' }, dpt), h('td', {}, k.size),
      h('td', {}, (k.inp ? 'S' : '') + (k.out ? 'Ü' : '')),
      h('td', { class: 'num', 'data-ko': nr, 'data-dpt': dpt }, koValText(nr, dpt)),
      h('td', { class: 'mono muted' }, v ? `${v.dir === 'rx' ? '→' : '←'} ${fmtT(v.t)} · ${v.n}×` : '')));
  }
  $('#kos-table').replaceChildren(h('tr', {}, ...['Nr', 'Objekt', 'DPT', 'Größe', 'Flags', 'Wert', 'zuletzt'].map(x => h('th', {}, x))), ...rows);
}

// Parameter: ETS-Ansicht in ets.js (renderParams)

// ------------------------------------------------------------ Hardware
function hwAct(patch) { return act({ action: 'hw', patch }); }

function renderHw(force) {
  const hw = S.poll.hw, sim = S.poll.state?.sim;
  if (!hw || !sim) return;
  const sig = JSON.stringify(hw) + '|' + S.poll.board + '|' + JSON.stringify(portRoles()) + '|' + sim.led.test.active;
  if (!force && sig === S.sigHw) { updateHwLive(); return; }
  if (!force && document.activeElement && $('#tab-hw').contains(document.activeElement) && document.activeElement.tagName === 'INPUT') { updateHwLive(); return; }
  S.sigHw = sig;
  const roles = portRoles();
  const n = sim.ports;
  const rows = [];
  for (let p = 0; p < n; p++) {
    const r = roles[p];
    rows.push(h('tr', {},
      h('td', {}, h('b', {}, PORTS[p]), h('span', { class: 'muted' }, ` · ${p}`)),
      h('td', {}, r ? h('span', {}, h('span', { class: 'swatch', style: `background:${ROLE_COLOR[r.role]}` }), `${chName(r.ch)} ${ROLE_NAME[r.role]}`) : h('span', { class: 'muted' }, 'frei')),
      h('td', {}, h('input', { type: 'number', min: 0, max: 200, step: 0.5, value: hw.portW[p],
        onchange: e => hwAct({ portW: { [p]: Number(e.target.value) } }) }), h('span', { class: 'unit' }, ' W')),
      h('td', {}, h('label', {}, h('input', { type: 'checkbox', checked: !!hw.short[p], onchange: e => hwAct({ short: { [p]: e.target.checked } }) }), ' Kurzschluss')),
      h('td', { class: 'num', 'data-hwduty': p }, ''),
      h('td', {}, h('div', { class: 'seg' },
        h('button', { title: 'ETS-Funktionstest: Port voll ein (Function Property 160/6)', onclick: () => fpTest(p, true) }, 'ein'),
        h('button', { title: 'ETS-Funktionstest: Port aus', onclick: () => fpTest(p, false) }, 'aus')))));
  }
  const num = (key, label, attrs, unit) => h('label', {}, label, h('input', Object.assign({ type: 'number', value: hw[key],
    onchange: e => hwAct({ [key]: Number(e.target.value) }) }, attrs)), unit ? h('span', { class: 'unit' }, unit) : null);
  const chip = (key, label) => h('label', {}, h('input', { type: 'checkbox', checked: hw.chips[key], onchange: e => hwAct({ chips: { [key]: e.target.checked } }) }), label);
  const localNow = () => { const d = new Date(); d.setSeconds(0, 0); return new Date(d - d.getTimezoneOffset() * 60000).toISOString().slice(0, 16); };
  const timeInp = h('input', { type: 'datetime-local', value: (hw.time || '').replace(' ', 'T') || localNow(),
    onchange: e => hwAct({ time: e.target.value.replace('T', ' ') }) });
  $('#tab-hw').replaceChildren(
    h('div', { class: 'hwgrid' },
      h('div', { class: 'card' }, h('h3', {}, 'Versorgung und Messung'),
        h('div', { class: 'hwform' },
          num('supplyV', 'LED-Versorgung', { min: 0, max: 40, step: 0.1 }, ' V'),
          h('label', {}, h('input', { type: 'checkbox', checked: hw.supplyOn, onchange: e => act({ action: 'power', which: 'supply', on: e.target.checked }) }), ' 24 V ein'),
          num('shunt', 'Shunt', { min: 0.001, max: 1, step: 0.001 }, ' Ω'),
          num('ambient', 'Umgebung', { min: -20, max: 80, step: 0.5 }, ' °C')),
        h('p', { class: 'hint', style: 'margin-top:8px' }, 'Der Strom ergibt sich aus Tastgrad × Last ÷ 24 V je Port (Kurzschluss: 10 A), dazu 15 mA Eigenbedarf. Die Spannung sinkt um I × 50 mΩ. Die Gerätetemperatur folgt der Verlustleistung mit 2 min Zeitkonstante.')),
      h('div', { class: 'card' }, h('h3', {}, 'Bausteine (I²C)'),
        h('div', { class: 'hwform' }, chip('pca', ' PCA9685 (PWM)'), chip('ina', ' INA226 (Strom)'), chip('tmp', ' TMP100 (Temperatur)'), chip('oled', ' SSD1306 (Display)')),
        h('p', { class: 'hint', style: 'margin-top:8px' }, 'Ein abgewählter Baustein antwortet nicht mehr (NACK) - wie ein Defekt oder eine lose Verbindung.')),
      h('div', { class: 'card' }, h('h3', {}, 'Uhrzeit und Bus'),
        h('div', { class: 'hwform' },
          h('label', {}, 'Ortszeit beim Start', timeInp),
          h('button', { onclick: () => hwAct({ time: localNow().replace('T', ' ') }) }, 'jetzt'),
          h('label', {}, h('input', { type: 'checkbox', checked: hw.timeValid, onchange: e => hwAct({ timeValid: e.target.checked }) }), ' Uhrzeit gültig (vom Bus empfangen)'),
          h('label', {}, 'Physikalische Adresse', h('input', { value: hw.pa, size: 8, onchange: e => hwAct({ pa: e.target.value }) })),
          h('label', {}, 'Busdip', h('input', { type: 'number', id: 'busdip-ms', min: 10, max: 5000, step: 10, value: S.busdipMs || 200,
            onchange: e => { S.busdipMs = Number(e.target.value); } }), h('span', { class: 'unit' }, ' ms'))),
        h('p', { class: 'hint', style: 'margin-top:8px' }, 'Die Uhrzeit läuft mit der Simulationszeit; sie bestimmt HCL und Sonnenauf-/-untergang. „Projekt laden“ setzt sie zurück.'))),
    h('div', { class: 'card' }, h('h3', {}, 'Ausgänge: Last, Kurzschluss, Funktionstest'),
      h('table', { class: 'grid ports' },
        h('tr', {}, ...['Port', 'Kanal', 'Last', 'Fehler', 'Tastgrad', 'Funktionstest'].map(x => h('th', {}, x))), ...rows),
      h('div', { class: 'tools', style: 'margin-top:8px' },
        h('span', { class: 'muted' }, 'Kanaltest der Firmware (Konsole):'),
        h('button', { onclick: () => act({ action: 'console', text: 'test walk' }) }, 'test walk'),
        h('button', { onclick: () => act({ action: 'console', text: 'test next' }) }, 'next'),
        h('button', { onclick: () => act({ action: 'console', text: 'test prev' }) }, 'prev'),
        h('button', { onclick: () => act({ action: 'console', text: 'test stop' }) }, 'stop'),
        h('span', { class: 'muted', id: 'fp-result' }, ''))));
  updateHwLive();
}

function updateHwLive() {
  const sim = S.poll.state?.sim; if (!sim) return;
  for (const el of $$('[data-hwduty]')) el.textContent = de((sim.duty[Number(el.dataset.hwduty)] || 0) * 100, 1) + ' %';
}

async function fpTest(port, on) {
  const j = await act({ action: 'fp', port, on });
  const r = j.fp;
  const el = $('#fp-result');
  if (el) el.textContent = r ? (r.handled ? `Funktionstest Port ${PORTS[port]} ${on ? 'ein' : 'aus'}: Antwort ${r.res || '—'}` : `Funktionstest: ${r.text || 'nicht behandelt (nicht parametriert?)'}`) : '';
}

// ------------------------------------------------------------ Telegramme, Konsole, Pruefung
function renderBus() {
  const f = $('#bus-filter').value.trim().toLowerCase();
  const rx = $('#bus-rx').checked, tx = $('#bus-tx').checked;
  const rows = [];
  for (let i = S.busRows.length - 1; i >= 0 && rows.length < 400; i--) {
    const e = S.busRows[i];
    if ((e.ev === 'rx' && !rx) || (e.ev === 'ko' && !tx)) continue;
    const k = S.kos[e.nr] || { t: '?', f: '', name: '' };
    if (f && !(String(e.nr) === f || k.t.toLowerCase().includes(f) || k.f.toLowerCase().includes(f) || k.name.toLowerCase().includes(f))) continue;
    const etsDpt = S.visKos.get(e.nr) || e.dpt;
    rows.push(h('tr', { class: e.ev === 'rx' ? 'rx' : 'tx' }, h('td', { class: 'mono' }, fmtTms(e.t)),
      h('td', {}, e.ev === 'rx' ? (e.delivered ? '→ empfangen' : '→ verworfen') : '← gesendet'),
      h('td', { class: 'num' }, e.nr), h('td', {}, `${channelOfKo(k.name ? k : { name: '' })}: ${koLabel(k)}`),
      h('td', { class: 'mono', title: 'DPT des Telegramms (Firmware/Eingabe) · DPT in der ETS' }, e.dpt === etsDpt ? e.dpt : `${e.dpt} · ETS ${etsDpt}`),
      h('td', { class: 'mono' }, e.raw || ''),
      h('td', { class: 'mono' }, fmtKo(e.nr, e, etsDpt))));
  }
  $('#bus-table').replaceChildren(h('tr', {}, ...['Zeit', 'Richtung', 'Nr', 'Objekt', 'DPT', 'Roh', 'Wert'].map(x => h('th', {}, x))), ...rows);
}

function renderLog() {
  if (!S.logDirty) return;
  S.logDirty = false;
  const el = $('#log');
  const atEnd = el.scrollTop + el.clientHeight >= el.scrollHeight - 30;
  el.replaceChildren(...S.logLines.slice(-1500).map(l => h('div', { class: l.cls }, l.text)));
  if (atEnd) el.scrollTop = el.scrollHeight;
}

function renderChecks() {
  $('#checks-table').replaceChildren(h('tr', {}, h('th', {}, 'Zeit'), h('th', {}, 'Art'), h('th', {}, 'Meldung')),
    ...S.checks.slice(-600).reverse().map(c => h('tr', {}, h('td', { class: 'mono' }, fmtT(c.t || 0)),
      h('td', { class: c.cls }, c.kind), h('td', {}, c.text))));
}

// ------------------------------------------------------------ Gesamt
function render() {
  if (!S.poll) return;
  etsApplyWide();
  renderBar();
  renderLamps();
  renderOutputs();
  renderChart();
  renderMeas();
  renderPanel();
  renderModule();
  if (S.tab === 'inputs') renderInputs();
  else if (S.tab === 'params') renderParams();
  else if (S.tab === 'kos') renderKos();
  else if (S.tab === 'bus') renderBus();
  else if (S.tab === 'console') renderLog();
  else if (S.tab === 'hw') renderHw();
  else if (S.tab === 'checks') renderChecks();
}

function resetHistory() { S.pwm = []; S.pwmIdx = 0; S.checks = []; S.busRows = []; S.warnCount = 0; }

function setupUi() {
  $$('#tabs button').forEach(b => b.addEventListener('click', () => {
    S.tab = b.dataset.tab;
    $$('#tabs button').forEach(x => x.classList.toggle('active', x === b));
    $$('.tabbody').forEach(x => x.classList.toggle('hidden', x.id !== 'tab-' + S.tab));
    if (S.tab === 'checks') S.warnCount = 0;
    S.logDirty = true; S.sigInputs = S.sigParams = S.sigKos = S.sigHw = '';
    try { history.replaceState(null, '', '#' + S.tab); } catch (e) {}
    render();
  }));
  // Reiter aus der Adresse (#params, #hw ...): bleibt beim Neuladen erhalten.
  const fromHash = location.hash.slice(1);
  $$('#tabs button').find(b => b.dataset.tab === fromHash)?.click();
  $$('#speed [data-speed]').forEach(b => b.addEventListener('click', () => act({ action: 'speed', value: b.dataset.speed })));
  $$('#speed [data-step]').forEach(b => b.addEventListener('click', () => act({ action: 'step', ms: b.dataset.step })));
  $('#pwr-bus').addEventListener('click', () => act({ action: 'power', which: 'bus', on: !S.poll.state.bus }));
  $('#pwr-supply').addEventListener('click', () => act({ action: 'power', which: 'supply', on: !S.poll.state.supply }));
  $('#btn-busdip').addEventListener('click', () => act({ action: 'busdip', ms: S.busdipMs || 200 }));
  $('#btn-program').addEventListener('click', () => act({ action: 'program' }));
  $('#btn-restart').addEventListener('click', () => act({ action: 'restart' }));
  $('#btn-unload').addEventListener('click', () => { if (confirm('Applikation entladen? Das Gerät startet ohne Parameter, wie neu.')) act({ action: 'unload' }); });
  $$('[data-btn]').forEach(b => b.addEventListener('click', () => act({ action: 'btn', kind: b.dataset.btn })));
  $$('[data-panel]').forEach(b => b.addEventListener('click', () => act({ action: 'panel', button: Number(b.dataset.panel) })));
  $('#board').addEventListener('change', e => act({ action: 'board', board: e.target.value }).then(resetHistory));
  $('#project').addEventListener('change', e => { if (e.target.value) act({ action: 'project_load', name: e.target.value }).then(resetHistory); });
  $('#btn-save').addEventListener('click', () => {
    const name = prompt('Projektname:', S.poll.project || 'Mein Aufbau');
    if (name) act({ action: 'project_save', name });
  });
  $$('#window button').forEach(b => b.addEventListener('click', () => {
    S.win = Number(b.dataset.win); $$('#window button').forEach(x => x.classList.toggle('active', x === b)); renderChart();
  }));
  const sendCmd = text => act({ action: 'console', text, diag: $('#cmd-diag').checked });
  $('#cmd-send').addEventListener('click', () => { const t = $('#cmd').value.trim(); if (t) sendCmd(t); });
  $('#cmd').addEventListener('keydown', e => { if (e.key === 'Enter') { const t = e.target.value.trim(); if (t) sendCmd(t); } });
  $$('[data-cmd]').forEach(b => b.addEventListener('click', () => sendCmd(b.dataset.cmd)));
  $('#log-clear').addEventListener('click', () => { S.logLines = []; S.logDirty = true; renderLog(); });
  $('#bus-clear').addEventListener('click', () => { S.busRows = []; renderBus(); });
  for (const id of ['#bus-filter', '#bus-rx', '#bus-tx']) $(id).addEventListener('input', renderBus);
  window.addEventListener('resize', renderChart);
}

async function main() {
  S.meta = await (await fetch('/api/meta')).json();
  S.params = S.meta.params;
  S.pages = S.meta.pages;
  for (const k of S.meta.kos) S.kos[k.nr] = k;
  setupUi();
  await poll();
  setInterval(poll, 250);
}
main();
