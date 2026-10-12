"""Localize visible angle-bracket indicators; never modify matching sources."""
import json
import re
from pathlib import Path

LABELS = json.loads((Path(__file__).resolve().parents[1] / 'translations/display-tags.json').read_text())['labels']
TAG = re.compile(r'<[a-z_\u3400-\u9fff]+(?:,\s*[a-z_\u3400-\u9fff]+)*>')

def localize(text):
    def replace(match):
        names = match[0][1:-1].split(',')
        if not any(name.strip() in LABELS for name in names):
            return match[0]
        return '<' + ', '.join(LABELS.get(name.strip(), name.strip()) for name in names) + '>'
    return TAG.sub(replace, text)

def indicators(text):
    result=[]
    for tag in TAG.findall(text):
        names=[name.strip() for name in tag[1:-1].split(',')]
        if all(name in LABELS.values() or re.fullmatch('[a-z_]+',name) for name in names):
            result.append(tag)
    return sorted(result)
