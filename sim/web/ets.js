'use strict';
// ETS-Ansicht der Parameter: stellt den Dynamic-Teil der Applikation so dar
// wie die ETS 6 - Kanalbaum mit Seiten, Raster-Layouts, Optionsfelder bis zwei
// Werte, Hinweis-/Fehlerboxen, Bilder, Schaltflaechen mit dem ETS-Skript der
// Applikation, Kontexthilfe aus den Hilfetexten der knxprod.
// Daten: meta.ets (gen_meta.py), Werte: S.values (app.js).

const ETS = {
  sel: null, open: new Set(), sig: '', helpKey: null, helpCache: {}, status: '',
  hl: (() => { try { return localStorage.getItem('ledsim.ets.hl') === '1'; } catch (e) { return false; } })(),
  help: (() => { try { return localStorage.getItem('ledsim.ets.help') !== '0'; } catch (e) { return true; } })(),
  wide: (() => { try { return localStorage.getItem('ledsim.ets.wide') === '1'; } catch (e) { return false; } })(),
  fns: null,
};
const GRID_BASE = 532;      // Breite der Parameterspalte in px, wie in der ETS

function etsStore(key, v) { try { localStorage.setItem(key, v); } catch (e) {} }

// ------------------------------------------------------------ Auswertung
/// Kinder eines Knotens mit aufgeloesten choose/when (alle zutreffenden when,
/// der default-Zweig nur, wenn kein anderer passt - wie die ETS).
function etsVisible(kids, vals) {
  const out = [];
  for (const k of kids || []) {
    if (k.t !== 'c') { out.push(k); continue; }
    const v = vals[k.p];
    const hits = k.w.filter(w => w.test !== null && w.test !== undefined && evalTest(v, w.test));
    const use = hits.length ? hits : k.w.filter(w => w.test === null || w.test === undefined);
    for (const w of use) out.push(...etsVisible(w.kids, vals));
  }
  return out;
}

const isPage = n => n.t === 'pb' && !n.inline && n.text;

/// Unterseiten eines Knotens (durch choose und Inline-Bloecke hindurch).
function etsPagesOf(node, vals) {
  const out = [];
  for (const k of etsVisible(node.kids, vals)) {
    if (isPage(k)) out.push(k);
    else if (k.t === 'pb') out.push(...etsPagesOf(k, vals));
  }
  return out;
}

/// Text mit ETS-Platzhalter {{0:Vorgabe}}: Wert des Textparameters, sonst die Vorgabe.
function etsText(text, tp) {
  const v = tp ? S.values[tp] : '';
  return String(text || '').replace(/\{\{\d+(?::([^}]*))?\}\}/g, (m, d) => (v !== undefined && v !== null && String(v).trim()) ? String(v) : (d ?? ''));
}

function etsIcon(name, cls = 'ets-ico') {
  return name ? h('img', { class: cls, src: `baggage/icons/${name}.png`, alt: '', onerror: e => { e.target.style.visibility = 'hidden'; } }) : h('span', { class: cls });
}

const etsChanged = n => {
  const p = S.params[n];
  return p && String(S.values[n]).toUpperCase() !== String(p.d).toUpperCase();
};

/// Alle sichtbaren Parameter einer Seite (ohne Unterseiten), fuer die Hervorhebung.
function etsParamsOf(node, vals, out = []) {
  for (const k of etsVisible(node.kids, vals)) {
    if (k.t === 'p') out.push(k.n);
    else if (k.t === 'pb' && !isPage(k)) etsParamsOf(k, vals, out);
  }
  return out;
}

