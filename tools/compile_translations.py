#!/usr/bin/env python3
"""Merge reviewed tables and Luna batches, expand exact quoted aliases, track pending."""
import json,re
from pathlib import Path
from display_tags import localize
from display_names import localize_names
from validate_translations import ROOT, UPSTREAM, rows, source_keys, completed_batch

def compile_tables():
    output=ROOT/'build/tables';output.mkdir(parents=True,exist_ok=True)
    summary={}
    for table in ('trans','para'):
        original=source_keys(table)
        manual=rows(ROOT/f'translations/zh_{table}.tsv')
        data={}
        draft_batches=[]
        if table=='trans' and (ROOT/'translations/name-lexicon.json').exists():
            from generate_names import generate
            data.update(generate())
        if table=='trans':
            for p in sorted((ROOT/'translations/short-batches').glob('*.tsv')):
                batch=completed_batch(p,ROOT/'translations/short-worklists')
                if batch is None:
                    draft_batches.append(p.name)
                    continue
                data.update(batch)
        if table=='para':
            for p in sorted((ROOT/'translations/batches').glob('*.tsv')):
                batch=completed_batch(p,ROOT/'translations/worklists')
                if batch is None:
                    draft_batches.append(p.name)
                    continue
                data.update(batch)
        data.update(manual)
        if table=='para':
            glossary_path=ROOT/'translations/prose-glossary.json'
            if glossary_path.exists():
                glossary=json.loads(glossary_path.read_text())
                terms=re.compile(r'(?<![A-Za-z0-9_.-])('+ '|'.join(map(re.escape,sorted(glossary,key=len,reverse=True)))+r')(?![A-Za-z0-9_.-])')
                # Tags and native capture references remain byte-for-byte intact.
                for src,dst in data.items():
                    pieces=re.split(r'(<[a-z_]+>|\{(?:n|\*|[1-9])\})',dst)
                    data[src]=''.join(piece if i%2 else terms.sub(lambda m:glossary[m[0]],piece) for i,piece in enumerate(pieces))
        unknown=set(data)-set(original)
        if unknown: raise ValueError(f'Unknown English keys: {list(unknown)[:3]}')
        def unquote(text):
            return text[1:-1] if text.startswith('"') and text.endswith('"') else text
        if table=='para':
            # Older batches translated the speaker-prefixed form first.
            # Derive the same bubble text without consuming a body capture.
            for src,dst in list(data.items()):
                sm=re.fullmatch(r'\{\*\}: "(.*)"',src)
                dm=re.fullmatch(r'\{(?:\*|1)\}[:：]\s*["“](.*)["”]',dst)
                if not sm or not dm:continue
                body=sm[1]
                if body not in original and '"'+body+'"' not in original:continue
                translated=dm[1]
                if re.search(r'\{1\}',translated):continue
                translated=re.sub(r'\{([2-9])\}',lambda m:'{'+str(int(m[1])-1)+'}',translated)
                if body in original:data.setdefault(body,translated)
                if '"'+body+'"' in original:data.setdefault('"'+body+'"','"'+translated+'"')
        canonical=dict(data)
        canonical.update({unquote(s):unquote(d) for s,d in data.items()})
        aliases=0
        speaker_aliases=0
        if table=='para':
            # A speech bubble, quoted log, and captured speaker share one translation.
            for src in original:
                if src in data: continue
                match=re.fullmatch(r'(\{\*\}|[A-Za-z0-9][A-Za-z0-9 ._\-]*): "(.*)"',src)
                if not match: continue
                speaker,body=match.groups()
                dst=canonical.get(body)
                if dst is None: continue
                shift=1 if speaker=='{*}' else 0
                position=[0]
                indices=[]
                def reference(m):
                    if m[1].isdigit(): index=int(m[1])
                    else: position[0]+=1;index=position[0]
                    index+=shift;indices.append(index)
                    return '{'+str(index)+'}'
                dst=re.sub(r'\{(n|\*|[1-9])\}',reference,dst)
                if indices and max(indices)>9: continue
                data[src]=('{1}' if shift else speaker)+': "'+dst+'"'
                speaker_aliases+=1
        for src in original:
            key=unquote(src)
            if src not in data and key in canonical:
                quoted=src!=key
                data[src]=('"' if quoted else '')+canonical[key]+('"' if quoted else '');aliases+=1
        if table=='para' and glossary_path.exists():
            for src,dst in data.items():
                pieces=re.split(r'(<[a-z_]+>|\{(?:n|\*|[1-9])\})',dst)
                data[src]=''.join(piece if i%2 else terms.sub(lambda m:glossary[m[0]],piece) for i,piece in enumerate(pieces))
        data={src:localize_names(localize(dst)) for src,dst in data.items()}
        (output/f'zh_{table}.tsv').write_text(''.join(s+'\t'+data[s]+'\n' for s in original if s in data))
        pending=[s for s in original if s not in data]
        (ROOT/f'analysis/pending-{table}.json').write_text(json.dumps(pending,ensure_ascii=False,indent=2)+'\n')
        summary[table]={'upstream_rules':len(rows(UPSTREAM/f'kr_{table}.tsv')),'source_rules':len(original),'translated_rules':len(data),
                        'manual_rules':len(manual),'draft_batches_excluded':draft_batches,'quoted_aliases_added':aliases,'speaker_aliases_added':speaker_aliases,'pending_rules':len(pending)}
    (ROOT/'analysis/translation-progress.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2)+'\n')
    return output
if __name__=='__main__': print(compile_tables())
