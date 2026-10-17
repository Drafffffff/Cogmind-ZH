#!/usr/bin/env python3
"""Host regression for the narrow nested map-label child-console renderer."""
from __future__ import annotations
import hashlib, json, re, subprocess, tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
HELPER=ROOT/'runtime/zh_map_label_draw.h'
RULES=ROOT/'output/Cogmind-ZH-manual-preview/zh_trans.tsv'
SOURCE_FIXTURE=ROOT/'analysis/map-label-probe-runtime.json'
REPORT=ROOT/'analysis/map-label-draw-tests.json'

def cstr(s:str)->str:
    return '"'+s.replace('\\','\\\\').replace('"','\\"').replace('\n','\\n')+'"'

def cell_width(s:str)->int:
    return sum(2 if ord(ch)>0x2e7f else 1 for ch in s)

def main()->None:
    sources=[]
    for item in json.loads(SOURCE_FIXTURE.read_text(encoding='utf-8'))['source_observations']:
        sources.append(item['source'].strip())
    assert len(sources)==15 and all(sources)
    table=[]
    for line in RULES.read_text(encoding='utf-8').splitlines():
        if not line or line.startswith('#') or '\t' not in line: continue
        a,b=line.split('\t',1);table.append((a,b))
    compiled=[]
    for src in sources:
        matches=[]
        for a,b in table:
            pat=re.escape(a)
            pat=pat.replace(re.escape('{*}'),r'([0-9]+)').replace(re.escape('{n}'),r'([0-9]+)')
            m=re.fullmatch(pat,src)
            if m: matches.append((a,b,m.groups()))
        # Repeated log samples of a label should resolve to one exact loaded rule.
        assert len(matches)==1,(src,matches)
        a,b,caps=matches[0]
        expected=b
        for cap in caps: expected=expected.replace('{*}',cap,1).replace('{n}',cap,1)
        assert cell_width(expected)<=len(src),(src,expected)
        compiled.append((a,b))
    unique=[]
    for pair in compiled:
        if pair not in unique: unique.append(pair)
    source_init=',\n'.join(cstr(s) for s in sources)
    rule_src_init=','.join(cstr(a) for a,b in unique)
    rule_dst_init=','.join(cstr(b) for a,b in unique)
    expected_targets=[]
    for src,(a,b) in zip(sources,compiled):
        match=re.fullmatch(re.escape(a).replace(re.escape('{*}'),r'([0-9]+)').replace(re.escape('{n}'),r'([0-9]+)'),src)
        assert match
        dst=b
        for cap in match.groups(): dst=dst.replace('{*}',cap,1).replace('{n}',cap,1)
        expected_targets.append(dst)
    expected_init=','.join(cstr(x) for x in expected_targets)
    transition_path=ROOT/'analysis/map-label-transition-fixture.json'
    transition=json.loads(transition_path.read_text(encoding='utf-8'))
    def glyph_row(row): return '{'+','.join(str(x['glyph']) for x in row)+'}'
    def color_row(row): return '{'+','.join('{'+','.join(str(v) for v in bytes.fromhex(x['fg_bg']))+'}' for x in row)+'}'
    fixture_source=transition['source'];assert len(fixture_source)==23
    arr=('static const int fixcg[5][23]={'+','.join(glyph_row(x['child']) for x in transition['states'])+'};\n'
         'static const unsigned char fixcc[5][23][6]={'+','.join(color_row(x['child']) for x in transition['states'])+'};\n'
         'static const int fixrg[5][23]={'+','.join(glyph_row(x['root']) for x in transition['states'])+'};\n'
         'static const unsigned char fixrc[5][23][6]={'+','.join(color_row(x['root']) for x in transition['states'])+'};\n')
    test_c=r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include "cmkr_core.h"
