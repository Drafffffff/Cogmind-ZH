#!/usr/bin/env python3
"""Generate a Chinese-width ASCII manual provider for a dedicated DLL decoder.

Never installs files. Missing translations stop the build. The generated proxy
keeps the verified native chapter/subheading grammar and provides prewrapped
rows, so native scroll counts can follow Chinese content rather than English.
Runtime compatibility still requires an in-game acceptance check.
"""
import ctypes
import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path

from model_native_manual import parse
from validate_translations import ROOT, UPSTREAM


def digest(data):
    return hashlib.sha256(data).hexdigest()


def c_string(text):
    return '"' + ''.join(f'\\{b:03o}' for b in text.encode('utf-8')) + '"'


def core_layout(build):
    """Use the same C cell-width/wrapping implementation as the installed DLL."""
    source = build / 'manual_core.c'
    source.write_text('#include "cmkr_core.h"\n'
                      'int zh_cells(const char*s){return cm_cells(s);}\n'
                      'int zh_wrap(const char*s,int width,char*out,int cap)'
                      '{return (int)(cm_wrap(s,width,out,cap)-s);}\n')
    compiler = shutil.which('clang') or shutil.which('cc')
    if not compiler:
        raise RuntimeError('C compiler required for authoritative width calculation')
    library = build / 'manual_core.so'
    subprocess.run([compiler, '-shared', '-fPIC', '-O1', '-I', str(UPSTREAM),
                    str(source), '-o', str(library)], check=True)
    lib = ctypes.CDLL(str(library))
    lib.zh_cells.argtypes = [ctypes.c_char_p]
    lib.zh_cells.restype = ctypes.c_int
    lib.zh_wrap.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
    lib.zh_wrap.restype = ctypes.c_int
    return lib


