/* Intro experiment: runtime labels advance every two root-grid columns.
   Progress is sampled from native visible provider cells, not a timer. */
#include "zh_animation_data.h"
typedef struct { int active, found, col, row, cells; } ZhAnimationState;
static ZhAnimationState zh_animation_states[ZH_ANIMATION_COUNT];
/* Native intro uses two root columns per ASCII slot. Status blocks follow
   the translated label: two root cells between visible texts, and one
   root cell of padding on each side of the status. */
static int ZhStatusColumn(size_t index)
{
    int after=zh_animation_texts[index].status_after;
    if(after<0) return -1;
    return 3+cm_cells(zh_animation_texts[after].text);
}
static const char *zh_animation_noise[] = {
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

static int ZhIntroGameHud(void)
{
    int x;
    for (x = 0; x + 8 < g_grid.w; ++x)
        if (ChAt(x,0)=='/' && ChAt(x+2,0)=='L' && ChAt(x+4,0)=='O' && ChAt(x+6,0)=='G') return 1;
    return 0;
}

static int ZhAnimationVisible(const XCell *c)
{
    return abs((int)c->fgR-c->bgR)+abs((int)c->fgG-c->bgG)+abs((int)c->fgB-c->bgB)>3;
}

static void DrawZhAnimations(SSurf *surface)
{
    size_t i;
    int x,y;
    if (ZhIntroGameHud()) {
        for (i=0;i<ZH_ANIMATION_COUNT;++i) zh_animation_states[i].active=0;
        return;
    }
    /* Discover actual flattened screen positions rather than treating local
       constructor coordinates as screen coordinates. */
    for (y=0;y<g_grid.h;++y) for(x=0;x<g_grid.w;++x) {
        char ch=ChAt(x,y);
        if (ch<'a' || ch>'z' || x+6>=g_grid.w) continue;
        /* Intro background also contains random Latin characters. A four-cell
           provider run distinguishes owned text from that unrelated noise. */
        if(ChAt(x+2,y)!=ch || ChAt(x+4,y)!=ch || ChAt(x+6,y)!=ch) continue;
        for (i=0;i<ZH_ANIMATION_COUNT;++i) {
            ZhAnimationState *state=&zh_animation_states[i];
            if (!state->active || state->found || ch!=zh_animation_texts[i].marker ||
                !ZhAnimationVisible(&CELL(x,y))) continue;
            state->found=1;state->col=x;state->row=y;
            L("[zh-animation-position] index=%u col=%d row=%d cells=%d",(unsigned)i,x,y,state->cells);
        }
    }
    for (i=0;i<ZH_ANIMATION_COUNT;++i) {
        ZhAnimationState *state=&zh_animation_states[i];
        const ZhAnimationText *entry=&zh_animation_texts[i];
        const char *p=entry->text;
        int owned=0,offset=0,target_cells=cm_cells(p),start;
        int status=entry->status_after>=0;
        BYTE status_bg[3]={0,0,0}, black[3]={0,0,0};
        const BYTE *paint_bg=NULL;
        if (!state->active || !state->found || state->row<0 || state->row>=g_grid.h ||
            state->col<0 || state->col+state->cells*2>g_grid.w) continue;
        for (x=0;x<state->cells;++x) {
            XCell *c=&CELL(state->col+x*2,state->row);
            if (ChAt(state->col+x*2,state->row)==entry->marker && ZhAnimationVisible(c)) {
                ++owned;
                status_bg[0]=c->bgR;status_bg[1]=c->bgG;status_bg[2]=c->bgB;
            }
        }
        if (!owned) continue;
        DrawSpan(surface,state->col,state->row,state->cells*2,1,0,status?black:NULL,"",0,0,0);
        if(status) {
            int block_col=ZhStatusColumn(i),block_width=target_cells+2;
            if(block_col<0 || block_col+block_width>g_grid.w) continue;
            DrawSpan(surface,block_col,state->row,block_width,1,0,status_bg,"",0,0,0);
            start=block_col+1;
            paint_bg=status_bg;
        } else start=state->col+(entry->center?(state->cells*2-target_cells)/2:0);
        while (*p) {
            unsigned cp;
            const char *next=cm_u8next(p,&cp);
            int width=cm_wide(cp)?2:1;
            int first=offset*state->cells/target_cells;
            int last=((offset+width)*state->cells+target_cells-1)/target_cells-1;
            int complete=1,any=0,sample=first,j;
            char glyph[8];
            XCell *c;
            if(first<0 || last>=state->cells || next-p>7) break;
            for(j=first;j<=last;++j) {
                XCell *native=&CELL(state->col+j*2,state->row);
                int visible=ZhAnimationVisible(native);
                if(visible) { any=1;sample=j; }
                if(!visible || ChAt(state->col+j*2,state->row)!=entry->marker) complete=0;
            }
            /* Keep a short decoding frontier behind the native cursor. A glyph
               settles only after two subsequent source cells have settled;
               reaching the complete native line settles the final frontier. */
            if (complete && owned<state->cells) {
                int edge=last+2;
                if(edge>=state->cells) complete=0;
                else for(j=last+1;j<=edge;++j) {
                    XCell *following=&CELL(state->col+j*2,state->row);
                    if(ChAt(state->col+j*2,state->row)!=entry->marker ||
                       !ZhAnimationVisible(following)) complete=0;
                }
            }
            if (!any) { offset+=width;p=next;continue; }
            c=&CELL(state->col+sample*2,state->row);
            memcpy(glyph,p,(size_t)(next-p));glyph[next-p]=0;
            if (complete)
                DrawSpan(surface,start+offset,state->row,width,1,0,paint_bg,glyph,c->fgR,c->fgG,c->fgB);
            else if(width==2) {
                unsigned seed=(unsigned)(i*7+offset+c->glyph+c->fgR*3+c->fgG+c->fgB*5);
                const char *noise=zh_animation_noise[seed % (sizeof zh_animation_noise/sizeof zh_animation_noise[0])];
                DrawSpan(surface,start+offset,state->row,width,1,0,paint_bg,noise,c->fgR,c->fgG,c->fgB);
            }
            offset+=width;p=next;
        }
    }
}
