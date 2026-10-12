"""Apply reviewed non-expanding readable names to visible translated text."""
import json
import re
from pathlib import Path

NAMES=json.loads((Path(__file__).resolve().parents[1]/'translations/display-names.json').read_text())['names']
LOOKUP={key.casefold():value for key,value in NAMES.items()}
PATTERN=re.compile(r'(?<![A-Za-z0-9_.-])('+'|'.join(map(re.escape,sorted(NAMES,key=len,reverse=True)))+r')(?![A-Za-z0-9_.-])',re.I)

def localize_names(text):
    return PATTERN.sub(lambda match:LOOKUP[match[0].casefold()],text)