def wrap(lib, text, width=68):
    text = text.replace(r'\n', '\x01').replace(r'\<', '')
    pending = text.encode('utf-8')
    result = []
    hard_after = []
    soft_gaps = []
    while pending:
        buf = ctypes.create_string_buffer(4096)
        consumed = lib.zh_wrap(pending, width, buf, len(buf))
        if not 0 < consumed <= len(pending):
            raise ValueError('Chinese wrapping made no progress')
        line = buf.value.decode('utf-8')
        # Keep literal shortcuts/configuration keys intact when the C wrapper
        # reaches a cell boundary inside a printable ASCII token.
        is_token = lambda c: 33 <= c <= 126
        if consumed < len(pending) and is_token(pending[consumed - 1]) and is_token(pending[consumed]):
            boundary = consumed
            while boundary and is_token(pending[boundary - 1]):
                boundary -= 1
            if boundary:
                consumed = boundary
                line = pending[:boundary].decode('utf-8').rstrip()
        if lib.zh_cells(line.encode('utf-8')) > width:
            raise ValueError('Wrapped line exceeds native viewport')
        result.append(line)
        # cm_wrap stops immediately before CM_BR. Remember that boundary so
        # Chinese punctuation repair never moves text across an explicit break.
        hard = pending[consumed:consumed + 1] == b'\x01'
        hard_after.append(hard)
        raw_chunk = pending[:consumed]
        trailing_spaces = len(raw_chunk) - len(raw_chunk.rstrip(b' '))
        if hard or not pending[consumed:]:
            soft_gaps.append('')
        else:
            # cm_wrap consumes a run of spaces between words and emits one
            # separator when the next word is placed. Keep that separator so
            # moving the boundary cannot merge ASCII words.
            soft_gaps.append(' ' if trailing_spaces else '')
        pending = pending[consumed:]
    # Repair only soft-wrap runs. Rebalance the same number of lines, choosing
    # the legal set of codepoint boundaries closest to the native wrapping.
    opening = set('（【《“‘；')
    closing = set('。，、；：！？）】》”’')
    run_start = 0
    for boundary_index in range(len(result)):
        if boundary_index != len(result) - 1 and not hard_after[boundary_index]:
            continue
        run_end = boundary_index + 1
        run = result[run_start:run_end]
        gaps = soft_gaps[run_start:run_end - 1]
        if len(run) > 1 and (any(gaps) or any(
                (a.rstrip() and a.rstrip()[-1] in opening) or
                (b.lstrip() and b.lstrip()[0] in closing)
                for a, b in zip(run, run[1:]))):
            joined = ''.join(row + (gaps[i] if i < len(gaps) else '')
                             for i, row in enumerate(run))
            chars = list(joined)
            old_cuts = []
            total = 0
            for i, row in enumerate(run[:-1]):
                total += len(row)
                old_cuts.append(total)
                total += len(gaps[i])
            cells = [lib.zh_cells(ch.encode('utf-8')) for ch in chars]
            prefix = [0]
            for cell in cells:
                prefix.append(prefix[-1] + cell)

            def ascii_token_split(cut):
                return (0 < cut < len(chars) and
                        33 <= ord(chars[cut - 1]) <= 126 and
                        33 <= ord(chars[cut]) <= 126)

            from functools import lru_cache

            @lru_cache(None)
            def choose(row_index, start):
                if row_index == len(run) - 1:
                    if start >= len(chars) or prefix[-1] - prefix[start] > width:
                        return None
                    left_index = start - 1
                    while left_index >= 0 and chars[left_index].isspace():
                        left_index -= 1
                    right_index = start
                    while right_index < len(chars) and chars[right_index].isspace():
                        right_index += 1
                    if (left_index >= 0 and chars[left_index] in opening) or (
                            right_index < len(chars) and chars[right_index] in closing):
                        return None
                    if ascii_token_split(start):
                        return None
                    return (0, (len(chars),))
                best = None
                remaining_rows = len(run) - row_index - 1
                for end in range(start + 1, len(chars) - remaining_rows + 1):
                    if prefix[end] - prefix[start] > width:
                        break
                    left_index = end - 1
                    while left_index >= start and chars[left_index].isspace():
                        left_index -= 1
                    right_index = end
                    while right_index < len(chars) and chars[right_index].isspace():
                        right_index += 1
                    if ((left_index >= start and chars[left_index] in opening) or
                            (right_index < len(chars) and chars[right_index] in closing)):
                        continue
                    if ascii_token_split(end):
                        continue
                    tail = choose(row_index + 1, end)
                    if tail is None:
                        continue
                    cost = abs(end - old_cuts[row_index]) + tail[0]
                    candidate = (cost, (end,) + tail[1])
                    if best is None or candidate < best:
                        best = candidate
                return best

            selected = choose(0, 0)
            if selected is not None:
                cuts = selected[1]
                points = (0,) + cuts
                result[run_start:run_end] = [
                    ''.join(chars[points[i]:points[i + 1]])
                    for i in range(len(run))]
            elif any(gaps):
                raise ValueError('Could not preserve soft-wrap spaces within the row width')
        run_start = run_end
    # Rejoin soft rows without a separator and explicit-break rows with CM_BR;
    # this checks spaces and all other source characters exactly.
    rebuilt = ''.join(line + ('\x01' if hard_after[i] else '')
                      for i, line in enumerate(result))
    normalized = text.replace(r'\n', '\x01').replace(r'\<', '')
    parts = normalized.split('\x01')
    normalized_parts = []
    for i, part in enumerate(parts):
        indent = ''
        if i:
            indent = part[:len(part) - len(part.lstrip(' '))]
            part = part[len(indent):]
        else:
            part = part.lstrip(' ')
        # Native cm_wrap keeps a single separator between words, but collapses
        # runs of ordinary spaces. Indentation immediately after CM_BR is an
        # explicit layout instruction and is retained above.
        part = re.sub(r' {2,}', ' ', part.rstrip(' '))
        normalized_parts.append(indent + part)
    normalized = '\x01'.join(normalized_parts)
    if rebuilt != normalized:
        raise ValueError(f'Chinese wrapping lost or changed source characters: {rebuilt!r} != {normalized!r}')
    if any(lib.zh_cells(line.encode('utf-8')) > width for line in result):
        raise ValueError('Wrapped line exceeds native viewport after punctuation repair')
    return result


def translation_items(raw, audit, review):
    original = raw.decode('utf-8').splitlines()
    items = {}
    for item in audit['resolved_items']:
        target = item.get('translation')
        if target is None:
            selected = next((c for c in item['checks']
                             if c['mode'] == item['resolved_by'] and c['status'] == 'preserved'), None)
            if selected is None:
                raise ValueError('Resolved item lacks complete text: ' + item['item_id'])
            target = selected['candidate']
        if item['source'] != '\n'.join(original[item['line_start'] - 1:item['line_end']]):
            raise ValueError('Audit source changed: ' + item['item_id'])
        items[item['line_start']] = (item['line_end'], target, 'existing_table')
    for item in review.get('layout_overrides', []):
        first, last = item['line_start'], item['line_end']
        source = '\n'.join(original[first - 1:last])
        if item['source'] != source:
            raise ValueError(f'Override source changed at lines {first}-{last}')
        if not item.get('translation') or item.get('kind') not in ('translated', 'preserved'):
            raise ValueError(f'Incomplete reviewed override at line {first}')
        for line in range(first, last + 1):
            items.pop(line, None)
        items[first] = (last, item['translation'], 'reviewed_override')
    covered = {line for first, (last, _, _) in items.items() for line in range(first, last + 1)}
    unresolved = [item['item_id'] for item in audit['unresolved_items']
                  if not all(line in covered for line in range(item['line_start'], item['line_end'] + 1))]
    if unresolved:
        raise ValueError('Manual has unresolved items: ' + ', '.join(unresolved))
    return items


