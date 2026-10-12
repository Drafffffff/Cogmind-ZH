#!/usr/bin/env python3
"""Check Chinese tables against the pinned upstream matcher and English keys."""
import contextlib
import importlib.util
import io
import json
from pathlib import Path
import re
from display_tags import indicators, localize

ROOT = Path(__file__).resolve().parents[1]
UPSTREAM = ROOT / 'upstream/KR-AOS-Cogmind/Cogmind-KR_PDS'


def rows(path, text=None):
    result = {}
    if text is None: text=path.read_text(encoding='utf-8')
    for n, line in enumerate(text.splitlines(), 1):
        if not line or line.startswith('#'):
            continue
        if line.count('\t') != 1:
            raise ValueError(f'{path.name}:{n}: expected one tab')
        src, dst = line.split('\t')
        if not src or not dst or src in result:
            raise ValueError(f'{path.name}:{n}: empty or duplicate entry')
        result[src] = dst
    return result




def completed_batch(path, worklist_root):
    text=path.read_text(encoding='utf-8')
    worklist=worklist_root/path.with_suffix('.json').name
    expected=set(json.loads(worklist.read_text())) if worklist.exists() else None
    lines=[line for line in text.splitlines() if line and not line.startswith('#')]
    if expected is not None and (not text.endswith('\n') or len(lines)!=len(expected)):
        return None
    data=rows(path,text)
    if expected is not None and set(data)!=expected:return None
    return data


def source_keys(table):
    original=rows(UPSTREAM/f'kr_{table}.tsv')
    registry=ROOT/'translations/resource-sources.json'
    if registry.exists():
        for entry in json.loads(registry.read_text()).get(table,[]):
            raw=entry['raw'].lstrip('/').strip()
            expected=re.sub(r'%[0-9]|\{[A-Za-z_][A-Za-z0-9_]*\}','{*}',raw).replace('[name]','{*}')
            if entry['source']!=expected:
                raise ValueError('Resource source differs from documented native substitution: '+entry['source'])
            original.setdefault(entry['source'],'')
    native=ROOT/'translations/native-sources.json'
    if native.exists():
        document=json.loads(native.read_text())
        for entry in document.get(table,[]):
            pieces=[]
            for component in entry['components']:
                if set(component)=={'literal'}:
                    pieces.append(component['literal'])
                elif set(component)=={'capture'} and component['capture'] in ('*','n'):
                    pieces.append('{'+component['capture']+'}')
                else:
                    raise ValueError('Invalid native composite component: '+str(component))
            expected=''.join(pieces).strip()
            if entry.get('normalization')=='paragraph_whitespace':
                expected=re.sub(r'\s+', ' ', expected)
            if entry['source']!=expected or not entry.get('evidence'):
                raise ValueError('Native composite differs from documented components: '+entry['source'])
            original.setdefault(entry['source'],'')
    return original


def validate():
    from compile_translations import compile_tables
    tables = compile_tables()
    spec = importlib.util.spec_from_file_location('upstream_checker', UPSTREAM/'check_tsv.py')
    checker = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(checker)
    report = {'status': 'passed', 'tables': {}}
    for name, check in [('trans', checker.check_trans), ('para', checker.check_para)]:
        path = tables/f'zh_{name}.tsv'
        data = rows(path)
        original = source_keys(name)
        for src, dst in data.items():
            if src not in original:
                raise ValueError(f'Unknown upstream key: {src}')
            if re.search(r'[\uac00-\ud7a3\u1100-\u11ff\u3130-\u318f]', dst):
                raise ValueError(f'Unexpected Korean in translation: {src}')
            if any(ord(c) > 0xffff for c in dst):
                raise ValueError(f'Unsupported non-BMP character: {src}')
            tokens = checker.TOKRE.findall(src)
            refs = checker.TOKRE.findall(dst)
            indices = []
            positional = 0
            for token in refs:
                if token.isdigit():
                    indices.append(int(token))
                else:
                    positional += 1
                    indices.append(positional)
            if set(indices) != set(range(1, len(tokens)+1)):
                raise ValueError(f'Missing or invalid capture: {src}')
            if indicators(localize(src)) != indicators(dst):
                raise ValueError(f'Changed display indicator semantics: {src}')
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            errors = check(str(path))
        warnings = output.getvalue().count('[!]')
        if errors or warnings or '[X]' in output.getvalue():
            raise ValueError(output.getvalue())
        report['tables'][name] = {'translated_rules': len(data),
                                 'upstream_rules': len(rows(UPSTREAM/f'kr_{name}.tsv')),
                                 'verified_resource_rules':len(original)-len(rows(UPSTREAM/f'kr_{name}.tsv')),
                                 'source_rules':len(original),
                                 'errors': errors, 'width_warnings': warnings}
    report['total_rules'] = sum(x['translated_rules'] for x in report['tables'].values())
    return report


if __name__ == '__main__':
    report = validate()
    output = ROOT/'analysis/translation-validation.json'
    output.write_text(json.dumps(report, ensure_ascii=False, indent=2)+'\n')
    print(json.dumps(report, ensure_ascii=False, indent=2))
