#!/usr/bin/env python3
"""Generate source-preserving HUD startup reveal mappings from validated tables."""
import json
from validate_translations import ROOT
from build_manual_layout import c_string
SOURCES=['Systems online...','Loading variables...','CORE=STABLE','CORE=UNSTABLE',
         'INTEGRATION=OK','LOCATION=UNKNOWN','GOAL=EXPLORE','GOAL=ESCAPE']
HELP_SOURCES=['( remaining core integrity )','( available energy )','( available matter )',
              '( system corruption level )','( temperature/heat level )',
              '( movement type and speed )','( time elapsed )','( current location )']
def build(folder):
    table={}
    for name in ['zh_trans.tsv','zh_para.tsv']:
        for row in (ROOT/'build/tables'/name).read_text().splitlines():
            if '\t' in row:
                src,dst=row.split('\t',1);table[src]=dst
    records=[{'source':s,'translation':table[s],'column':1 if s in SOURCES else -1}
             for s in SOURCES+HELP_SOURCES]
    header='/* Generated HUD log mappings. Native history is unchanged. */\n'
    header+='typedef struct {const char *source,*text;int column;} ZhHudText;\nstatic const ZhHudText zh_hud_texts[]={\n'
    header+=',\n'.join('{'+c_string(r['source'])+','+c_string(r['translation'])+','+str(r['column'])+'}' for r in records)
    header+='\n};\n#define ZH_HUD_COUNT (sizeof zh_hud_texts/sizeof zh_hud_texts[0])\n'
    (folder/'zh_hud_data.h').write_text(header)
    report={'status':'requires_runtime_verification','scope':'Eight exact HUD startup/status variants and eight observed help attribute labels. Native source/history preserved.','records':records}
    (ROOT/'analysis/hud-layout-build.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
    return report