typedef unsigned char BYTE; typedef unsigned short WORD; typedef uint32_t DWORD;
typedef size_t SIZE_T; typedef uintptr_t ULONG_PTR; typedef int BOOL;
static DWORD mocktick; static void *GetCurrentProcess(void){return (void*)1;} static DWORD GetTickCount(void){return ++mocktick;}
static unsigned g_frame; static void L(const char *fmt,...){(void)fmt;}
typedef struct {DWORD f0,glyph,f8;BYTE fgR,fgG,fgB,bgR,bgG,bgB;WORD pad;} XCell;
typedef struct {int w,h;XCell*cells;} Grid; typedef struct {int w,h;void*pixels;} SSurf;
static XCell rootcells[160][45]; static XCell mapraw[50*35],bridgeraw[1],rootraw[160*45];
static Grid g_grid={160,45,&rootcells[0][0]}; static XCell *g_snap=&rootcells[0][0];
#define CELL(x,y) rootcells[(x)][(y)]
static char ChAt(int x,int y){DWORD c=CELL(x,y).glyph;return c>=32&&c<=126?(char)c:' ';}
static int g_on=1,g_cellW=10,g_cellH=20; static void *g_console=(void*)0x11110000;
#define NLAB 15
static unsigned char selfmem[NLAB][0x40],stringmem[NLAB][0x1c],heapstr[NLAB][192];
static unsigned char childcellraw[NLAB][180*20];
static unsigned char mapmem[0x40],bridgemem[0x40],rootmem[0x40];
static const uintptr_t SELFBASE=0x10000000,STRBASE=0x20000000,HEAPBASE=0x30000000,CELLBASE=0x40000000;
static const uintptr_t MAPADDR=0x50000000,BRIDGEADDR=0x50001000,ROOTADDR=0x11110000;
static void wi(void*p,int off,int v){memcpy((char*)p+off,&v,4);}
static int idxaddr(uintptr_t a,uintptr_t base,int *idx,uintptr_t unit){if(a<base)return 0;uintptr_t d=a-base;*idx=(int)(d/unit);return *idx>=0&&*idx<NLAB;}
static BOOL ReadProcessMemory(void*proc,const void*addr,void*out,SIZE_T n,SIZE_T*got){uintptr_t a=(uintptr_t)addr;int i;(void)proc;if(!addr||!out)return 0;
 if(idxaddr(a,SELFBASE,&i,0x1000)){a=(uintptr_t)selfmem[i]+(a-(SELFBASE+(uintptr_t)i*0x1000));if(a+n>(uintptr_t)selfmem[i]+sizeof selfmem[i])return 0;}
 else if(idxaddr(a,STRBASE,&i,0x1000)){a=(uintptr_t)stringmem[i]+(a-(STRBASE+(uintptr_t)i*0x1000));if(a+n>(uintptr_t)stringmem[i]+sizeof stringmem[i])return 0;}
 else if(idxaddr(a,HEAPBASE,&i,0x1000)){a=(uintptr_t)heapstr[i]+(a-(HEAPBASE+(uintptr_t)i*0x1000));if(a+n>(uintptr_t)heapstr[i]+sizeof heapstr[i])return 0;}
 else if(idxaddr(a,CELLBASE,&i,0x1000)){a=(uintptr_t)childcellraw[i]+(a-(CELLBASE+(uintptr_t)i*0x1000));if(a+n>(uintptr_t)childcellraw[i]+sizeof childcellraw[i])return 0;}
 else if(a>=MAPADDR&&a+n<=MAPADDR+sizeof mapmem)a=(uintptr_t)mapmem+(a-MAPADDR);
 else if(a>=BRIDGEADDR&&a+n<=BRIDGEADDR+sizeof bridgemem)a=(uintptr_t)bridgemem+(a-BRIDGEADDR);
 else if(a>=ROOTADDR&&a+n<=ROOTADDR+sizeof rootmem)a=(uintptr_t)rootmem+(a-ROOTADDR);
 else return 0;memcpy(out,(void*)a,n);if(got)*got=n;return 1;}
