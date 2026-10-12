#!/usr/bin/env python3
"""Run the actual FillGaps implementation with border direction guards."""
import hashlib,json,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def main():
    upstream=(ROOT/'upstream/KR-AOS-Cogmind/Cogmind-KR_PDS/cmkr.c').read_text()
    start=upstream.index('static void FillGaps(void)\n{')
    end=upstream.index('/* ==================================================================',start)
    body=upstream[start:end].replace('IsBox(o->col - 1, row) && IsBox(nx, row)','ZhBorderGapAllowed(o->col - 1, row, nx)')
    code=r'''
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "cmkr_core.h"
typedef unsigned char BYTE;
typedef struct{unsigned glyph;BYTE fgR,fgG,fgB,bgR,bgG,bgB;}XCell;
static struct{int w,h;}g_grid={160,45};static XCell cells[45][160];
#define CELL(x,y) cells[y][x]
static int ChAt(int x,int y){return CELL(x,y).glyph;}
static int IsBox(int x,int y){return x>=0&&x<160&&y>=0&&y<45&&CELL(x,y).glyph>=128&&CELL(x,y).glyph<=159;}
#define MAX_RUN 600
typedef struct{int row,col,cells,vs;char t[MAX_TEXT];}Op;
static Op g_ops[MAX_RUN];static int g_nop;
static void AddOpRaw(int row,int col,int count,const char*t,BYTE r,BYTE g,BYTE b){assert(g_nop<MAX_RUN);Op*o=&g_ops[g_nop++];o->row=row;o->col=col;o->cells=count;o->vs=1;strcpy(o->t,t);}
static void AddOpRawBg(int row,int col,int count,const char*t,BYTE r,BYTE g,BYTE b,const BYTE*bg){AddOpRaw(row,col,count,t,r,g,b);}
#include "zh_border_gap.h"
'''+body+r'''
static void setup(unsigned left,unsigned right){memset(cells,0,sizeof cells);memset(g_ops,0,sizeof g_ops);g_nop=1;g_ops[0].row=4;g_ops[0].col=1;g_ops[0].cells=48;g_ops[0].vs=1;strcpy(g_ops[0].t,"中文正文");CELL(0,4).glyph=left;CELL(49,4).glyph=right;}
int main(void){
 setup(128,128);FillGaps();assert(g_nop==1); /* actual LOG body */
 setup(129,129);FillGaps();assert(g_nop==2); /* horizontal title */
 setup(136,137);FillGaps();assert(g_nop==2);
 setup(129,128);FillGaps();assert(g_nop==1);
 setup(128,137);FillGaps();assert(g_nop==1);
 setup(140,140);FillGaps();assert(g_nop==1);
 assert(!ZhBorderGapAllowed(-1,4,49));assert(!ZhBorderGapAllowed(0,-1,49));assert(!ZhBorderGapAllowed(0,4,160));
 puts("PASS: production FillGaps preserves horizontal titles and rejects body vertical borders, mixed orientation, unverified glyphs, and bounds");return 0;}
'''
    with tempfile.TemporaryDirectory() as td:
        p=Path(td);(p/'test.c').write_text(code)
        subprocess.run(['clang','-O1','-g','-fsanitize=address,undefined','-I',str(ROOT/'runtime'),'-I',str(ROOT/'upstream/KR-AOS-Cogmind/Cogmind-KR_PDS'),str(p/'test.c'),'-o',str(p/'test')],check=True)
        r=subprocess.run([str(p/'test')],capture_output=True,text=True,check=True)
    report={'status':'passed','helper_sha256':hashlib.sha256((ROOT/'runtime/zh_border_gap.h').read_bytes()).hexdigest(),'result':r.stdout.strip(),'scope':'Actual upstream FillGaps body with the production guard substitution; native pixel rendering tested separately.'}
    (ROOT/'analysis/border-gap-tests.json').write_text(json.dumps(report,indent=2)+'\n');print(r.stdout.strip())
if __name__=='__main__':main()
