"""Metadaten fuer den Simulator aus der generierten ETS-Applikation.

Liest src/LEDModule.debug.xml (OpenKNXproducer create --Debug) und schreibt meta.json:

  - defaults   Parameterabbild mit allen Vorgabewerten (Hex), so wie es die
               ETS bei einem frisch eingefuegten Geraet herunterladen wuerde
  - params     jeder Parameter mit Adresse, Bitlage, Typ, Wertebereich, Text
  - kos        jedes Kommunikationsobjekt mit Nummer, Text, Groesse, DPT, Flags
  - pages      die ETS-Seiten der Kanaele "OpenKNX", "LED" und "HCL" in
               Dokumentreihenfolge, mit Ueberschriften, Hinweisen, Parametern,
               Objekten und Zuweisungen. Jedes Element traegt die Bedingungen
               seiner <choose>/<when>-Vorfahren, damit die Oberflaeche die
               Sichtbarkeit wie die ETS auswerten kann.

Konfigurationstransfer (UCT) gehoert nicht zur Dimmerlogik und fehlt in den
Seiten. Ins Parameterabbild kommen trotzdem ALLE Vorgabewerte.

Aufruf: python gen_meta.py <LEDModule.debug.xml> <meta.json>
"""

import json
import re
import struct
import sys
import xml.etree.ElementTree as ET

SRC, DST = sys.argv[1], sys.argv[2]
root = ET.parse(SRC).getroot()
NS = root.tag.split('}')[0] + '}'
CHANNELS = ('BASE_Main', 'APP_LED', 'HCL_HCL')
PREFIXES = ('APP_', 'EK_', 'TW_', 'RGB_', 'HCL_')


def tag(e):
    return e.tag[len(NS):] if e.tag.startswith(NS) else e.tag


def find(e, path):
    return e.find('/'.join(NS + p for p in path.split('/')))


app = root.find('.//' + NS + 'ApplicationProgram')
static = find(app, 'Static')
dynamic = find(app, 'Dynamic')


def clean(text):
    """ETS-Platzhalter {{0:Vorgabe}} durch die Vorgabe ersetzen."""
    if text is None:
        return ''
    return re.sub(r'\{\{\d+:([^}]*)\}\}', lambda m: m.group(1).strip(), text).strip()


# ------------------------------------------------------------------ Typen
types = {}
for pt in static.iter(NS + 'ParameterType'):
    t = {'k': 'none', 's': 0}
    for x in pt:
        n = tag(x)
        if n == 'TypeNumber':
            t = {'k': 'bool' if x.get('UIHint') == 'CheckBox' else ('int' if x.get('Type') == 'signedInt' else 'uint'),
                 's': int(x.get('SizeInBit')),
                 'min': float(x.get('minInclusive', 0)), 'max': float(x.get('maxInclusive', 0))}
            if x.get('Increment'):
                t['step'] = float(x.get('Increment'))
        elif n == 'TypeTime':
            t = {'k': 'uint', 's': int(x.get('SizeInBit')), 'tunit': x.get('Unit', ''),
                 'min': float(x.get('minInclusive', 0)), 'max': float(x.get('maxInclusive', 0))}
        elif n == 'TypeRestriction':
            t = {'k': 'enum', 's': int(x.get('SizeInBit')),
                 'enum': [[int(en.get('Value')), en.get('Text')] for en in x if tag(en) == 'Enumeration']}
        elif n == 'TypePicture':
            t = {'k': 'pic', 's': 0, 'pic': x.get('RefId'), 'align': x.get('HorizontalAlignment', 'Left')}
        elif n == 'TypeFloat':
            enc = x.get('Encoding', '')
            t = {'k': 'float', 's': 16 if 'DPT 9' in enc else 32, 'enc': 'dpt9' if 'DPT 9' in enc else 'ieee',
                 'min': float(x.get('minInclusive', 0)), 'max': float(x.get('maxInclusive', 0))}
        elif n == 'TypeColor':
            t = {'k': 'color', 's': 24}
        elif n == 'TypeText':
            t = {'k': 'text', 's': int(x.get('SizeInBit'))}
        else:
            continue
        if x.get('UIHint'):
            t['ui'] = x.get('UIHint')
    types[pt.get('Id')] = t

# ------------------------------------------------------------------ Parameter
params = {}   # id -> dict
order = []