static Rule rules[128],*g_tr=rules;static int arr[128],ofs[258],g_ntr,*g_trArr=arr,*g_trOfs=ofs;
static int BkFit(const Rule*r,const int*a,int lo,int hi,int n){while(lo<hi){int m=lo+(hi-lo)/2;if(r[a[m]].minLen>n)lo=m+1;else hi=m;}return lo;}
static int draws,clears,finals,noise_draws,bad_row,bad_native;static char output[2048];static int outputn;
static void DrawSpan(SSurf*s,int col,int row,int nc,int vs,int big,const BYTE*bg,const char*text,int r,int g,int b){(void)s;(void)nc;(void)vs;(void)big;(void)bg;(void)r;(void)g;(void)b;++draws;if(row<10||row>=45||col<0||col>=100)bad_row=1;if(!text||!*text){++clears;return;}if((unsigned char)text[0]>=0xE4)++finals;if((unsigned char)text[0]>=0xE3&&(unsigned char)text[0]<=0xE9)++noise_draws;size_t n=strlen(text);if(outputn+(int)n<sizeof output){memcpy(output+outputn,text,n);outputn+=(int)n;output[outputn]=0;}}
#include "zh_child_text_draw.h"
#include "zh_map_label_draw.h"
static const char *sources[NLAB]={SOURCES};
static const int NRULES=RULECOUNT;
FIXTURE_ARRAYS
static const char *rule_src[NRULES]={RULE_SRCS}; static const char *rule_dst[NRULES]={RULE_DSTS};
static const char *expected[NLAB]={EXPECTED};
static ZhChildTextCell *childcells[NLAB];
static void setup_rules(void){int i,b,cnt[257]={0},cur[257];for(i=0;i<NLAB;i++)childcells[i]=(ZhChildTextCell*)childcellraw[i];for(i=0;i<NRULES;i++)if(!cm_rulebuild(&rules[g_ntr++],rule_src[i],rule_dst[i]))abort();
 for(i=0;i<g_ntr;i++){b=(rules[i].ns<=0||rules[i].s[0].cap||rules[i].s[0].len<=0)?256:(unsigned char)rules[i].s[0].lit[0];cnt[b]++;}ofs[0]=0;for(b=0;b<257;b++){ofs[b+1]=ofs[b]+cnt[b];cur[b]=ofs[b];}
 for(i=0;i<g_ntr;i++){b=(rules[i].ns<=0||rules[i].s[0].cap||rules[i].s[0].len<=0)?256:(unsigned char)rules[i].s[0].lit[0];arr[cur[b]++]=i;}
 for(b=0;b<257;b++)for(i=ofs[b]+1;i<ofs[b+1];i++){int v=arr[i],j=i;while(j>ofs[b]&&rules[arr[j-1]].minLen<rules[v].minLen){arr[j]=arr[j-1];--j;}arr[j]=v;}}
static void root_hud(void){memset(rootcells,0,sizeof rootcells);CELL(21,0).glyph='/';CELL(23,0).glyph='L';CELL(25,0).glyph='O';CELL(27,0).glyph='G';}
static void map_header(void){memset(mapmem,0,sizeof mapmem);memset(bridgemem,0,sizeof bridgemem);memset(rootmem,0,sizeof rootmem);
 wi(mapmem,4,(int)BRIDGEADDR);wi(mapmem,8,50);wi(mapmem,12,35);wi(mapmem,16,(int)MAPADDR);wi(mapmem,0x18,1);wi(mapmem,0x1c,0);wi(mapmem,0x20,10);wi(mapmem,0x24,0);wi(mapmem,0x28,10);
 wi(bridgemem,4,(int)ROOTADDR);wi(bridgemem,8,1);wi(bridgemem,12,1);wi(bridgemem,16,(int)BRIDGEADDR);wi(bridgemem,0x18,0);wi(bridgemem,0x1c,0);wi(bridgemem,0x20,0);wi(bridgemem,0x24,0);wi(bridgemem,0x28,0);
 wi(rootmem,8,160);wi(rootmem,12,45);wi(rootmem,16,(int)ROOTADDR);wi(rootmem,0x18,0);}