// ------------------------------------------------------------ Baum
function etsTree() {
  const vals = S.values;
  const chans = etsVisible(S.meta.ets.tree, vals).filter(n => n.t === 'ch');
  const pend = diffValues();
  const rows = [];
  const pageRow = (pg, depth, path) => {
    const subs = etsPagesOf(pg, vals);
    const open = ETS.open.has(pg.id);
    const ps = etsParamsOf(pg, vals);
    const chg = ETS.hl && ps.some(etsChanged);
    const pending = ps.some(n => n in pend);
    rows.push(h('div', { class: `ets-node ets-page d${depth}` + (ETS.sel === pg.id ? ' sel' : '') + (chg ? ' chg' : ''),
      onclick: e => { if (e.target.classList.contains('ets-tog')) return; ETS.sel = pg.id; if (subs.length) ETS.open.add(pg.id); ETS.helpKey = null; renderParams(true); } },
      h('span', { class: 'ets-tog', onclick: () => { open ? ETS.open.delete(pg.id) : ETS.open.add(pg.id); renderParams(true); } }, subs.length ? (open ? '−' : '+') : ''),
      etsIcon(pg.icon), h('span', { class: 'ets-lbl' }, etsText(pg.text, pg.tp)),
      pending ? h('span', { class: 'ets-pend', title: 'Änderungen noch nicht programmiert' }) : null));
    if (open) for (const s of subs) pageRow(s, depth + 1, path.concat(pg));
  };
  for (const ch of chans) {
    const open = !ETS.open.has('!' + ch.n);      // Kanaele sind offen, bis man sie zuklappt
    rows.push(h('div', { class: 'ets-node ets-ch', onclick: () => { open ? ETS.open.add('!' + ch.n) : ETS.open.delete('!' + ch.n); renderParams(true); } },
      h('span', { class: 'ets-tog' }, open ? '−' : '+'), etsIcon(ch.icon), h('span', { class: 'ets-lbl' }, etsText(ch.text, ch.tp))));
    if (open) for (const pg of etsPagesOf(ch, vals)) pageRow(pg, 1, [ch]);
  }
  return rows;
}

/// Pfad zur ausgewaehlten Seite: [Kanal, Seite, Unterseite ...]
function etsPath(id) {
  const vals = S.values;
  const find = (node, path) => {
    for (const pg of etsPagesOf(node, vals)) {
      if (pg.id === id) return path.concat(pg);
      const r = find(pg, path.concat(pg));
      if (r) return r;
    }
    return null;
  };
  for (const ch of etsVisible(S.meta.ets.tree, vals).filter(n => n.t === 'ch')) {
    const r = find(ch, [ch]);
    if (r) return r;
  }
  return null;
}

function etsFirstPage() {
  const vals = S.values;
  const chans = etsVisible(S.meta.ets.tree, vals).filter(n => n.t === 'ch');
  const led = chans.find(c => c.n === 'APP_LED') || chans[0];
  return led ? etsPagesOf(led, vals)[0] : null;
}

// ------------------------------------------------------------ Seite
/// Wert setzen; die Antwort erzwingt das Neuzeichnen, auch wenn das geklickte
/// Element noch den Fokus hat (sonst kaemen abhaengige Parameter erst beim
/// naechsten Klick).
function etsSet(name, value) { return act({ action: 'param', name, value }).then(j => { ETS.force = true; return j; }); }