def add_param(p, offset, bit):
    t = types.get(p.get('ParameterType'), {'k': 'none', 's': 0})
    d = dict(n=p.get('Name'), t=clean(p.get('Text')), k=t['k'], s=t['s'],
             o=offset, b=bit, v=p.get('Value'), access=p.get('Access'),
             unit=(p.get('SuffixText') or '').strip())
    for key in ('min', 'max', 'enum', 'enc', 'step', 'ui', 'tunit', 'pic', 'align'):
        if key in t:
            d[key] = t[key]
    params[p.get('Id')] = d
    order.append(p.get('Id'))


for e in find(static, 'Parameters'):
    if tag(e) == 'Union':
        mem = find(e, 'Memory')
        base_o = int(mem.get('Offset')) if mem is not None else None
        base_b = int(mem.get('BitOffset')) if mem is not None else 0
        for p in e:
            if tag(p) == 'Parameter':
                add_param(p, None if base_o is None else base_o + int(p.get('Offset', 0)),
                          base_b + int(p.get('BitOffset', 0)))
    elif tag(e) == 'Parameter':
        mem = find(e, 'Memory')
        if mem is not None:
            add_param(e, int(mem.get('Offset')), int(mem.get('BitOffset', 0)))
        else:
            add_param(e, None, 0)

# ParameterRefs: Ref-Id -> Parameter-Id, mit Text- und Wert-Ueberschreibung
refs = {}
ref_attr = {}   # Ref-Id -> Text/Access der Referenz (gilt je Einblendung, wie in der ETS)
for r in static.iter(NS + 'ParameterRef'):
    pid = r.get('RefId')
    refs[r.get('Id')] = pid
    ref_attr[r.get('Id')] = (r.get('Text'), r.get('Access'))
    if pid in params:
        if r.get('Value') is not None and params[pid].get('refValue') is None:
            params[pid]['refValue'] = r.get('Value')
        if r.get('Text') and not params[pid].get('refText'):
            params[pid]['refText'] = clean(r.get('Text'))


def value_of(p):
    raw = p.get('refValue') if p.get('refValue') is not None else p.get('v')
    if raw is None or raw == '':
        return '' if p['k'] in ('text', 'color') else 0
    if p['k'] in ('text', 'color'):
        return raw
    if p['k'] == 'float':
        return float(raw)
    try:
        return int(raw)
    except ValueError:
        return int(float(raw))


def dpt9(v):
    m = int(round(float(v) * 100))
    e = 0
    while m < -2048 or m > 2047:
        m = int(round(m / 2))
        e += 1
    return (0x8000 if m < 0 else 0) | (e << 11) | (m & 0x7FF)