static void setup_label(int i,int xroot,int yroot,int visible){size_t n=strlen(sources[i]);uintptr_t self=SELFBASE+(uintptr_t)i*0x1000,so=STRBASE+(uintptr_t)i*0x1000,heap=HEAPBASE+(uintptr_t)i*0x1000,cells=CELLBASE+(uintptr_t)i*0x1000;int ax=xroot/2,ay=yroot-10,j;
 memset(selfmem[i],0,sizeof selfmem[i]);memset(stringmem[i],0,sizeof stringmem[i]);memset(heapstr[i],0,sizeof heapstr[i]);memset(childcellraw[i],0,sizeof childcellraw[i]);memcpy(heapstr[i],sources[i],n);
 wi(stringmem[i],0,(int)heap);wi(stringmem[i],0x10,(int)n);wi(stringmem[i],0x14,128);
 wi(selfmem[i],4,(int)MAPADDR);wi(selfmem[i],8,(int)n+4);wi(selfmem[i],12,1);wi(selfmem[i],16,(int)cells);wi(selfmem[i],0x18,0);wi(selfmem[i],0x1c,ax);wi(selfmem[i],0x20,ay);wi(selfmem[i],0x24,ax*2);wi(selfmem[i],0x28,10+ay);
 for(j=0;j<(int)n;j++){ZhChildTextCell*c=&childcells[i][j];c->glyph=(unsigned char)sources[i][j];c->fgR=0;c->fgG=visible?178:0;c->fgB=0;c->bgR=c->bgG=c->bgB=0;CELL(xroot+j,yroot).glyph=(unsigned char)sources[i][j];CELL(xroot+j,yroot).fgR=c->fgR;CELL(xroot+j,yroot).fgG=c->fgG;CELL(xroot+j,yroot).fgB=c->fgB;CELL(xroot+j,yroot).bgR=c->bgR;CELL(xroot+j,yroot).bgG=c->bgG;CELL(xroot+j,yroot).bgB=c->bgB;}
}
static void apply_fixture(int k){int j;for(j=0;j<23;j++){ZhChildTextCell*c=&childcells[2][j];XCell*r=&CELL(20+j,20);c->glyph=(DWORD)fixcg[k][j];c->fgR=fixcc[k][j][0];c->fgG=fixcc[k][j][1];c->fgB=fixcc[k][j][2];c->bgR=fixcc[k][j][3];c->bgG=fixcc[k][j][4];c->bgB=fixcc[k][j][5];r->glyph=(DWORD)fixrg[k][j];r->fgR=fixrc[k][j][0];r->fgG=fixrc[k][j][1];r->fgB=fixrc[k][j][2];r->bgR=fixrc[k][j][3];r->bgG=fixrc[k][j][4];r->bgB=fixrc[k][j][5];}}
static void syncroot(int i,int xr,int yr){int j;for(j=0;j<(int)strlen(sources[i]);j++){ZhChildTextCell*c=&childcells[i][j];CELL(xr+j,yr).glyph=c->glyph;CELL(xr+j,yr).fgR=c->fgR;CELL(xr+j,yr).fgG=c->fgG;CELL(xr+j,yr).fgB=c->fgB;CELL(xr+j,yr).bgR=c->bgR;CELL(xr+j,yr).bgG=c->bgG;CELL(xr+j,yr).bgB=c->bgB;}}
static void setup_long(int n){int j;uintptr_t self=SELFBASE,str=STRBASE,heap=HEAPBASE,cells=CELLBASE;memset(selfmem[0],0,sizeof selfmem[0]);memset(stringmem[0],0,sizeof stringmem[0]);memset(heapstr[0],'A',(size_t)n);memset(childcellraw[0],0,sizeof childcellraw[0]);heapstr[0][n]=0;wi(stringmem[0],0,(int)heap);wi(stringmem[0],0x10,n);wi(stringmem[0],0x14,192);wi(selfmem[0],4,(int)MAPADDR);wi(selfmem[0],8,n+1);wi(selfmem[0],12,1);wi(selfmem[0],16,(int)cells);wi(selfmem[0],0x1c,0);wi(selfmem[0],0x20,10);wi(selfmem[0],0x24,0);wi(selfmem[0],0x28,20);for(j=0;j<n;j++)childcells[0][j].glyph='A';}
static void resetdraw(void){draws=clears=finals=noise_draws=bad_row=bad_native=outputn=0;output[0]=0;}
static int run_tests(SSurf*s){int i;
 setup_rules();map_header();root_hud();
 /* All 15 live-observed source rows resolve to the actual loaded production
    rule and expected generated target; repeated instances remain distinct. */
 for(i=0;i<NLAB;i++){setup_label(i,(i%3)*32,12+(i/3)*6,1);if(!ZhRememberMapLabelText((void*)(SELFBASE+(uintptr_t)i*0x1000),0,0,(void*)(STRBASE+(uintptr_t)i*0x1000)))return 10+i;}
 for(i=0;i<NLAB;i++){int found=0,j;for(j=0;j<ZH_MAP_LABEL_MAX;j++)if(zh_map_label_entries[j].active&&zh_map_label_entries[j].self==(void*)(SELFBASE+(uintptr_t)i*0x1000)){found=1;break;}if(!found)return 40+i;for(int q=0;q<ZH_MAP_LABEL_MAX;q++)if(zh_map_label_entries[q].active&&zh_map_label_entries[q].self==(void*)(SELFBASE+(uintptr_t)i*0x1000)&&strcmp(zh_map_label_entries[q].target,expected[i]))return 55+i;}
 /* Same-row projected source rectangles overlap by design in this fixture:
    renderer refuses ambiguous sibling clears instead of painting across. */
 /* Simulate overlapping projected siblings after successful source ownership;
    only conflicting entries are suppressed. */
 zh_map_label_entries[1]=zh_map_label_entries[0];
 resetdraw();DrawZhMapLabels(s);if(!zh_map_label_entries[0].conflict||!zh_map_label_entries[1].conflict)return 60;
 zh_map_label_entries[1].active=0;resetdraw();DrawZhMapLabels(s);if(!draws)return 601;
 /* Stale child text projected over a different, currently owned root label
    is excluded from conflict arbitration and cannot block that owner. */
 memset(zh_map_label_entries,0,sizeof zh_map_label_entries);setup_label(0,20,20,1);if(!ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 602;
 setup_label(1,20,20,1);if(!ZhRememberMapLabelText((void*)(SELFBASE+0x1000),0,0,(void*)(STRBASE+0x1000)))return 603;
 resetdraw();DrawZhMapLabels(s);if(zh_map_label_entries[0].root_owned||!zh_map_label_entries[1].root_owned||zh_map_label_entries[1].conflict||!draws)return 604;
 /* Exercise the committed live sequence: child before root composition, root
    identity with color lag, aligned native fade, then explicit completion. */
 memset(zh_map_label_entries,0,sizeof zh_map_label_entries);setup_label(2,20,20,1);
 if(!ZhRememberMapLabelText((void*)(SELFBASE+0x2000),0,0,(void*)(STRBASE+0x2000)))return 61;
 memset(stringmem[2],0,sizeof stringmem[2]);memset(heapstr[2],0,sizeof heapstr[2]);
 {ZhMapLabelEntry*e=NULL;for(i=0;i<ZH_MAP_LABEL_MAX;i++)if(zh_map_label_entries[i].active){e=&zh_map_label_entries[i];break;}if(!e||!e->cap_count||e->complete)return 62;
  apply_fixture(0);resetdraw();DrawZhMapLabels(s);if(draws||!e->active)return 621; /* root not composed yet */
  apply_fixture(1);resetdraw();DrawZhMapLabels(s);if(noise_draws||finals||!e->active)return 622; /* root/child both dark */
  apply_fixture(2);resetdraw();DrawZhMapLabels(s);if(draws||!e->active)return 623; /* one-sample color lag */
  apply_fixture(3);{unsigned char before[sizeof childcellraw[0]];memcpy(before,childcells[0],sizeof before);resetdraw();DrawZhMapLabels(s);if(!noise_draws||finals||!e->active)return 624;if(memcmp(before,childcells[0],sizeof before))return 625;}
  /* Hide numeric capture [1] while retaining visible body; it must not leak. */
  for(i=0;i<3;i++){ZhChildTextCell*c=&childcells[2][i];XCell*r=&CELL(20+i,20);c->bgR=c->bgG=c->bgB=0;r->bgR=r->bgG=r->bgB=0;}
  resetdraw();DrawZhMapLabels(s);if(strstr(output,"1"))return 626;
  apply_fixture(4);if(!ZhSetMapLabelComplete((void*)(SELFBASE+0x2000),sources[2],(unsigned)strlen(sources[2]),1))return 627;
  resetdraw();DrawZhMapLabels(s);if(!finals||!strstr(output,"轻型突击步枪")||!strstr(output,"[1]"))return 628;
  /* Native child can outlive a disappeared/occluded root label. Do not draw or
     retire on absence; restoring exact owner permits a later frame. */
  apply_fixture(0);resetdraw();DrawZhMapLabels(s);if(draws||!e->active)return 629;
  apply_fixture(4);resetdraw();DrawZhMapLabels(s);if(!finals||!e->active)return 630;
  /* Child source mutation is an identity failure and retires the cached row. */
  childcells[2][4].glyph='X';resetdraw();DrawZhMapLabels(s);if(draws||e->active)return 631;
 }
 /* Hidden labels are concealed without early translated text. */
 memset(zh_map_label_entries,0,sizeof zh_map_label_entries);setup_label(2,20,20,0);if(!ZhRememberMapLabelText((void*)(SELFBASE+0x2000),0,0,(void*)(STRBASE+0x2000)))return 70;resetdraw();DrawZhMapLabels(s);if(finals||noise_draws||clears==0)return 71;
 /* Failure gates: HUD off, wrong parent scale, wrong hierarchy, cycles,
    bad projection, bad bounds, and source reuse/mismatch all refuse safely. */
 memset(zh_map_label_entries,0,sizeof zh_map_label_entries);setup_label(0,20,20,1);g_on=0;if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 80;g_on=1;
 setup_label(0,20,20,1);wi(mapmem,0x18,0);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 81;wi(mapmem,0x18,1);
 setup_label(0,20,20,1);wi(mapmem,4,(int)MAPADDR);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 82;wi(mapmem,4,(int)BRIDGEADDR);
 setup_label(0,20,20,1);wi(selfmem[0],0x24,21);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 83;
 setup_label(0,20,20,1);wi(selfmem[0],0x1c,49);wi(selfmem[0],0x24,98);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 84;
 setup_label(0,20,20,1);wi(selfmem[0],4,(int)ROOTADDR);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 85;
 setup_label(0,20,20,1);ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE);childcells[0][0].glyph='X';resetdraw();DrawZhMapLabels(s);if(draws||zh_map_label_entries[0].active)return 86;
 setup_label(0,20,20,1);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE)){}else return 87;ZhForgetMapLabels((void*)SELFBASE);resetdraw();DrawZhMapLabels(s);if(draws)return 88;
 /* A cycle in the parent chain is rejected, and complete temporary source
    destruction after registration is harmless because bytes were copied. */
 setup_label(0,20,20,1);wi(bridgemem,4,(int)MAPADDR);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 89;wi(bridgemem,4,(int)ROOTADDR);
 memset(zh_map_label_entries,0,sizeof zh_map_label_entries);setup_label(0,20,20,1);if(!ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 90;memset(stringmem[0],0,sizeof stringmem[0]);memset(heapstr[0],0,sizeof heapstr[0]);
 for(i=0;i<(int)strlen(sources[0]);++i)if(sources[0][i]!=' '){childcells[0][i].fgG=(i==1?0:100);CELL(20+i,20).fgG=childcells[0][i].fgG;}resetdraw();DrawZhMapLabels(s);if(!noise_draws)return 91;
 for(i=0;i<(int)strlen(sources[0]);++i)if(sources[0][i]!=' '){childcells[0][i].fgG=178;CELL(20+i,20).fgG=178;}if(!ZhSetMapLabelComplete((void*)SELFBASE,sources[0],(unsigned)strlen(sources[0]),1))return 920;resetdraw();DrawZhMapLabels(s);if(!finals)return 92;
 memset(zh_map_label_entries,0,sizeof zh_map_label_entries);setup_long(97);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 93;setup_long(160);if(ZhRememberMapLabelText((void*)SELFBASE,0,0,(void*)STRBASE))return 94;
 return 0;}