/// Eingabeelement eines Parameters, wie die ETS es zeigt.
function etsControl(ref, p, inCell) {
  const name = ref.n, cur = S.values[name];
  const ro = ref.acc === 'Read';
  const id = 'ets_' + name;
  const unit = p.unit ? h('span', { class: 'ets-unit' }, p.unit) : null;
  if (p.k === 'enum') {
    if (p.ui !== 'DropDown' && p.enum.length <= 2) {
      return h('div', { class: 'ets-radios', role: 'radiogroup' }, ...p.enum.map(([v, t]) =>
        h('label', { class: 'ets-radio' }, h('input', { type: 'radio', name: id, value: v, checked: Number(cur) === v, disabled: ro,
          onchange: () => etsSet(name, v) }), h('span', {}, t))));
    }
    return h('div', { class: 'ets-sel' }, h('select', { id, disabled: ro, onchange: e => etsSet(name, e.target.value) },
      ...p.enum.map(([v, t]) => h('option', { value: v, selected: Number(cur) === v }, t))));
  }
  if (p.k === 'bool') {
    return h('label', { class: 'ets-check' }, h('input', { type: 'checkbox', id, checked: Number(cur) === 1, disabled: ro,
      onchange: e => etsSet(name, e.target.checked ? 1 : 0) }));
  }
  if (p.k === 'pic') {
    return h('div', { class: 'ets-pic ' + (p.align || 'Left').toLowerCase() }, h('img', { src: 'baggage/' + p.pic, alt: p.t }));
  }
  if (p.k === 'color') {
    const txt = h('input', { class: 'ets-in', value: String(cur || '').toUpperCase(), disabled: ro, size: 8,
      onchange: e => { const v = e.target.value.trim(); if (/^#?[0-9a-f]{6}$/i.test(v)) etsSet(name, v.startsWith('#') ? v : '#' + v); else e.target.classList.add('bad'); } });
    return h('div', { class: 'ets-color' }, h('input', { type: 'color', value: String(cur || '#000000').toLowerCase(), disabled: ro,
      onchange: e => etsSet(name, e.target.value) }), txt);
  }
  if (p.k === 'text') {
    return h('input', { class: 'ets-in', id, value: cur ?? '', disabled: ro, maxlength: Math.floor(p.s / 8) || undefined,
      onchange: e => etsSet(name, e.target.value) });
  }
  if (p.ui === 'Time_hhmm') {
    const mins = Number(cur) || 0;
    const txt = `${String(Math.floor(mins / 60)).padStart(2, '0')}:${String(mins % 60).padStart(2, '0')}`;
    return h('div', { class: 'ets-num' }, h('input', { class: 'ets-in', id, value: txt, disabled: ro, title: 'hh:mm',
      onchange: e => {
        const m = e.target.value.trim().match(/^(\d{1,2}):(\d{2})$/);
        const v = m ? Number(m[1]) * 60 + Number(m[2]) : NaN;
        if (Number.isNaN(v) || v < p.min || v > p.max) { e.target.classList.add('bad'); return; }
        etsSet(name, v);
      } }), unit);
  }
  // Zahlen (ganzzahlig und Gleitkomma)
  const isFloat = p.k === 'float';
  const show = isFloat ? Number(cur).toLocaleString('de-DE', { maximumFractionDigits: 6, useGrouping: false }) : cur;
  const range = p.min !== undefined ? `${de(p.min, isFloat ? 2 : 0)} … ${de(p.max, isFloat ? 2 : 0)}` : '';
  const inp = h('input', { class: 'ets-in', id, value: show, disabled: ro, title: range,
    type: isFloat ? 'text' : 'number', min: p.min, max: p.max, step: isFloat ? undefined : (p.step || 1),
    onchange: e => {
      const v = Number(String(e.target.value).replace(',', '.'));
      if (e.target.value === '' || Number.isNaN(v) || (p.min !== undefined && (v < p.min || v > p.max)) || (!isFloat && !Number.isInteger(v))) {
        e.target.classList.add('bad'); e.target.title = 'erlaubt: ' + range; return;
      }
      e.target.classList.remove('bad');
      etsSet(name, v);
    } });
  return h('div', { class: 'ets-num' + (inCell ? ' cell' : '') }, inp, unit);
}

function etsParamRow(ref, inCell) {
  const p = S.params[ref.n];
  if (!p || ref.acc === 'None') return null;
  const pend = diffValues();
  const cls = 'ets-p' + (ETS.hl && etsChanged(ref.n) ? ' chg' : '') + (ref.n in pend ? ' pend' : '') + (ref.acc === 'Read' ? ' ro' : '');
  const text = etsText(ref.text || p.t, null);
  const help = { 'data-help': ref.help || undefined, title: `${text}\n${ref.n}` };
  // Bilder wie in der ETS: ausserhalb eines Rasters in der Wertespalte, im Raster ueber die Zelle
  if (p.k === 'pic') return h('div', Object.assign({ class: cls + ' pic' + (inCell ? ' cell' : '') }, help), inCell ? null : h('span'), etsControl(ref, p, inCell));
  if (inCell) return h('div', Object.assign({ class: cls + ' cell' }, help), etsControl(ref, p, true));
  return h('div', Object.assign({ class: cls, style: ref.ind ? `--ind:${ref.ind}` : '' }, help),
    h('label', { class: 'ets-pl', for: 'ets_' + ref.n }, text), h('div', { class: 'ets-pv' }, etsControl(ref, p, false)));
}

function etsSeparator(s, inCell) {
  const help = { 'data-help': s.help || undefined };
  switch (s.ui) {
    case 'Headline': return h('div', Object.assign({ class: 'ets-head' + (inCell ? ' cell' : '') }, help), s.text);
    case 'HorizontalRuler': return h('hr', { class: 'ets-hr' });
    case 'Information': return h('div', Object.assign({ class: 'ets-info' }, help), h('span', { class: 'ets-i' }, 'i'), h('span', {}, s.text));
    case 'Error': return h('div', Object.assign({ class: 'ets-err' }, help), h('span', { class: 'ets-i' }, '!'), h('span', {}, s.text));
    default: return s.text ? h('div', Object.assign({ class: 'ets-sep' + (inCell ? ' cell' : '') }, help), s.text) : null;
  }
}

function etsButton(b) {
  return h('div', { class: 'ets-btnrow' }, h('button', { class: 'ets-btn', onclick: () => etsRunButton(b) }, etsIcon(b.icon, 'ets-bico'), b.text));
}

function etsGrid(pb) {
  const cols = (pb.grid || []).map(w => /%$/.test(w) ? `${(parseFloat(w) / 100 * GRID_BASE).toFixed(0)}px` : (w ? `${parseInt(w, 10)}px` : 'auto'));
  const grid = h('div', { class: 'ets-grid', 'data-help': pb.help || undefined, style: `grid-template-columns:${cols.join(' ') || 'auto'}` });
  for (const k of etsFlat(pb.kids)) {
    const el = etsItem(k, true);
    if (!el) continue;
    const [r, c] = String(k.cell || '').split(',').map(Number);
    if (r && c) el.style.gridArea = `${r} / ${c}`;
    grid.append(el);
  }
  return grid;
}

/// Sichtbare Elemente; Inline-Bloecke ohne Raster gehen in der Folge auf.
function etsFlat(kids) {
  const out = [];
  for (const k of etsVisible(kids, S.values)) {
    if (k.t === 'pb' && !isPage(k) && !k.grid) out.push(...etsFlat(k.kids));
    else out.push(k);
  }
  return out;
}

function etsItem(k, inCell) {
  if (k.t === 'p') return etsParamRow(k, inCell);
  if (k.t === 's') return etsSeparator(k, inCell);
  if (k.t === 'b') return etsButton(k);
  if (k.t === 'pb' && k.grid) return etsGrid(k);
  return null;
}

function etsPage(pg) {
  const body = h('div', { class: 'ets-pagebody' });
  for (const k of etsFlat(pg.kids)) {
    if (isPage(k)) continue;
    const el = etsItem(k, false);
    if (el) body.append(el);
  }
  return body;
}

// ------------------------------------------------------------ Kontexthilfe
function etsMarkdown(src) {
  const esc = s => s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
  const inline = s => esc(s).replace(/\*\*([^*]+)\*\*/g, '<b>$1</b>').replace(/`([^`]+)`/g, '<code>$1</code>')
    .replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2" target="_blank" rel="noopener">$1</a>')
    .replace(/(^|[^*])\*([^*]+)\*/g, '$1<i>$2</i>');
  const out = [];
  let list = null, para = [];
  const flush = () => {
    if (para.length) { out.push('<p>' + inline(para.join(' ')) + '</p>'); para = []; }
    if (list) { out.push('<ul>' + list.map(x => '<li>' + inline(x) + '</li>').join('') + '</ul>'); list = null; }
  };
  for (const raw of src.replace(/^﻿/, '').split(/\r?\n/)) {
    const line = raw.trimEnd();
    let m;
    if (!line.trim()) { flush(); continue; }
    if ((m = line.match(/^(#{1,6})\s+(.*)$/))) { flush(); out.push(`<h${Math.min(m[1].length + 1, 6)}>${inline(m[2])}</h${Math.min(m[1].length + 1, 6)}>`); continue; }
    if ((m = line.match(/^\s*[*-]\s+(.*)$/))) { if (para.length) { out.push('<p>' + inline(para.join(' ')) + '</p>'); para = []; } (list = list || []).push(m[1]); continue; }
    if (list && /^\s/.test(line)) { list[list.length - 1] += ' ' + line.trim(); continue; }     // eingerueckte Fortsetzung
    if (list) flush();
    para.push(line.trim());
  }
  flush();
  return out.join('');
}

async function etsShowHelp(key) {
  const box = $('#ets-help');
  if (!box || !ETS.help) return;
  if (!key) { box.replaceChildren(h('div', { class: 'ets-nohelp' }, 'Keine Hilfe zu diesem Element.')); return; }
  if (ETS.helpCache[key] === undefined) {
    try {
      const r = await fetch(`baggage/help/${encodeURIComponent(key)}.txt`);
      ETS.helpCache[key] = r.ok ? await r.text() : '';
    } catch (e) { ETS.helpCache[key] = ''; }
  }
  if (ETS.helpKey !== key && ETS.helpKey !== null) return;     // inzwischen anderes gewaehlt
  const txt = ETS.helpCache[key];
  box.innerHTML = txt ? etsMarkdown(txt) : '';
  if (!txt) box.replaceChildren(h('div', { class: 'ets-nohelp' }, 'Keine Hilfe zu diesem Element.'));
}

// ------------------------------------------------------------ Schaltflaechen (ETS-Skript)
/// Fuehrt den EventHandler einer Schaltflaeche mit dem Skript der Applikation
/// aus. Nachgebildet ist der Teil der ETS-Skript-API, den OpenKNX-Skripte
/// nutzen: device.getParameterByName, online.invokeFunctionProperty,
/// progress.setText. Die Function Property geht an den Kern - wie am Geraet.
function etsRunButton(b) {
  const setStatus = (t, cls = '') => { ETS.status = t; const el = $('#ets-status'); if (el) { el.textContent = t; el.className = 'ets-status ' + cls; } };
  try {
    if (!ETS.fns) ETS.fns = new Function(S.meta.ets.script + '\n;return function (__n) { return eval(__n); };')();
    const fn = ETS.fns(b.fn);
    const param = n => {
      if (!(n in S.params)) throw new Error(`Parameter ${n} unbekannt`);
      return { get value() { return S.values[n]; }, set value(v) { S.values[n] = v; etsSet(n, v); }, name: n };
    };
    const device = { getParameterByName: param, getParameter: param };
    const call = (obj, prop, data) => {
      const x = new XMLHttpRequest();
      x.open('POST', '/api/action', false);     // die ETS-Skript-API ist synchron
      x.send(JSON.stringify({ action: 'fp_raw', obj, prop, data: Array.from(data || []) }));
      const j = JSON.parse(x.responseText);
      if (!j.ok) throw new Error(j.error);
      if (!j.handled) throw new Error(j.text || `Gerät beantwortet Function Property ${obj}/${prop} nicht (programmiert und Bus an?)`);
      return j.res;
    };
    const online = { connect() {}, disconnect() {}, invokeFunctionProperty: call, readFunctionPropertyState: call };
    const progress = { setText: t => setStatus(t), setProgress() {} };
    let ctx = {};
    try { ctx = b.args ? JSON.parse(b.args) : {}; } catch (e) {}
    setStatus('…');
    fn(device, online, progress, ctx);
    if (ETS.status === '…') setStatus(`${b.text}: ausgeführt`, 'ok');
    else setStatus(ETS.status, 'ok');
  } catch (e) {
    setStatus(String(e.message || e), 'bad');
  }
  setTimeout(poll, 30);
}

// ------------------------------------------------------------ Gesamt
function etsApplyWide() {
  document.body.classList.toggle('ets-wide', ETS.wide && S.tab === 'params');
}

function renderParams(force) {
  etsApplyWide();
  const box = $('#tab-params');
  const sig = JSON.stringify(S.poll.pending) + '|' + JSON.stringify(S.poll.active) + '|' + ETS.sel + '|' + [...ETS.open].join(',') + '|' + ETS.hl + ETS.help + ETS.wide;
  if (!force && sig === ETS.sig) return;
  const ae = document.activeElement;
  const typing = ae && box.contains(ae) && ae.tagName === 'INPUT' && !['checkbox', 'radio', 'color'].includes(ae.type);
  if (ETS.force) { force = true; ETS.force = false; }
  else if (!force && typing) return;      // nicht mitten in eine Eingabe hinein neu zeichnen
  // Fokus ueber das Neuzeichnen retten (Tastaturbedienung)
  const keep = ae && box.contains(ae) ? { id: ae.id, name: ae.name, value: ae.value } : null;
  ETS.sig = sig;
  let path = ETS.sel && etsPath(ETS.sel);
  if (!path) { const f = etsFirstPage(); ETS.sel = f?.id; path = f && etsPath(f.id); }
  const pg = path ? path[path.length - 1] : null;
  for (const n of (path || []).slice(1, -1)) ETS.open.add(n.id);
  const n = Object.keys(diffValues()).length;
  const scroll = $('#ets-content')?.scrollTop || 0;
  const treeScroll = $('#ets-tree')?.scrollTop || 0;
  const tbtn = (label, on, onclick, title) => h('button', { class: 'ets-tb' + (on ? ' on' : ''), onclick, title }, label);
  box.replaceChildren(h('div', { class: 'ets' + (ETS.help ? '' : ' nohelp') },
    h('div', { class: 'ets-bar' },
      h('div', { class: 'ets-crumb' }, [S.meta.ets.app, ...(path || []).map(x => etsText(x.text, x.tp))].join('  >  ')),
      h('span', { class: 'ets-status', id: 'ets-status' }, ETS.status),
      h('button', { class: 'ets-tb prog' + (n ? ' dirty' : ''), onclick: () => act({ action: 'program' }), title: 'Parameter wie die ETS herunterladen, mit Neustart' },
        n ? `Programmieren (${n})` : 'Programmieren'),
      tbtn('Verwerfen', false, () => act({ action: 'params_revert' }), 'Nicht programmierte Änderungen verwerfen'),
      tbtn('Kontexthilfe', ETS.help, () => { ETS.help = !ETS.help; etsStore('ledsim.ets.help', ETS.help ? '1' : '0'); renderParams(true); }, 'Kontexthilfe ein-/ausblenden'),
      tbtn('Änderungen hervorheben', ETS.hl, () => { ETS.hl = !ETS.hl; etsStore('ledsim.ets.hl', ETS.hl ? '1' : '0'); renderParams(true); }, 'Von der Vorgabe abweichende Parameter und Seiten gelb hervorheben'),
      tbtn('Standardwerte', false, () => { if (confirm('Alle Parameter auf Standardwerte setzen?')) act({ action: 'params_defaults' }); }, 'Alle Parameter auf Standardwerte zurücksetzen'),
      tbtn(ETS.wide ? '⇥ schmal' : '⇤ breit', false, () => { ETS.wide = !ETS.wide; etsStore('ledsim.ets.wide', ETS.wide ? '1' : '0'); renderParams(true); render(); }, 'Parameter über die ganze Fensterbreite')),
    h('div', { class: 'ets-main' },
      h('div', { class: 'ets-tree', id: 'ets-tree' }, ...etsTree()),
      h('div', { class: 'ets-right' },
        h('div', { class: 'ets-content', id: 'ets-content',
          onfocusin: e => etsHelpFrom(e.target), onclick: e => etsHelpFrom(e.target) },
          pg ? h('div', { class: 'ets-page' }, etsPage(pg)) : h('div', { class: 'ets-nohelp' }, 'Keine Seite sichtbar.')),
        ETS.help ? h('div', { class: 'ets-help', id: 'ets-help' }) : null))));
  $('#ets-content').scrollTop = scroll;
  $('#ets-tree').scrollTop = treeScroll;
  if (keep) {
    const el = keep.id ? document.getElementById(keep.id)
      : keep.name ? [...box.querySelectorAll(`input[name="${CSS.escape(keep.name)}"]`)].find(x => x.value === keep.value) : null;
    el?.focus({ preventScroll: true });
  }
  etsShowHelp(ETS.helpKey || pg?.help || path?.[0]?.help);
}

function etsHelpFrom(el) {
  const holder = el.closest('[data-help]');
  const key = holder?.dataset.help;
  const path = ETS.sel && etsPath(ETS.sel);
  const pageHelp = path?.[path.length - 1]?.help || path?.[0]?.help;
  ETS.helpKey = key || null;
  etsShowHelp(key || pageHelp);
}
