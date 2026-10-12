#!/usr/bin/env python3
"""Translate known item/entity names and only their existing upstream variants."""
import json,re,collections
from validate_translations import ROOT,UPSTREAM,rows,completed_batch

def cells(s):
    return sum(2 if (0x1100<=ord(c)<=0x115f or 0x2e80<=ord(c)<=0xa4cf or 0xac00<=ord(c)<=0xd7a3 or 0xf900<=ord(c)<=0xfaff or 0xfe30<=ord(c)<=0xfe6f or 0xff00<=ord(c)<=0xff60 or 0xffe0<=ord(c)<=0xffe6) else 1 for c in s)

def generate():
    lex=json.loads((ROOT/'translations/name-lexicon.json').read_text())
    maps=[]
    for short in (False,True):
        d={k.casefold():v for k,v in lex['G'].items()}
        if short:d.update({k.casefold():v for k,v in lex['SHORT'].items()})
        d.update({k.casefold():v for k,v in lex['PHRASE'].items()})
        d.update({k.casefold()+'.':v for k,v in lex['PREFIX'].items()})
        for k in lex['KEEP']:d.setdefault(k.casefold(),k)
        maps.append(d)
    known=rows(UPSTREAM/'names/names.tsv')
    original=rows(UPSTREAM/'kr_trans.tsv')
    pattern=re.compile(r'(\[\{\*\}\] |)(.*?)(?:((?: \(\{n\}\))*!?(?: \[!?)*(?: \[\{\*\}\])?))')
    def form(src):
        quoted=src.startswith('"') and src.endswith('"')
        match=pattern.fullmatch(src[1:-1] if quoted else src)
        if not match:return None
        pre,core,post=match.groups()
        return ('"' if quoted else '')+pre,core,post+('"' if quoted else '')
    candidates=set(known)
    for src in original:
        m=form(src)
        if not m:continue
        core=m[1].strip('()')
        # English title/all-caps item labels, never arbitrary dialogue/sentences.
        if core and all(w=='of' or w[0].isupper() or w[0].isdigit() for w in core.split()) and not re.search(r'[{}:,;!?]',core):
            candidates.add(core)
    def translate(name,d):
        if ' of ' in name:
            first,last=name.split(' of ',1);name=last+' '+first
        chunks=[];i=0
        keys=sorted(d,key=len,reverse=True)
        while i<len(name):
            tail=name[i:];fold=tail.casefold()
            matched=False
            for key in keys:
                if fold.startswith(key) and (len(tail)==len(key) or not tail[len(key)].isalnum()):
                    chunks.append(d[key]);i+=len(key);matched=True;break
            if matched:continue
            model=re.match(r"(?:[A-Z0-9][A-Z0-9.-]*\d[A-Z0-9.-]*|[A-Z0-9]{1,3}-[A-Z0-9]{2,5})(?:'s)?",tail,re.I)
            roman=re.match(r'(?:I{1,3}|IV|VI{0,3}|IX|XI{0,2})(?=$|[^A-Za-z])',tail)
            if model or roman:
                m=model or roman;chunks.append(m[0]);i+=len(m[0]);continue
            if tail.startswith("'s"):
                chunks.append('的');i+=2;continue
            if tail.startswith('of '):chunks.append('之');i+=3;continue
            if not tail[0].isalpha() or tail[0].isdigit():
                chunks.append(tail[0]);i+=1;continue
            return None
        return ''.join(chunks)
    base={};skipped=[]
    for src in sorted(candidates):
        name=src.removesuffix(' [').strip().strip('()')
        if name.casefold() in base:continue
        for d in maps:
            dst=translate(name,d)
            if dst is None:break
            # Spaces between Chinese components are unnecessary in an item name.
            dst=re.sub(r'(?<=[\u3400-\u9fff]) +| +(?=[\u3400-\u9fff])','',dst)
            if cells(dst)<=cells(name):
                base[name.casefold()]=dst;break
        else:dst=None
        if name.casefold() not in base:skipped.append(name)
    overrides={}
    for p in sorted((ROOT/'translations/short-batches').glob('*.tsv')):
        batch=completed_batch(p,ROOT/'translations/short-worklists')
        if batch is None:continue
        overrides.update(batch)
    overrides.update(rows(ROOT/'translations/zh_trans.tsv'))
    preserved=set()
    for path in sorted((ROOT/'translations/short-batches').glob('*.keep.json')):
        for entry in json.loads(path.read_text()):
            src=entry['source']
            if src in overrides and overrides[src].casefold()==src.casefold():
                preserved.add(src.casefold())
                parsed=form(src)
                if parsed and parsed[1] in overrides and overrides[parsed[1]].casefold()==parsed[1].casefold():
                    preserved.add(parsed[1].casefold())
    for src,dst in overrides.items():
        if not src.endswith(' ['):
            base[src.casefold()]=dst
    # The game also displays all-caps and compact forms of the same names.
    aliases=dict(base)
    for k,v in base.items():aliases.setdefault(k.replace(' ',''),v)
    generated={}
    for src in original:
        # Decorations carry captures and must not be translated or renumbered.
        m=form(src)
        if not m:continue
        pre,name,post=m; paren=name.startswith('(') and name.endswith(')')
        core=name[1:-1] if paren else name
        dst=aliases.get(core.casefold())
        if dst is None:continue
        # Unchanged aliases require an explicit, reviewed preservation record.
        if dst.casefold()==core.casefold() and core.casefold() not in preserved:continue
        if paren:dst='('+dst+')'
        if post.startswith(' [') and '{' not in name:
            dst+=' '*max(0,cells(name)-cells(dst))
        candidate=pre+dst+post
        if cells(candidate)<=cells(src):generated[src]=candidate
    output=ROOT/'build/tables';output.mkdir(parents=True,exist_ok=True)
    (output/'names.tsv').write_text(''.join(k+'\t'+v+'\n' for k,v in generated.items()))
    report={'known_name_forms':len(known),'translated_base_names':len(base),'generated_rules':len(generated),
            'skipped_base_names':skipped}
    (ROOT/'analysis/name-generation.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
    return generated
if __name__=='__main__':print(len(generate()))
