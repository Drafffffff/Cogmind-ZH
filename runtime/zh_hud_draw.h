/* Full source cells exist before visibility; sample original reveal and colors.
   Never replace native log strings, so history/export and scrolling stay intact. */
#include "zh_hud_data.h"
static int ZhHudEqual(char a,char b)
{
    if(a>='a' && a<='z')a-=32;
    if(b>='a' && b<='z')b-=32;
    return a==b;
}
static void DrawZhHudMessages(SSurf *surface)
{
    int y;
    size_t i;
    if(!g_on || !ZhIntroGameHud()) return;
    for(y=1;y<g_grid.h;++y) for(i=0;i<ZH_HUD_COUNT;++i) {
        const ZhHudText *entry=&zh_hud_texts[i];
        int count=(int)strlen(entry->source),total=cm_cells(entry->text);
        int col,start=entry->column<0?0:entry->column;
        int stop=entry->column<0?g_grid.w-count:entry->column;
        for(col=start;col<=stop;++col) {
        int j,owned=0,ink=0,offset=0,width=count>total?count:total;
        const char *p=entry->text;
        if(width+col>g_grid.w || !ZhHudEqual(ChAt(col,y),entry->source[0]))continue;
        for(j=0;j<count;++j) if(!ZhHudEqual(ChAt(col+j,y),entry->source[j]))break;
        if(j!=count || (col+count<g_grid.w && ChAt(col+count,y)!=' '))continue;
        for(j=0;j<count;++j) if(entry->source[j]!=' ') {
            ++ink;if(ZhAnimationVisible(&CELL(col+j,y)))++owned;
        }
        if(!owned)continue;
        DrawSpan(surface,col,y,width,1,0,NULL,"",0,0,0);
        while(*p) {
            unsigned cp;
            const char *next=cm_u8next(p,&cp);
            int advance=cm_wide(cp)?2:1;
            int first=offset*count/total,last=((offset+advance)*count+total-1)/total-1;
            int any=0,complete=1,sample=first;
            XCell *c;char glyph[8];
            if(last>=count || next-p>7)break;
            for(j=first;j<=last;++j) if(entry->source[j]!=' ') {
                int visible=ZhAnimationVisible(&CELL(col+j,y));
                if(visible){any=1;sample=j;}else complete=0;
            }
            if(complete && owned<ink) {
                int settled=0;
                for(j=last+1;j<count && settled<2;++j) if(entry->source[j]!=' ') {
                    if(!ZhAnimationVisible(&CELL(col+j,y)))complete=0;
                    ++settled;
                }
                if(settled<2)complete=0;
            }
            /* Spaces occupy their final cells too. A source interval made only
               of whitespace samples preceding settled ink instead. */
            if(cp==32 && complete && !any) {
                for(j=first-1;j>=0;--j) if(entry->source[j]!=' ' && ZhAnimationVisible(&CELL(col+j,y))) {
                    any=1;sample=j;break;
                }
            }
            if(any) {
                c=&CELL(col+sample,y);
                memcpy(glyph,p,(size_t)(next-p));glyph[next-p]=0;
                if(complete)DrawSpan(surface,col+offset,y,advance,1,0,NULL,glyph,c->fgR,c->fgG,c->fgB);
                else if(advance==2) {
                    unsigned seed=(unsigned)(i*7+offset+c->fgR+c->fgG*3+c->fgB*5);
                    DrawSpan(surface,col+offset,y,advance,1,0,NULL,
                        zh_animation_noise[seed % (sizeof zh_animation_noise/sizeof zh_animation_noise[0])],c->fgR,c->fgG,c->fgB);
                }
            }
            offset+=advance;p=next;
        }
        }
    }
}
