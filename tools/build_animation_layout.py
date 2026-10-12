#!/usr/bin/env python3
"""Build a bounded provider registry from runtime-observed intro source strings."""
import json
from pathlib import Path
from validate_translations import ROOT
from build_manual_layout import c_string


def build(folder):
    evidence=json.loads((ROOT/'analysis/animation-source-runtime-probe.json').read_text())
    tables={}
    for name in ['zh_trans.tsv','zh_para.tsv']:
        for line in (ROOT/'build/tables'/name).read_text().splitlines():
            if '\t' in line:
                source,target=line.split('\t',1)
                tables[source]=target
    sources=[]
    for record in evidence['records']:
        if ' source=' not in record:continue
        source=record.split(' source=',1)[1]
        if source in sources or not any(c.isalpha() for c in source):continue
        if source.strip() not in tables:raise ValueError('Animation source lacks translation: '+source)
        sources.append(source)
    markers='qzxjvkwyfcb dhmnp'.replace(' ','')
    if len(sources)>len(markers):raise ValueError('Intro registry exceeds distinct marker budget')
    records=[]
    for i,source in enumerate(sources):
        target=tables[source.strip()]
        records.append({'source':source,'translation':target,'marker':markers[i],
                        'center':source.startswith(' ') or source=='Grid Sage Games presents',
                        'status_after':i-1 if source.startswith(' ') else -1})
    header='/* Generated from runtime-observed intro sources. */\n'
    header+='typedef struct {const char *source,*text; char marker; int center,status_after;} ZhAnimationText;\n'
    header+='static const ZhAnimationText zh_animation_texts[] = {\n'
    header+=',\n'.join('{'+c_string(r['source'])+','+c_string(r['translation'])+",'"+r['marker']+"',"+str(int(r['center']))+','+str(r['status_after'])+'}' for r in records)
    header+='\n};\n#define ZH_ANIMATION_COUNT (sizeof zh_animation_texts / sizeof zh_animation_texts[0])\n'
    (folder/'zh_animation_data.h').write_text(header)
    report={'status_layout':'follow_translated_label_one_cell_padding_each_side', 'status':'prototype_requires_runtime_verification','scope':'Runtime-observed intro labels only; other animated UI paths remain to implement.','records':records}
    (ROOT/'analysis/animation-layout-build.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
    return report
