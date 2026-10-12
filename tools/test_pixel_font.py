#!/usr/bin/env python3
"""Test embedded atlas coverage, lookup, sharp pixels, and raster bounds."""
import json, shutil, subprocess
from build_pixel_font import build
from validate_translations import ROOT, rows, validate

def main():
    validate();folder=ROOT/'build/pixel-tests';glyphs=build(folder)
    required=set(range(32,127))
    for t in ('trans','para'):
        for dst in rows(ROOT/f'build/tables/zh_{t}.tsv').values(): required.update(map(ord,dst))
    missing=sorted(required-set(glyphs))
    if missing: raise ValueError(f'Missing glyphs: {missing}')
    code=r'''
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zh_pixel.h"
int main(void) {
    int sizes[][2]={{5,10},{6,12},{8,16},{10,20},{12,24},{16,32},{24,48}};
    size_t i,j; unsigned char a[2*24*48+2];
    for(i=0;i<ZH_PIXEL_COUNT;i++) {
        const ZhPixel *p=&ZH_PIXEL[i];
        if(ZhPixelFind(p->cp)!=p || (i && ZH_PIXEL[i-1].cp>=p->cp)) return 1;
        for(j=0;j<sizeof sizes/sizeof sizes[0];j++) {
            int bw=sizes[j][0]*(p->advance==12?2:1),bh=sizes[j][1],k;
            memset(a,0,sizeof a); a[0]=17;a[bw*bh+1]=19;
            ZhPixelRaster(p,bw,bh,a+1);
            if(a[0]!=17 || a[bw*bh+1]!=19) return 2;
            for(k=1;k<=bw*bh;k++) if(a[k]!=0 && a[k]!=255) return 3;
        }
    }
    if(ZhPixelFind(0x10000)!=NULL || !ZhPixelFind(0x6838)) return 4;
    puts("PASS: sorted BMP lookup, 7 raster sizes, bounds, binary pixel alpha");return 0;
}
'''
    (folder/'test.c').write_text(code)
    cc=shutil.which('clang') or shutil.which('cc')
    subprocess.run([cc,'-O1','-fsanitize=address,undefined',str(folder/'test.c'),'-o',str(folder/'test')],check=True)
    r=subprocess.run([str(folder/'test')],capture_output=True,text=True,check=True)
    report={'status':'passed','atlas_glyphs':len(glyphs),'required_glyphs':len(required),
            'missing_glyphs':missing,'raster_test':r.stdout.strip()}
    (ROOT/'analysis/pixel-font-tests.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
    print(r.stdout.strip())
if __name__=='__main__': main()
