#include "zh_literal_match.h"
/* Bounded reveal overlay for already-matched one-line menu Ops.
   Planner ownership/matching is authoritative; this layer only samples the
   original root cells and stages the translated glyphs. */
#ifndef ZH_MENU_OP_DRAW_H
#define ZH_MENU_OP_DRAW_H

#include <stdlib.h>
#include <string.h>
#include "zh_decode_state.h"

#define ZH_MENU_OP_MAX_SOURCE 256
#define ZH_MENU_OP_FAMILY 0x4d454e55u

static const char *zh_menu_op_noise[]={
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

static char ZhMenuOpChar(int x,int y)
{
    DWORD g=CELL(x,y).glyph;
    return g>=32&&g<=126?(char)g:'\0';
}

static int ZhMenuOpVisible(const XCell *c)
{
    return c&&abs((int)c->fgR-(int)c->bgR)+abs((int)c->fgG-(int)c->bgG)+
           abs((int)c->fgB-(int)c->bgB)>3;
}

static int ZhMenuOpEqualFold(char a,char b)
{
    if(a>='a'&&a<='z')a=(char)(a-('a'-'A'));
    if(b>='a'&&b<='z')b=(char)(b-('a'-'A'));
    return a==b;
}

static int ZhMenuOpSpacedMarker(const char *marker)
{
    int x,n=(int)strlen(marker),i;
    for(x=0;x+2*(n-1)<g_grid.w;++x){
        for(i=0;i<n;++i)if(ZhMenuOpChar(x+2*i,0)!=marker[i])break;
        if(i==n)return 1;
    }
    return 0;
}

static int ZhMenuOpContains(int row,const char *text)
{
    int x,n=(int)strlen(text),i;
    if(row<0||row>=g_grid.h||n<=0||n>g_grid.w)return 0;
    for(x=0;x+n<=g_grid.w;++x){
        for(i=0;i<n;++i)if(!ZhMenuOpEqualFold(ZhMenuOpChar(x+i,row),text[i]))break;
        if(i==n)return 1;
    }
    return 0;
}

static int ZhMenuOpNav(void)
{
    static const char nav[]="[1] BASIC \\ [2] ADVANCED \\ [3] MANUAL \\ [4] OPTIONS \\ [5] NEWS \\ [6] CREDITS \\ [7] RECORDS";
    int x,i,n=(int)strlen(nav);
    for(x=0;x+n<=g_grid.w;++x){
        for(i=0;i<n;++i)if(ZhMenuOpChar(x+i,10)!=nav[i])break;
        if(i==n)return 1;
    }
    return 0;
}

static int ZhMenuOpBasicActive(void)
{
    int i,bright=0;
    /* In the captured Basic page its tab letters use the active #00D900
       color. The textual hints are a second guard if the tab is mid-fade. */
    if(ZhMenuOpContains(2,"Click to expand/shrink")||
       ZhMenuOpContains(3,"Scroll wheel to scroll"))return 1;
    if(!ZhMenuOpNav())return 0;
    for(i=0;i<5;++i){const XCell*c=&CELL(9+i,10);if(c->fgG>=180)++bright;}
    return bright>=3;
}

static int ZhMenuOpContext(SSurf *surface)
{
    return surface&&g_on&&g_grid.w==160&&g_grid.h==45&&g_cellW>0&&
        g_cellH==2*g_cellW&&surface->w>=g_grid.w*g_cellW&&
        surface->h>=g_grid.h*g_cellH&&ZhMenuOpSpacedMarker("/LOG/")&&
        ZhMenuOpSpacedMarker("/COMBAT/")&&ZhMenuOpContains(9,"/ P A R T S /")&&
        ZhMenuOpNav()&&!ZhMenuOpBasicActive();
}

static int ZhMenuOpHasCjk(const char *text)
{
    const char*p=text;
    while(*p){unsigned cp;const char*next=cm_u8next(p,&cp);
        if((cp>=0x2e80&&cp<=0xa4cf)||(cp>=0xac00&&cp<=0xd7a3)||
           (cp>=0xf900&&cp<=0xfaff)||(cp>=0xff00&&cp<=0xff60))return 1;
        p=next;
    }
    return 0;
}

static int ZhMenuOpProviderSpan(const char *source)
{
    return strstr(source,"ZHm")!=NULL||strstr(source,"ZHM")!=NULL;
}

static int ZhOptionsContext(SSurf *surface);

/* Return 1 when the caller should skip its ordinary DrawSpan. Matched spans
   are concealed even while fully dark; visible native cells alone drive ink. */
static int DrawZhMenuOpReveal(SSurf *surface,const Op *op)
{
    char source[ZH_MENU_OP_MAX_SOURCE];
    int ink_at[ZH_MENU_OP_MAX_SOURCE],ink=0,visible=0,i,total,offset=0;
    const char*p;
    if((!ZhMenuOpContext(surface)&&!ZhOptionsContext(surface))||!op||op->vs!=1||op->big||op->row<0||
       op->row>=g_grid.h||op->col<0||op->cells<=0||
       op->col+op->cells>g_grid.w||op->cells>=ZH_MENU_OP_MAX_SOURCE||
       !op->t[0]||!ZhMenuOpHasCjk(op->t))return 0;
    if(memchr(op->t,CM_BR,strlen(op->t))||memchr(op->t,CM_SHL,strlen(op->t)))return 0;
    total=cm_cells(op->t);
    if(total<=0||total>op->cells)return 0;
    for(i=0;i<op->cells;++i){
        char c=ZhMenuOpChar(op->col+i,op->row);
        const XCell*cell=&CELL(op->col+i,op->row);
        if(c<' '||c>'~')return 0;
        source[i]=c;
        if(c!=' '){ink_at[ink++]=i;if(ZhMenuOpVisible(cell))++visible;}
    }
    source[op->cells]=0;
    if(ink<2||ZhMenuOpProviderSpan(source))return 0;

    /* Preserve native or Op-owned backgrounds while removing English glyphs. */
    for(i=0;i<op->cells;++i){
        const XCell*c=&CELL(op->col+i,op->row);BYTE cellbg[3];
        if(op->bgSet){cellbg[0]=op->bgR;cellbg[1]=op->bgG;cellbg[2]=op->bgB;}
        else {cellbg[0]=c->bgR;cellbg[1]=c->bgG;cellbg[2]=c->bgB;}
        DrawSpan(surface,op->col+i,op->row,1,1,0,cellbg,"",c->fgR,c->fgG,c->fgB);
    }
    if(!visible)return 1;
    p=op->t;
    while(*p){
        unsigned cp;const char*next=cm_u8next(p,&cp);int width=cm_wide(cp)?2:1;
        int first=(offset*ink)/total,last=((offset+width)*ink+total-1)/total-1;
        int sample,any=0,j;const XCell*cell;char glyph[8];
        size_t bytes=(size_t)(next-p);
        if(last>=ink)last=ink-1;
        if(first<0||first>=ink||last<first||offset+width>total||bytes>=sizeof glyph)return 1;
        sample=first;
        for(j=first;j<=last;++j){int at=ink_at[j];
            if(ZhMenuOpVisible(&CELL(op->col+at,op->row))){any=1;sample=j;}
        }
        if(any){
            int src=ink_at[sample];cell=&CELL(op->col+src,op->row);
            memcpy(glyph,p,bytes);glyph[bytes]=0;
            if(cm_wide(cp)&&!ZhDecodeGlyphReady(ZH_MENU_OP_FAMILY,
                    op->col,op->row,op->cells,1,op->t,GetTickCount(),g_frame,
                    (unsigned)offset,(unsigned)total)){
                unsigned seed=(unsigned)(op->col*7+op->row*13+offset+
                    cell->fgR+cell->fgG*3+cell->fgB*5);
                const char*noise=zh_menu_op_noise[seed%(sizeof zh_menu_op_noise/sizeof zh_menu_op_noise[0])];
                BYTE opbg[3]={op->bgR,op->bgG,op->bgB};
                DrawSpan(surface,op->col+offset,op->row,width,1,0,
                    op->bgSet?opbg:NULL,
                    noise,cell->fgR,cell->fgG,cell->fgB);
            }else{
                BYTE opbg[3]={op->bgR,op->bgG,op->bgB};
                const BYTE*bg=op->bgSet?opbg:NULL;
                DrawSpan(surface,op->col+offset,op->row,width,1,0,bg,glyph,
                         cell->fgR,cell->fgG,cell->fgB);
            }
        }
        offset+=width;p=next;
    }
    return 1;
}

/* Resolve only a single literal source token to a single literal CJK target.
   The production order within its normal first-byte/minLen bucket is kept. */
static int ZhOptionsExactTarget(const char *source,int length,char *target,int cap)
{
    return ZhLiteralTarget(source,length,target,cap,0)&&
           ZhMenuOpHasCjk(target)&&cm_cells(target)<=length;
}

static int ZhOptionsFindText(int row,int xlo,int xhi,const char *text,int *found_x)
{
    int x,n=(int)strlen(text),count=0;
    if(row<0||row>=g_grid.h||n<=0)return 0;
    if(xlo<0)xlo=0;if(xhi>g_grid.w-n)xhi=g_grid.w-n;
    for(x=xlo;x<=xhi;++x){int i;for(i=0;i<n;++i)if(ZhMenuOpChar(x+i,row)!=text[i])break;
        if(i==n){if(found_x)*found_x=x;++count;}}
    return count;
}

static int ZhOptionsActiveTab(void)
{
    int x,nav[3]={-1,-1,-1},i;
    if(!ZhOptionsFindText(10,0,120,"[1] BASIC",&nav[0])||
       !ZhOptionsFindText(10,0,120,"[2] ADVANCED",&nav[1])||
       !ZhOptionsFindText(10,0,120,"[4] OPTIONS",&x)||
       !ZhOptionsFindText(10,0,120,"[7] RECORDS",&nav[2]))return 0;
    {
        int options_x=x+4,options=0,other=0,n=0;
        for(i=0;i<7;++i)options+=CELL(options_x+i,10).fgG;
        for(i=0;i<5;++i)other+=CELL(nav[0]+4+i,10).fgG;
        n=5;
        for(i=0;i<8;++i)other+=CELL(nav[1]+4+i,10).fgG;
        n+=8;
        for(i=0;i<7;++i)other+=CELL(nav[2]+4+i,10).fgG;
        n+=7;
        /* Pixel25 evidence: selected OPTIONS text rises above inactive tabs
           by >=16 even in the first captured visible frame (75ms). */
        return options/7 >= other/n+8;
    }
}

static int ZhOptionsContext(SSurf *surface)
{
    /* Use the shared chrome and Options selected-tab evidence directly.
       ZhMenuOpContext also applies its Basic-page green heuristic, which is
       ambiguous on the Options animation fixture because inactive BASIC
       glyphs retain a brighter cached color. */
    return surface&&g_on&&g_grid.w==160&&g_grid.h==45&&g_cellW>0&&
        g_cellH==2*g_cellW&&surface->w>=g_grid.w*g_cellW&&
        surface->h>=g_grid.h*g_cellH&&ZhMenuOpSpacedMarker("/LOG/")&&
        ZhMenuOpSpacedMarker("/COMBAT/")&&
        ZhMenuOpNav()&&ZhOptionsActiveTab()&&
        !ZhMenuOpContains(2,"Click to expand/shrink")&&
        !ZhMenuOpContains(3,"Scroll wheel to scroll")&&
        ZhMenuOpContains(12,"General")&&ZhMenuOpContains(12,"Player")&&
        ZhMenuOpContains(20,"Visualization")&&ZhMenuOpContains(21,"Audio")&&
        ZhMenuOpContains(29,"Interface")&&ZhMenuOpContains(36,"Alarms")&&
        ZhMenuOpContains(40,"Behavior")&&
        ZhMenuOpContains(13,"a - [UI Layout]")&&
        ZhMenuOpContains(13,"A - [Name]")&&
        ZhMenuOpContains(19,"g - [Log Output]")&&
        ZhMenuOpContains(37,"u - [Keyboard Mode]")&&
        ZhMenuOpContains(37,"U - [Heat]");
}

static int ZhOptionsDrawLabel(SSurf *surface,int x,int y,int len,
                              const char *source)
{
    Op op;char target[MAX_TEXT];
    if(len<=0||len>160||x<0||x+len>g_grid.w||y<0||y>=g_grid.h||
       !ZhOptionsExactTarget(source,len,target,sizeof target))return 0;
    memset(&op,0,sizeof op);op.col=x;op.row=y;op.cells=len;op.vs=1;op.big=0;
    cm_cpy(op.t,target,MAX_TEXT);
    return DrawZhMenuOpReveal(surface,&op);
}

/* Options page source was observed fully in g_grid from 75ms onward while
   its per-cell foreground colors still faded. Only the exact item text
   inside `letter - [label]` and the seven captured section headings are
   overlaid; values, key names, and unrecognized text are left alone. */
static int DrawZhOptionsLabels(SSurf *surface)
{
    static const struct {int row;const char *source;} headings[]={
        {12,"General"},{12,"Player"},{20,"Visualization"},{21,"Audio"},
        {29,"Interface"},{36,"Alarms"},{40,"Behavior"}
    };
    int handled=0,i,y,x;
    if(!ZhOptionsContext(surface))return 0;
    for(i=0;i<(int)(sizeof headings/sizeof headings[0]);++i){
        int at=-1;
        if(ZhOptionsFindText(headings[i].row,0,105,headings[i].source,&at)==1){
            int n=(int)strlen(headings[i].source);
            if(ZhOptionsDrawLabel(surface,at,headings[i].row,n,headings[i].source))++handled;
        }
    }
    for(y=13;y<=43;++y)for(x=8;x<=100;++x){
        char lead=ZhMenuOpChar(x,y),key[80];int open,close,start,end,n,j;
        if(!((lead>='a'&&lead<='z')||(lead>='A'&&lead<='Z'))||
           ZhMenuOpChar(x+1,y)!=' '||ZhMenuOpChar(x+2,y)!='-'||
           ZhMenuOpChar(x+3,y)!=' '||ZhMenuOpChar(x+4,y)!='[')continue;
        open=x+4;start=open+1;close=start;
        while(close<g_grid.w&&close-start<70){char c=ZhMenuOpChar(close,y);
            if(c==']')break;
            if(c<' '||c>'~'||c=='[')break;
            ++close;
        }
        if(close>=g_grid.w||close-start>=70||ZhMenuOpChar(close,y)!=']')continue;
        while(start<close&&ZhMenuOpChar(start,y)==' ')++start;
        end=close;while(end>start&&ZhMenuOpChar(end-1,y)==' ')--end;
        n=end-start;if(n<=0||n>=(int)sizeof key)continue;
        for(j=0;j<n;++j)key[j]=ZhMenuOpChar(start+j,y);
        key[n]=0;
        if(ZhOptionsDrawLabel(surface,start,y,n,key))++handled;
        x=close;
    }
    return handled;
}

#endif