def build():
    folder = ROOT / 'build/zh-manual'
    folder.mkdir(parents=True, exist_ok=True)
    raw = (ROOT / 'original/bazzite/manual.txt').read_bytes()
    audit_path = ROOT / 'analysis/manual-translation-coverage.json'
    review_path = ROOT / 'translations/manual-gap-candidates.json'
    audit = json.loads(audit_path.read_text())
    review = json.loads(review_path.read_text())
    items = translation_items(raw, audit, review)
    chapters, _ = parse(raw)
    lib = core_layout(folder)
    records = []
    proxy = ['Chinese manual layout provider. Requires the matching Chinese DLL.', '']
    consumed = set()

    def encode(text, first, last, kind):
        cells = lib.zh_cells(text.encode('utf-8'))
        if cells > 68 or len(text.encode('utf-8')) >= 200:
            raise ValueError(f'Invalid decoder row width/length at line {first}')
        ident = len(records) + 1
        prefix = f'ZHm{ident:06X}'
        token = prefix + 'X' * max(0, cells - len(prefix))
        records.append({'id': ident, 'token': token, 'translation': text,
                        'source_line_start': first, 'source_line_end': last,
                        'kind': kind, 'cells': cells})
        return token

    for chapter in chapters:
        line = chapter['title_line']
        last, target, _ = items[line]
        if lib.zh_cells(target.encode('utf-8')) > 68:
            raise ValueError('Chapter title cannot be wrapped')
        proxy += ['-' * 65, ' ' + encode(target, line, last, 'chapter'), '-' * 65, '']
        consumed.add(line)
        for row in chapter['rows']:
            line = row['source_line']
            if line is None:
                continue  # Native parser itself inserts the subheading gap.
            if not row['text'].strip():
                if proxy[-1] != '':
                    proxy.append('')
                continue
            if line in consumed:
                continue
            if line not in items:
                raise ValueError(f'Native manual row has no logical translation at line {line}')
            last, target, _ = items[line]
            consumed.update(range(line, last + 1))
            if row['kind'] == 'subheading':
                if proxy[-1] == '':
                    proxy.pop()
                proxy += [' ' + encode(target, line, last, 'subheading'), '-' * 65]
            else:
                proxy.extend(encode(text, line, last, 'body') for text in wrap(lib, target) if text)
    payload = ('\r\n'.join(proxy) + '\r\n').encode('ascii')
    parsed, _ = parse(payload)
    if len(parsed) != len(chapters):
        raise ValueError('Proxy changed chapter count')
    lookup = {r['token']: r for r in records}
    visible_tokens = [c['title'] for c in parsed] + [r['text'] for c in parsed for r in c['rows'] if r['text']]
    if sorted(visible_tokens) != sorted(lookup):
        raise ValueError('Native grammar lost, duplicated, or changed proxy rows')
    (folder / 'manual.txt').write_bytes(payload)
    header = ('/* Generated by tools/build_manual_layout.py; do not edit. */\n'
              'typedef struct {const char *token, *text;} ZhManualRow;\n'
              'static const ZhManualRow zh_manual_rows[] = {\n' +
              ',\n'.join('{' + c_string(r['token']) + ',' + c_string(r['translation']) + '}' for r in records) +
              '\n};\n#define ZH_MANUAL_COUNT (sizeof(zh_manual_rows)/sizeof(zh_manual_rows[0]))\n')
    (folder / 'zh_manual_data.h').write_text(header)
    manifest = {'status': 'prototype_not_installed_or_runtime_verified',
                'source_sha256': digest(raw), 'audit_sha256': digest(audit_path.read_bytes()),
                'review_sha256': digest(review_path.read_bytes()), 'proxy_sha256': digest(payload),
                'chapter_count': len(chapters), 'subheading_count': sum(len(c['subheading_rows']) for c in parsed),
                'translated_rows': len(records), 'native_body_rows': sum(len(c['rows']) for c in parsed),
                'records': records}
    (folder / 'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n')
    (ROOT / 'analysis/manual-layout-build.json').write_text(json.dumps({k: v for k, v in manifest.items() if k != 'records'}, ensure_ascii=False, indent=2) + '\n')
    print(json.dumps({k: v for k, v in manifest.items() if k != 'records'}, ensure_ascii=False))


if __name__ == '__main__':
    build()