int main(void){SSurf s={1600,900,NULL};int r;r=run_tests(&s);printf("%d\n",r);return r?1:0;}
'''
    test_c=(test_c.replace('FIXTURE_ARRAYS',arr).replace('SOURCES',source_init).replace('RULE_SRCS',rule_src_init).replace('RULE_DSTS',rule_dst_init).replace('EXPECTED',expected_init).replace('RULECOUNT',str(len(unique))))
    # Generated array initializer macros need braces, not comma-only expansion.
    with tempfile.TemporaryDirectory() as td:
        src=Path(td)/'test.c';exe=Path(td)/'test';src.write_text(test_c,encoding='utf-8')
        include=['-I',str(ROOT/'upstream/KR-AOS-Cogmind/Cogmind-KR_PDS'),'-I',str(ROOT/'runtime')]
        test_result=0
        for suffix,flags in [('normal',[]),('asan-ubsan',['-fsanitize=address,undefined','-fno-omit-frame-pointer'])]:
            exe2=Path(td)/('test-'+suffix)
            cr=subprocess.run(['cc','-std=c99','-O0',*flags,*include,str(src),'-o',str(exe2)],capture_output=True,text=True)
            if cr.returncode: raise SystemExit(cr.stderr)
            result=subprocess.run([str(exe2)],capture_output=True,text=True)
            if result.returncode: raise SystemExit(f'{suffix} host test failed: stdout={result.stdout!r} stderr={result.stderr!r}')
            test_result=int(result.stdout.strip())
    report={
      'status':'passed',
      'test_result':test_result,
      'helper_sha256':hashlib.sha256(HELPER.read_bytes()).hexdigest(),
        'source_fixture_sha256':hashlib.sha256(SOURCE_FIXTURE.read_bytes()).hexdigest(),
      'transition_fixture_sha256':hashlib.sha256((ROOT/'analysis/map-label-transition-fixture.json').read_bytes()).hexdigest(),
      'sanitizers':['AddressSanitizer','UndefinedBehaviorSanitizer'],
      'production_rule_validation':'Python exact full-TSV match pass plus C core against selected matching rules; the C harness does not load every production rule simultaneously.',
      'rules_sha256':hashlib.sha256(RULES.read_bytes()).hexdigest(),
      'production_dll_sha256':None,
      'runtime_status':'not integrated or built',
      'scope':['15 live-probed map-label source records uniquely matched against full production TSV; C matcher exercised with the selected corresponding rules','nested scale-1 map chain and cell projection','capture gating for numeric [1] rank','captured missing-root and one-sample color-lag transitions skip without retiring; default-incomplete phonetic draw and explicit-completion Chinese draw','temporary source-buffer lifetime','source immutability and per-cell background clear','invalid scale/projection/root/bounds/cycle rejection','overlapping sibling refusal','HUD-off, child glyph reuse and destructor invalidation'],
      'limits':['Host mock validates the candidate helper only; it is not included in a production DLL.','No native completion producer is connected; entries remain in phonetic reveal mode unless an external caller explicitly sets completion. No palette/uniformity heuristic promotes them.','The committed transition fixture is sequential RPM data rather than an atomic frame capture; one-frame root/child lag is modeled explicitly.','Overlapping currently-owned projected siblings are deliberately refused.','Only the exact 4-level observed map ancestry and 50x35 scale-1 map parent are accepted.']
    }
    REPORT.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(f'PASS: 15 map-label source records; helper {report["helper_sha256"]}')

if __name__=='__main__': main()
