/* Render known manual providers from the live cell reveal state.
   The native grid remains untouched; only the proxy span is painted over. */
#ifndef ZH_MANUAL_DRAW_H
#define ZH_MANUAL_DRAW_H
#include <string.h>
#include "zh_manual.h"
#include "zh_decode_state.h"

#define ZH_DECODE_FAMILY_MANUAL 0x4d414e55u

static const char *zh_manual_noise[] = {
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

static int ZhManualCellVisible(const XCell *cell)
{
    return abs((int)cell->fgR-(int)cell->bgR) +
           abs((int)cell->fgG-(int)cell->bgG) +
           abs((int)cell->fgB-(int)cell->bgB) > 3;
}

static int ZhManualTokenChar(unsigned char c)
{
    return (c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9');
}

static void DrawZhManualReveal(SSurf *surface, int col, int row, int span,
                               const char *text, unsigned seed_base)
{
    int total=cm_cells(text),offset=0,source_cells=span;
    const char *p=text;
    unsigned visible_count=0;
    unsigned now=(unsigned)GetTickCount();
    int i;
    for(i=0;i<span;++i) if(ZhManualCellVisible(&CELL(col+i,row))) ++visible_count;
    if(total<=0 || total>source_cells) return;

    /* Conceal only the exact provider string, never the navigation key or
       native heading underline that may share the row. */
    DrawSpan(surface,col,row,span,1,0,NULL,"",0,0,0);
    if(!visible_count) return;
    while(*p) {
        unsigned cp;
        const char *next=cm_u8next(p,&cp);
        int width=cm_wide(cp)?2:1;
        int first=offset*source_cells/total;
        int last=((offset+width)*source_cells+total-1)/total-1;
        int complete=1,any=0,sample=first,j;
        char glyph[8];
        XCell *cell;
        if(first<0 || last>=source_cells || next-p>7) break;
        for(j=first;j<=last;++j) {
            XCell *native=&CELL(col+j,row);
            if(ZhManualCellVisible(native)) { any=1;sample=j; }
            else complete=0;
        }
        /* A short frontier prevents completed Chinese glyphs from outrunning
           the native caret. Near the end, reveal against the remaining cells. */
        if(complete && visible_count<(unsigned)source_cells) {
            int required=(width==2?1:0),seen=0;
            for(j=last+1;j<source_cells && seen<required;++j) {
                if(!ZhManualCellVisible(&CELL(col+j,row))) complete=0;
                ++seen;
            }
            if(seen<required) complete=0;
        }
        if(any) {
            cell=&CELL(col+sample,row);
            memcpy(glyph,p,(size_t)(next-p));glyph[next-p]=0;
            if(complete && ZhDecodeGlyphReady(ZH_DECODE_FAMILY_MANUAL,col,row,span,1,
                    text,now,(unsigned)g_frame,(unsigned)offset,(unsigned)total))
                DrawSpan(surface,col+offset,row,width,1,0,NULL,glyph,
                         cell->fgR,cell->fgG,cell->fgB);
            else if(width==2) {
                unsigned seed=(unsigned)(seed_base*7u+(unsigned)offset+
                    cell->glyph+cell->fgR*3u+cell->fgG+cell->fgB*5u);
                DrawSpan(surface,col+offset,row,width,1,0,NULL,
                    zh_manual_noise[seed%(sizeof zh_manual_noise/sizeof zh_manual_noise[0])],
                    cell->fgR,cell->fgG,cell->fgB);
            }
        }
        offset+=width;p=next;
    }
}

static void DrawZhManualRows(SSurf *surface)
{
    int x,y;
    char source[256];
    /* Require the complete generated token before taking over. If a runtime
       build hides or omits unrevealed cells from ChAt(), an incomplete token
       is deliberately left alone instead of guessing from a ZHm prefix.
       Exact tokens are safe at any column, including inside [index] labels. */
    for(y=0;y<g_grid.h;++y) for(x=0;x<g_grid.w;++x) {
        int end=x,length,j;
        const char *text;
        if(ChAt(x,y)!='Z' || x+8>=g_grid.w) continue;
        while(end<g_grid.w && end-x<(int)sizeof source-1 &&
              ZhManualTokenChar((unsigned char)ChAt(end,y))) ++end;
        length=end-x;
        if(length<9) continue;
        for(j=0;j<length;++j) source[j]=ChAt(x+j,y);
        source[length]=0;
        text=ZhManualDecode(source,(size_t)length);
        if(!text || cm_cells(text)>length) continue;
        DrawZhManualReveal(surface,x,y,length,text,
                           (unsigned char)source[8]);
        /* One exact run per row is enough; a decoded token cannot overlap
           another valid token without being part of the first token itself. */
        x=end-1;
    }
}
#endif
