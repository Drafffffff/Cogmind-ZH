/* Translate the observed advanced-help key legend over its native fade.
   This renderer reads the live root grid only; it never edits native cells. */
#ifndef ZH_ADVANCED_HELP_DRAW_H
#define ZH_ADVANCED_HELP_DRAW_H

#include <string.h>
#include "zh_decode_state.h"

#define ZH_DECODE_FAMILY_ADVANCED_BODY 0x41445642u

static const char *zh_advanced_help_noise[]={
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

typedef struct {
    const char *source;
    const char *target;
} ZhAdvancedHelpTerm;

static const ZhAdvancedHelpTerm zh_advanced_help_terms[]={
    {"Movement Keys","移动键"},
    {"Move Keys","移动键"},
    {"Arrows","方向键"},
    {"Accent","重音键"},
    {"Spacebar","空格键"},
    {"Wheel","滚轮"},
    {"Drag to Map","拖至地图"}
};

static int ZhAdvancedCellVisible(const XCell *c)
{
    return c && abs((int)c->fgR-(int)c->bgR)+abs((int)c->fgG-(int)c->bgG)+
           abs((int)c->fgB-(int)c->bgB)>3;
}

static char ZhAdvancedChar(int x,int y)
{
    DWORD g=CELL(x,y).glyph;
    return g>=32 && g<=126 ? (char)g : '\0';
}

static int ZhAdvancedEqualFold(char a,char b)
{
    if(a>='a'&&a<='z')a=(char)(a-('a'-'A'));
    if(b>='a'&&b<='z')b=(char)(b-('a'-'A'));
    return a==b;
}

static int ZhAdvancedWordChar(char c)
{
    return (c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_';
}

static int ZhAdvancedBlank(char c){return c==' '||c=='\0';}

static int ZhAdvancedContains(int row,const char *needle)
{
    int x,n=(int)strlen(needle);
    if(row<0||row>=g_grid.h||n<=0||n>g_grid.w)return 0;
    for(x=0;x+n<=g_grid.w;++x){int j;for(j=0;j<n;++j)if(ZhAdvancedChar(x+j,row)!=needle[j])break;if(j==n)return 1;}
    return 0;
}

static int ZhAdvancedSpacedMarker(const char *marker)
{
    int x,n=(int)strlen(marker),j;
    for(x=0;x+2*(n-1)<g_grid.w;++x){
        for(j=0;j<n;++j)if(ZhAdvancedChar(x+2*j,0)!=marker[j])break;
        if(j==n)return 1;
    }
    return 0;
}

/* The shared log/combat key legend is shown on Basic, Advanced, and Manual.
   Require the captured navigation and panel chrome so this cannot activate
   on an ordinary game screen that happens to contain a word such as Wheel. */
static int ZhAdvancedHelpChromeContext(SSurf *surface)
{
    if(!surface || !g_on || g_grid.w!=160 || g_grid.h!=45 ||
       g_cellW<=0 || g_cellH!=2*g_cellW ||
       surface->w<g_grid.w*g_cellW || surface->h<g_grid.h*g_cellH ||
       !ZhAdvancedSpacedMarker("/LOG/") || !ZhAdvancedSpacedMarker("/COMBAT/") ||
       !ZhAdvancedContains(9,"/ P A R T S /") ||
       !ZhAdvancedContains(10,"[1] BASIC \\ [2] ADVANCED \\ [3] MANUAL \\ [4] OPTIONS \\ [5] NEWS \\ [6] CREDITS \\ [7] RECORDS"))return 0;
    return 1;
}

static int ZhAdvancedContext(SSurf *surface)
{
    int page1,page2;
    if(!ZhAdvancedHelpChromeContext(surface))return 0;
    page1=ZhAdvancedContains(41,"Page 1/2")&&ZhAdvancedContains(43,"KP = Keypad/Numpad");
    page2=ZhAdvancedContains(41,"Page 2/2")&&
          ZhAdvancedContains(43,"Remember to look in other windows for their relevant commands");
    return page1||page2;
}

/* Advanced and Manual can inherit the full Log/Combat key table. Basic's
   shorter click/scroll hints are already covered by exact translation rules;
   require the observed full-table labels before drawing any key-zone overlay. */
static int ZhAdvancedFullTopSource(void)
{
    return ZhAdvancedContains(2,"Control")&&
           ZhAdvancedContains(3,"KP-/+")&&
           ZhAdvancedContains(4,"PgUp/Dn")&&
           ZhAdvancedContains(5,"End   End")&&
           ZhAdvancedContains(7,"Spacebar");
}

static int ZhAdvancedTopKeyZone(int x,int y,int width)
{
    if(y>=3&&y<=7&&x>=2&&x+width<=30)return 1;
    if(y>=3&&y<=4&&x>=52&&x+width<=80)return 1;
    return 0;
}

static int ZhAdvancedMainZone(int x,int y,int width)
{
    if(y>=13&&y<=39&&x>=2&&x+width<=50)return 1;
    if(y>=12&&y<=42&&x>=102&&x+width<=131)return 1;
    return 0;
}

/* Main help instructions are Advanced-page-specific. */
static int ZhAdvancedInZone(int x,int y,int width)
{
    return ZhAdvancedMainZone(x,y,width);
}

static int ZhAdvancedFindAt(int x,int y,int top_zone,const ZhAdvancedHelpTerm **term_out)
{
    size_t i;
    for(i=0;i<sizeof zh_advanced_help_terms/sizeof zh_advanced_help_terms[0];++i){
        const ZhAdvancedHelpTerm *term=&zh_advanced_help_terms[i];
        int n=(int)strlen(term->source),j;
        char before,after;
        if(!(top_zone?ZhAdvancedTopKeyZone(x,y,n):ZhAdvancedInZone(x,y,n))||x+n>g_grid.w)continue;
        for(j=0;j<n;++j)if(!ZhAdvancedEqualFold(ZhAdvancedChar(x+j,y),term->source[j]))break;
        if(j!=n)continue;
        before=x>0?ZhAdvancedChar(x-1,y):' ';
        after=x+n<g_grid.w?ZhAdvancedChar(x+n,y):' ';
        if(ZhAdvancedWordChar(before)||ZhAdvancedWordChar(after))continue;
        *term_out=term;return n;
    }
    return 0;
}

static int ZhAdvancedMatchEnd(int *span)
{
    int x=32,y=5;
    if(y!=5||x+3>=g_grid.w||ZhAdvancedChar(x,y)!='E'||
       ZhAdvancedChar(x+1,y)!='n'||ZhAdvancedChar(x+2,y)!='d'||
       ZhAdvancedWordChar(ZhAdvancedChar(x-1,y))||
       ZhAdvancedWordChar(ZhAdvancedChar(x+3,y))||!ZhAdvancedBlank(ZhAdvancedChar(x+3,y)))return 0;
    *span=4;return 1;
}

static void ZhAdvancedDrawSpan(SSurf *surface,int x,int y,int span,const char *target,unsigned salt,int decode_family)
{
    int source_ink[160],ink=0,target_cells=cm_cells(target),visible=0,uniform=1;
    int i,have_color=0,offset=0;BYTE fr=0,fg=0,fb=0,br=0,bg=0,bb=0;
    const char *p=target;
    if(span<=0||span>160||x<0||x+span>g_grid.w||y<0||y>=g_grid.h||
       target_cells<=0||target_cells>span)return;
    for(i=0;i<span;++i){
        const XCell *c=&CELL(x+i,y);
        char ch=ZhAdvancedChar(x+i,y);
        if(!ZhAdvancedBlank(ch)){
            source_ink[ink++]=i;
            if(ZhAdvancedCellVisible(c))++visible;
            else uniform=0;
            if(!have_color){fr=c->fgR;fg=c->fgG;fb=c->fgB;br=c->bgR;bg=c->bgG;bb=c->bgB;have_color=1;}
            else if(c->fgR!=fr||c->fgG!=fg||c->fgB!=fb||c->bgR!=br||c->bgG!=bg||c->bgB!=bb)uniform=0;
        }
    }
    if(!ink)return;
    if(!visible)uniform=0;
    /* Conceal native source glyphs without changing their background colors. */
    for(i=0;i<span;++i){const XCell *c=&CELL(x+i,y);BYTE cell_bg[3]={c->bgR,c->bgG,c->bgB};
        DrawSpan(surface,x+i,y,1,1,0,cell_bg,"",0,0,0);}
    while(*p){
        unsigned cp;const char *next=cm_u8next(p,&cp);int width=cm_wide(cp)?2:1;
        int sample=(offset+width/2)*ink/target_cells,source_i,cur;const XCell *c;char out[8];size_t bytes=(size_t)(next-p);
        if(sample>=ink)sample=ink-1;source_i=source_ink[sample];c=&CELL(x+source_i,y);
        cur=ZhAdvancedCellVisible(c);
        if(offset+width<=target_cells&&bytes<sizeof out){
            memcpy(out,p,bytes);out[bytes]=0;
            if(decode_family && cur && ZhDecodeGlyphReady((unsigned)decode_family,x,y,span,1,
                    target,(unsigned)GetTickCount(),(unsigned)g_frame,(unsigned)offset,(unsigned)target_cells)){
                DrawSpan(surface,x+offset,y,width,1,0,NULL,out,c->fgR,c->fgG,c->fgB);
            }else if(!decode_family && uniform)DrawSpan(surface,x+offset,y,width,1,0,NULL,out,c->fgR,c->fgG,c->fgB);
            else if(cur&&width==2){
                unsigned seed=(unsigned)(x*7+y*13+offset+salt+c->fgR+c->fgG*3+c->fgB*5);
                const char *noise=zh_advanced_help_noise[seed%(sizeof zh_advanced_help_noise/sizeof zh_advanced_help_noise[0])];
                DrawSpan(surface,x+offset,y,width,1,0,NULL,noise,c->fgR,c->fgG,c->fgB);
            }else if(cur&&width==1)DrawSpan(surface,x+offset,y,width,1,0,NULL,out,c->fgR,c->fgG,c->fgB);
        }
        offset+=width;p=next;
    }
}

/* Invoke after the ordinary root-grid renderer, so fade colors are sampled
   from the original native cells and this layer is the final visible pass. */
static void DrawZhAdvancedHelpContexts(SSurf *surface)
{
    int y,x,span;
    const ZhAdvancedHelpTerm *term;
    int chrome=ZhAdvancedHelpChromeContext(surface);
    int full_top=chrome&&ZhAdvancedFullTopSource();
    int advanced=chrome&&ZhAdvancedContext(surface);
    if(!chrome)return;
    /* Only a source-confirmed full key legend gets this overlay. In particular
       Basic's two whole-line click/scroll hints stay with their exact rules. */
    if(full_top){
        for(y=3;y<=7;++y){
            for(x=2;x<g_grid.w;++x){
                int n=ZhAdvancedFindAt(x,y,1,&term);
                if(!n)continue;
                ZhAdvancedDrawSpan(surface,x,y,n,term->target,(unsigned)(y*173+x),0);
                x+=n-1;
            }
        }
        if(ZhAdvancedMatchEnd(&span))ZhAdvancedDrawSpan(surface,32,5,span,"末尾",0x454e44u,0);
    }
    if(!advanced)return;
    /* Advanced page body zones retain their page-specific guard. */
    for(y=12;y<=42;++y){
        for(x=2;x<g_grid.w;++x){
            int n=ZhAdvancedFindAt(x,y,0,&term);
            if(!n)continue;
            ZhAdvancedDrawSpan(surface,x,y,n,term->target,(unsigned)(y*173+x),ZH_DECODE_FAMILY_ADVANCED_BODY);
            x+=n-1;
        }
    }
}

#endif
