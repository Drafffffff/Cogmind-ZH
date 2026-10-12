/* Narrow, source-confirmed overlay for the four Records overview headings.
   Read the live root grid; never alter native cells or retain stale strings. */
#ifndef ZH_RECORDS_DRAW_H
#define ZH_RECORDS_DRAW_H

#include <string.h>
#include "zh_decode_state.h"

typedef struct {
    int x,y;
    const char *source;
    const char *target;
    unsigned family;
} ZhRecordsTitle;

static const ZhRecordsTitle zh_records_titles[]={
    {3,14,"[Gallery Collection]","[画廊收藏]",0x52454301u},
    {6,19,"[Lore Collection]","[背景故事收藏]",0x52454302u},
    {9,23,"[Achievements]","[成就]",0x52454303u},
    {11,27,"[Dump Stats]","[导出统计]",0x52454304u}
};

static char ZhRecordsChar(int x,int y)
{
    DWORD glyph=CELL(x,y).glyph;
    return glyph>=32&&glyph<=126?(char)glyph:'\0';
}

static int ZhRecordsVisible(const XCell *c)
{
    return c && abs((int)c->fgR-(int)c->bgR)+abs((int)c->fgG-(int)c->bgG)+
           abs((int)c->fgB-(int)c->bgB)>3;
}

static int ZhRecordsAt(int x,int y,const char *text)
{
    int i,n=(int)strlen(text);
    if(x<0||y<0||y>=g_grid.h||x+n>g_grid.w)return 0;
    for(i=0;i<n;++i)if(ZhRecordsChar(x+i,y)!=text[i])return 0;
    return 1;
}

static int ZhRecordsSpacedMarker(const char *marker)
{
    int x,n=(int)strlen(marker),i;
    for(x=0;x+2*(n-1)<g_grid.w;++x){
        for(i=0;i<n;++i)if(ZhRecordsChar(x+2*i,0)!=marker[i])break;
        if(i==n)return 1;
    }
    return 0;
}

/* Require the selected Records tab and all four exact body headings. The
   four-line signature prevents activation on shared help/navigation chrome. */
static int ZhRecordsContext(SSurf *surface)
{
    size_t i;
    if(!surface||!g_on||g_grid.w!=160||g_grid.h!=45||g_cellW<=0||
       g_cellH!=2*g_cellW||surface->w<g_grid.w*g_cellW||
       surface->h<g_grid.h*g_cellH||
       !ZhRecordsSpacedMarker("/LOG/")||!ZhRecordsSpacedMarker("/COMBAT/")||
       !ZhRecordsAt(84,10,"[7] RECORDS"))return 0;
    for(i=0;i<sizeof zh_records_titles/sizeof zh_records_titles[0];++i)
        if(!ZhRecordsAt(zh_records_titles[i].x,zh_records_titles[i].y,
                        zh_records_titles[i].source))return 0;
    return 1;
}

static const char *zh_records_noise[]={
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

static void ZhRecordsDrawTitle(SSurf *surface,const ZhRecordsTitle *entry)
{
    int source_ink[64],ink=0,visible=0,total,source_len;
    int i,offset=0;const char *p;
    if(!entry||!surface)return;
    p=entry->target;source_len=(int)strlen(entry->source);total=cm_cells(entry->target);
    if(total<=0||source_len>64||total>source_len)return;
    for(i=0;entry->source[i];++i){
        const XCell *cell=&CELL(entry->x+i,entry->y);
        if(entry->source[i]!=' '){source_ink[ink++]=i;if(ZhRecordsVisible(cell))++visible;}
    }
    if(!ink||!visible)return;

    /* Preserve each native background/highlight cell while hiding its text. */
    for(i=0;entry->source[i];++i){
        const XCell *cell=&CELL(entry->x+i,entry->y);
        BYTE bg[3]={cell->bgR,cell->bgG,cell->bgB};
        DrawSpan(surface,entry->x+i,entry->y,1,1,0,bg,"",cell->fgR,cell->fgG,cell->fgB);
    }
    while(*p){
        unsigned cp;const char *next=cm_u8next(p,&cp);
        int width=cm_wide(cp)?2:1,sample=(offset+width/2)*ink/total,source_i;
        const XCell *cell;char glyph[8];size_t bytes=(size_t)(next-p);
        if(sample>=ink)sample=ink-1;
        source_i=source_ink[sample];cell=&CELL(entry->x+source_i,entry->y);
        if(ZhRecordsVisible(cell)&&offset+width<=total&&bytes<sizeof glyph){
            memcpy(glyph,p,bytes);glyph[bytes]=0;
            if(cm_wide(cp)&&!ZhDecodeGlyphReady(entry->family,entry->x,entry->y,
                    (int)strlen(entry->source),1,entry->target,GetTickCount(),g_frame,
                    (unsigned)offset,(unsigned)total)){
                unsigned seed=(unsigned)(entry->x*7+entry->y*13+offset+
                    cell->fgR+cell->fgG*3+cell->fgB*5);
                DrawSpan(surface,entry->x+offset,entry->y,width,1,0,NULL,
                    zh_records_noise[seed%(sizeof zh_records_noise/sizeof zh_records_noise[0])],
                    cell->fgR,cell->fgG,cell->fgB);
            }else{
                /* The span was already cleared cell by cell above. Leave its
                   per-cell native backgrounds untouched while blitting ink. */
                DrawSpan(surface,entry->x+offset,entry->y,width,1,0,NULL,glyph,
                         cell->fgR,cell->fgG,cell->fgB);
            }
        }
        offset+=width;p=next;
    }
}

static void DrawZhRecordsTitles(SSurf *surface)
{
    size_t i;
    if(!ZhRecordsContext(surface))return;
    for(i=0;i<sizeof zh_records_titles/sizeof zh_records_titles[0];++i)
        ZhRecordsDrawTitle(surface,&zh_records_titles[i]);
}

#endif