# ------------------------------------------------------------------ Vorgabeabbild
size = 0
for p in params.values():
    if p['o'] is not None and p['s']:
        size = max(size, p['o'] + (p['b'] + p['s'] + 7) // 8)
image = bytearray(size)


def put_bits(img, offset, bit, bits, value):
    value &= (1 << bits) - 1
    for i in range(bits):
        pos = offset * 8 + bit + i
        mask = 0x80 >> (pos % 8)
        if (value >> (bits - 1 - i)) & 1:
            img[pos // 8] |= mask
        else:
            img[pos // 8] &= ~mask & 0xFF


for pid in order:
    p = params[pid]
    if p['o'] is None or not p['s'] or p['k'] == 'none':
        continue
    val = value_of(p)
    p['d'] = val
    if p['k'] == 'text':
        raw = str(val).encode('latin-1', 'replace')[: p['s'] // 8]
        image[p['o']: p['o'] + len(raw)] = raw
    elif p['k'] == 'color':
        rgb = int(str(val).lstrip('#') or '0', 16)
        image[p['o']: p['o'] + 3] = rgb.to_bytes(3, 'big')
    elif p['k'] == 'float':
        if p.get('enc') == 'dpt9':
            image[p['o']: p['o'] + 2] = dpt9(val).to_bytes(2, 'big')
        else:
            image[p['o']: p['o'] + 4] = struct.pack('>f', float(val))
    else:
        put_bits(image, p['o'], p['b'], p['s'], int(val))

# ------------------------------------------------------------------ Objekte
kos = {}
for c in static.iter(NS + 'ComObject'):
    kos[c.get('Id')] = dict(nr=int(c.get('Number')), name=c.get('Name'),
                            t=clean(c.get('Text')), f=clean(c.get('FunctionText')),
                            size=c.get('ObjectSize'), dpts=[],
                            inp=c.get('WriteFlag') == 'Enabled',
                            out=c.get('TransmitFlag') == 'Enabled',
                            read=c.get('ReadFlag') == 'Enabled')
    if c.get('DatapointType'):
        kos[c.get('Id')]['dpts'].append(c.get('DatapointType'))
korefs = {}
for r in static.iter(NS + 'ComObjectRef'):
    k = kos.get(r.get('RefId'))
    if k is None:
        continue
    korefs[r.get('Id')] = (k['nr'], r.get('DatapointType') or (k['dpts'][0] if k['dpts'] else ''),
                           clean(r.get('Text')) if r.get('Text') else '',
                           clean(r.get('FunctionText')) if r.get('FunctionText') else '')
    if r.get('Text') and not k.get('rt'):
        k['rt'] = clean(r.get('Text'))
    if r.get('FunctionText') and not k.get('rf'):
        k['rf'] = clean(r.get('FunctionText'))
    if r.get('DatapointType') and r.get('DatapointType') not in k['dpts']:
        k['dpts'].append(r.get('DatapointType'))


def dpt_short(d):
    m = re.match(r'DPST-(\d+)-(\d+)', d or '')
    if m:
        return f'{m.group(1)}.{m.group(2)}'
    m = re.match(r'DPT-(\d+)', d or '')
    return f'{m.group(1)}.0' if m else ''


ko_list = []
for k in kos.values():
    if k['name'].startswith(('LOG_', 'UCT_')):
        continue
    ko_list.append(dict(nr=k['nr'], name=k['name'], t=k.get('rt') or k['t'], f=k.get('rf') or k['f'],
                        size=k['size'], dpts=[dpt_short(d) for d in k['dpts']],
                        inp=k['inp'], out=k['out']))
ko_list.sort(key=lambda k: k['nr'])

# ------------------------------------------------------------------ Seiten
used_params = set()
pages = []


def channel_of(name):
    m = re.match(r'(EK|TW|RGB|HCL)_(\d+)', name or '')
    if m:
        return f'{m.group(1)} {m.group(2)}'
    return 'Allgemein'


def walk(e, conds, page, grid, top):
    t = tag(e)
    if t == 'ParameterBlock':
        if e.get('Inline') == 'true':
            for child in e:
                walk(child, conds, page, e.get('Id') + ':' + 'g', top)
            return
        title = clean(e.get('Text'))
        if title:
            text_ref = refs.get(e.get('TextParameterRefId'))
            newpage = dict(title=title, parent=page['title'] if page else '', top=top,
                           cond=conds, items=[], textParam=params[text_ref]['n'] if text_ref in params else None)
            pages.append(newpage)
            for child in e:
                walk(child, conds, newpage, None, top)
            return
        for child in e:
            walk(child, conds, page, grid, top)
        return
    if t == 'choose':
        pid = refs.get(e.get('ParamRefId'))
        pname = params[pid]['n'] if pid in params else None
        tests = [w.get('test') for w in e if tag(w) == 'when' and w.get('test') is not None]
        for w in e:
            if tag(w) != 'when':
                continue
            c = [pname, w.get('test')] if w.get('default') != 'true' else [pname, None, tests]
            for child in w:
                walk(child, conds + [c], page, grid, top)
        return
    if page is None:
        for child in e:
            walk(child, conds, page, grid, top)
        return
    cell = e.get('Cell')
    row = (grid + ':' + cell.split(',')[0]) if grid and cell else None
    if t == 'ParameterRefRef':
        pid = refs.get(e.get('RefId'))
        if pid in params:
            used_params.add(pid)
            page['items'].append(dict(type='p', n=params[pid]['n'], cond=conds, row=row,
                                      indent=int(e.get('IndentLevel', 0))))
    elif t == 'ParameterSeparator':
        hint = e.get('UIHint') or ''
        page['items'].append(dict(type='h' if hint == 'Headline' else ('r' if hint == 'HorizontalRuler'
                                  else ('i' if hint == 'Information' else ('e' if hint == 'Error' else 'l'))),
                                  text=clean(e.get('Text')), cond=conds, row=row))
    elif t == 'ComObjectRefRef':
        ref = korefs.get(e.get('RefId'))
        if ref is not None:
            page['items'].append(dict(type='k', nr=ref[0], dpt=dpt_short(ref[1]), t=ref[2], f=ref[3],
                                      cond=conds))
    elif t == 'Assign':
        tgt = params.get(refs.get(e.get('TargetParamRefRef')), {}).get('n')
        src = params.get(refs.get(e.get('SourceParamRefRef')), {}).get('n')
        page['items'].append(dict(type='a', target=tgt, source=src, value=e.get('Value'), cond=conds))


def walk_channel(ch, conds):
    title = clean(ch.get('Text'))
    root_page = dict(title=title, parent='', top=title, cond=conds, items=[], textParam=None)
    pages.append(root_page)
    for child in ch:
        walk(child, conds, root_page, None, title)


# Kanaele koennen in einem <choose> stehen (Modul aktiv/inaktiv auf der Seite "Module")
for ch in dynamic.iter(NS + 'Channel'):
    if ch.get('Name') in CHANNELS:
        walk_channel(ch, [])

pages = [p for p in pages if p['items']]

# ------------------------------------------------------------------ ETS-Ansicht
# Der Dynamic-Teil als Baum, so wie die ETS ihn darstellt: Kanaele, Seiten,
# Raster, Trenner, Parameter, Schaltflaechen, und choose/when als eigene
# Knoten. Kompakte Schluessel, die Oberflaeche (web/ets.js) wertet sie aus.
ets_params = set()


def ets_walk(e):
    t = tag(e)
    if t == 'Channel':
        return dict(t='ch', n=e.get('Name'), text=e.get('Text') or '', icon=e.get('Icon'), help=e.get('HelpContext'),
                    tp=params.get(refs.get(e.get('TextParameterRefId')), {}).get('n'), kids=ets_kids(e))
    if t == 'ParameterBlock':
        grid = None
        if e.get('Layout') == 'Grid':
            cols = find(e, 'Columns')
            grid = [c.get('Width', '') for c in cols] if cols is not None else []
        return dict(t='pb', id=e.get('Id'), n=e.get('Name'), text=e.get('Text') or '', icon=e.get('Icon'),
                    help=e.get('HelpContext'), inline=e.get('Inline') == 'true', grid=grid, cell=e.get('Cell'),
                    tp=params.get(refs.get(e.get('TextParameterRefId')), {}).get('n'), kids=ets_kids(e))
    if t == 'choose':
        pid = refs.get(e.get('ParamRefId'))
        whens = [dict(test=None if w.get('default') == 'true' else w.get('test'), kids=ets_kids(w))
                 for w in e if tag(w) == 'when']
        return dict(t='c', p=params[pid]['n'] if pid in params else None, w=whens)
    if t == 'ParameterRefRef':
        pid = refs.get(e.get('RefId'))
        if pid not in params:
            return None
        ets_params.add(pid)
        text, access = ref_attr.get(e.get('RefId'), (None, None))
        return dict(t='p', n=params[pid]['n'], text=clean(text) if text else None,
                    acc=access or params[pid].get('access') or 'ReadWrite',
                    help=e.get('HelpContext'), ind=int(e.get('IndentLevel', 0)), cell=e.get('Cell'))
    if t == 'ParameterSeparator':
        return dict(t='s', ui=e.get('UIHint') or '', text=(e.get('Text') or '').strip(), help=e.get('HelpContext'),
                    cell=e.get('Cell'))
    if t == 'Button':
        return dict(t='b', text=e.get('Text') or '', icon=e.get('Icon'), fn=e.get('EventHandler'),
                    args=e.get('EventHandlerParameters'), cell=e.get('Cell'))
    return None     # Objekte, Assign, Rows/Columns: nicht dargestellt


def ets_kids(e):
    return [x for x in (ets_walk(c) for c in e) if x is not None]


def ets_top(e):
    """Kanaele, auch innerhalb von choose (Modul aktiv), ohne Konfigurationstransfer."""
    out = []
    for c in e:
        if tag(c) == 'Channel' and c.get('Name') in CHANNELS:
            out.append(ets_walk(c))
        elif tag(c) == 'choose':
            pid = refs.get(c.get('ParamRefId'))
            whens = [dict(test=None if w.get('default') == 'true' else w.get('test'), kids=ets_top(w))
                     for w in c if tag(w) == 'when']
            whens = [w for w in whens if w['kids']]
            if whens:
                out.append(dict(t='c', p=params[pid]['n'] if pid in params else None, w=whens))
    return out


ets_tree = ets_top(dynamic)
script = find(static, 'Script')
ets = dict(app=app.get('Name'), tree=ets_tree, script=script.text if script is not None else '')

# ------------------------------------------------------------------ Baggages
# Bilder, Icons und Hilfetexte aus der knxprod (dieselben Dateien wie in der ETS)
import io
import os
import zipfile

baggage = {}    # Baggage-Id -> Dateiname
for b in root.iter(NS + 'Baggage'):
    if b.get('Name'):
        baggage[b.get('Id')] = b.get('Name')
for p in params.values():
    if p['k'] == 'pic':
        p['pic'] = baggage.get(p['pic'], p['pic'])

out_dir = os.path.join(os.path.dirname(os.path.abspath(DST)), 'baggage')
knxprod = re.sub(r'\.debug\.xml$', '.knxprod', SRC)
n_files = 0
if os.path.exists(knxprod):
    with zipfile.ZipFile(knxprod) as z:
        for name in z.namelist():
            if '/Baggages/' not in name or name.endswith('/'):
                continue
            base = name.rsplit('/', 1)[1]
            data = z.read(name)
            if base.lower().endswith('.zip'):
                sub = 'help' if base.startswith('Help') else 'icons'
                os.makedirs(os.path.join(out_dir, sub), exist_ok=True)
                with zipfile.ZipFile(io.BytesIO(data)) as zz:
                    for inner in zz.namelist():
                        if not inner.endswith('/'):
                            with open(os.path.join(out_dir, sub, inner.rsplit('/', 1)[-1]), 'wb') as f:
                                f.write(zz.read(inner))
                            n_files += 1
            else:
                os.makedirs(out_dir, exist_ok=True)
                with open(os.path.join(out_dir, base), 'wb') as f:
                    f.write(data)
                n_files += 1
else:
    print(f'Hinweis: {knxprod} fehlt - ETS-Ansicht ohne Bilder, Icons und Hilfetexte')

# ------------------------------------------------------------------ Ausgabe
needed = set()
for pg in pages:
    for c in pg['cond']:
        needed.add(c[0])
    for it in pg['items']:
        for c in it['cond']:
            needed.add(c[0])
        if it['type'] == 'a':
            needed.update(x for x in (it['target'], it['source']) if x)
    if pg.get('textParam'):
        needed.add(pg['textParam'])

keep = {}
for pid, p in params.items():
    # Kanalnamen u. ae. sind reine ETS-Texte ohne Speicherplatz im Geraet: sie
    # kommen mit (fuer Anzeige und Projekt), aber nie ins Parameterabbild.
    ets_only = p['o'] is None
    if p['k'] == 'none':
        continue
    if not (p['n'].startswith(PREFIXES) or pid in used_params or pid in ets_params or p['n'] in needed):
        continue
    if p['n'].startswith(('LOG_', 'UCT_')):
        continue
    if ets_only:
        default = value_of(p)
    else:
        default = p.get('d', 0)
    entry = dict(n=p['n'], t=p.get('refText') or p['t'], k=p['k'], s=p['s'], o=p['o'], b=p['b'],
                 d=default, ch=channel_of(p['n']), hidden=p.get('access') == 'None',
                 unit=p.get('unit', ''), ets=ets_only)
    for key in ('min', 'max', 'enum', 'enc', 'step', 'ui', 'tunit', 'pic', 'align'):
        if key in p:
            entry[key] = p[key]
    keep[p['n']] = entry

meta = dict(source=SRC, paramSize=size, defaults=image.hex(), params=keep, kos=ko_list, pages=pages, ets=ets)
with open(DST, 'w', encoding='utf-8') as f:
    json.dump(meta, f, ensure_ascii=False, separators=(',', ':'))

print(f'meta.json: {len(keep)} Parameter, {len(ko_list)} Objekte, {len(pages)} Seiten, '
      f'Abbild {size} Byte, {n_files} Baggage-Dateien')
