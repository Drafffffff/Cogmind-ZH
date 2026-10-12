/* Progressive rendering for exact short Ops whose native source cells are
   already present in the grid but are still being revealed left-to-right.
   This is deliberately called only from the existing Op draw loop: work is
   bounded by that Op's source span, never by the whole grid or rule table. */
#include "zh_decode_state.h"
#include "zh_literal_match.h"

#define ZH_SHORT_RAW_FAMILY 0x53485254u
static int ZhShortVisible(const XCell *c)
{
    return abs((int)c->fgR-(int)c->bgR) +
           abs((int)c->fgG-(int)c->bgG) +
           abs((int)c->fgB-(int)c->bgB) > 3;
}

static int ZhShortHasHudHelper(void)
{
    static unsigned cached_frame=(unsigned)-1;
    static int cached_result;
    int x;
    if (cached_frame==(unsigned)g_frame) return cached_result;
    cached_frame=(unsigned)g_frame;
    cached_result=0;
    for (x=0; x+8<g_grid.w; ++x)
        if (ChAt(x,0)=='/' && ChAt(x+2,0)=='L' &&
            ChAt(x+4,0)=='O' && ChAt(x+6,0)=='G') {cached_result=1;break;}
    return cached_result;
}

static int ZhShortHasCjk(const char *s)
{
    while (*s) {
        unsigned cp;
        const char *next=cm_u8next(s,&cp);
        if (cm_wide(cp)) return 1;
        s=next;
    }
    return 0;
}

static int ZhShortKnownHudSource(const char *source,int len)
{
    size_t i;
    for (i=0;i<ZH_HUD_COUNT;++i) {
        const char *known=zh_hud_texts[i].source;
        int j,n=(int)strlen(known);
        if (n!=len) continue;
        for (j=0;j<len;++j) {
            char a=source[j],b=known[j];
            if (a>='a' && a<='z') a-=32;
            if (b>='a' && b<='z') b-=32;
            if (a!=b) break;
        }
        if (j==len) return 1;
    }
    return 0;
}

static int ZhShortAsciiCaseEqual(const char *a,const char *b,int len)
{
    int i;
    for (i=0;i<len;++i) {
        unsigned char x=(unsigned char)a[i],y=(unsigned char)b[i];
        if (x>='a' && x<='z') x=(unsigned char)(x-('a'-'A'));
        if (y>='a' && y<='z') y=(unsigned char)(y-('a'-'A'));
        if (x!=y) return 0;
    }
    return 1;
}

static const char *zh_short_reveal_noise[] = {
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

static int ZhShortAllVisible(const Op *op)
{
    int i,ink=0;
    for (i=0;i<op->cells;++i) {
        char ch=ChAt(op->col+i,op->row);
        if (ch==' ') continue;
        ++ink;
        if (!ZhShortVisible(&CELL(op->col+i,op->row))) return 0;
    }
    return ink>=2;
}

static void ZhShortDrawWholeMapped(SSurf *surface,const Op *op)
{
    BYTE inkR[160],inkG[160],inkB[160];
    int i,ink=0,offset=0,targetCells=cm_cells(op->t);
    const char *p=op->t;
    for (i=0;i<op->cells;++i) if (ChAt(op->col+i,op->row)!=' ') {
        const XCell *c=&CELL(op->col+i,op->row);
        inkR[ink]=c->fgR;inkG[ink]=c->fgG;inkB[ink]=c->fgB;++ink;
    }
    if (!ink || targetCells<=0) return;
    DrawSpan(surface,op->col,op->row,op->cells,1,0,NULL,"",0,0,0);
    while (*p) {
        unsigned cp;const char *next=cm_u8next(p,&cp);
        int width=cm_wide(cp)?2:1;
        int sample=(offset+width/2)*ink/targetCells;
        char glyph[8];size_t n=(size_t)(next-p);
        if (sample>=ink) sample=ink-1;
        if (n>=sizeof glyph) break;
        memcpy(glyph,p,n);glyph[n]=0;
        DrawSpan(surface,op->col+offset,op->row,width,1,0,NULL,glyph,
                 inkR[sample],inkG[sample],inkB[sample]);
        offset+=width;p=next;
    }
}

/* Raw-row translations share one bounded phase regardless of case or cell
   visibility pattern. The matcher has already proved that the entire source
   fragment owns this rectangle; colors are sampled from that same root span. */
static int ZhShortDrawBoundedMapped(SSurf *surface,int x,int y,int source_cells,
                                    const char *target)
{
    BYTE inkR[160],inkG[160],inkB[160],inkBg[160][3],inkVisible[160];
    int i,ink=0,any_visible=0,target_cells=cm_cells(target),offset=0;
    const char *p;
    if(!surface||!target||source_cells<=0||source_cells>160||
       x<0||y<0||y>=g_grid.h||x+source_cells>g_grid.w||
       target_cells<=0||target_cells>source_cells)return 0;
    for(i=0;i<source_cells;++i){
        const XCell *c=&CELL(x+i,y);char ch=ChAt(x+i,y);
        int delta=abs((int)c->fgR-(int)c->bgR)+
                  abs((int)c->fgG-(int)c->bgG)+
                  abs((int)c->fgB-(int)c->bgB);
        if(ch<' '||ch>'~')return 0;
        if(ch==' ')continue;
        if(delta>0)any_visible=1;
        inkR[ink]=c->fgR;inkG[ink]=c->fgG;inkB[ink]=c->fgB;
        inkBg[ink][0]=c->bgR;inkBg[ink][1]=c->bgG;inkBg[ink][2]=c->bgB;
        inkVisible[ink]=(BYTE)ZhShortVisible(c);
        ++ink;
    }
    if(ink<2||!any_visible)return 0;
    /* Erase native letters cell by cell with their original background. */
    ink=0;
    for(i=0;i<source_cells;++i){
        char ch=ChAt(x+i,y);const XCell *c=&CELL(x+i,y);
        BYTE bg[3]={c->bgR,c->bgG,c->bgB};
        if(ch==' ')continue;
        DrawSpan(surface,x+i,y,1,1,0,bg,"",c->fgR,c->fgG,c->fgB);
        ++ink;
    }
    p=target;
    while(*p){
        unsigned cp;const char *next=cm_u8next(p,&cp);
        int width=cm_wide(cp)?2:1;
        int sample=(offset+width/2)*ink/target_cells;
        char glyph[8];size_t n=(size_t)(next-p);
        if(sample>=ink)sample=ink-1;
        if(n>=sizeof glyph)return 1;
        memcpy(glyph,p,n);glyph[n]=0;
        if(ZhDecodeGlyphReady(ZH_SHORT_RAW_FAMILY,x,y,source_cells,1,target,
                              (unsigned)GetTickCount(),(unsigned)g_frame,
                              (unsigned)offset,(unsigned)target_cells)) {
            DrawSpan(surface,x+offset,y,width,1,0,inkBg[sample],glyph,
                     inkR[sample],inkG[sample],inkB[sample]);
        } else if(width==2 && inkVisible[sample]) {
            unsigned seed=(unsigned)(x*7+y*13+offset+inkR[sample]+inkG[sample]*3+inkB[sample]*5);
            const char *noise=zh_short_reveal_noise[
                seed%(sizeof zh_short_reveal_noise/sizeof zh_short_reveal_noise[0])];
            DrawSpan(surface,x+offset,y,width,1,0,inkBg[sample],noise,
                     inkR[sample],inkG[sample],inkB[sample]);
        }
        offset+=width;p=next;
    }
    return 1;
}

static int ZhShortAsciiRowHas(int row,const char *needle)
{
    int x,i,n=(int)strlen(needle);
    if(row<0||row>=g_grid.h||n<=0||n>g_grid.w)return 0;
    for(x=0;x+n<=g_grid.w;++x){
        for(i=0;i<n;++i)if(ChAt(x+i,row)!=needle[i])break;
        if(i==n)return 1;
    }
    return 0;
}

static int ZhShortDataTitleContext(void)
{
    return g_grid.h>6&&
        (ZhShortAsciiRowHas(0,"/ D A T A /")||ZhShortAsciiRowHas(0,"/DATA/"))&&
        ZhShortAsciiRowHas(4,"Overview")&&
        ZhShortAsciiRowHas(5,"Type")&&ZhShortAsciiRowHas(6,"Slot");
}

/* The icon bracket in the item DATA title is a complete boundary. Extract
   the entire printable title to its left and require a full static table rule. */
static int ZhShortDataTitleTarget(const char *title,int len,char *out,int cap)
{
    return ZhLiteralTarget(title,len,out,cap,1)&&ZhShortHasCjk(out)&&cm_cells(out)<=len;
}

static int ZhShortFindDataIconTitle(int *start,int *title_cells,int *skip_end,
                                    char *target,int target_cap)
{
    int x;
    if(g_grid.h<=2)return 0;
    for(x=52;x+4<g_grid.w&&x<96;++x){
        int title_end=x-2,begin,i,n,pos,icons=0;
        char title[128];
        if(CELL(x,2).glyph!='['||CELL(x-1,2).glyph!=' ')continue;
        pos=x+1;
        while(pos<g_grid.w&&icons<4){
            unsigned icon=CELL(pos,2).glyph;
            if(icon==0||!(icon<32||icon>126))break;
            ++icons;++pos;
        }
        if(!icons||pos>=g_grid.w||CELL(pos,2).glyph!=']')continue;
        while(title_end>=52&&CELL(title_end,2).glyph==' ')--title_end;
        begin=title_end;
        while(begin>=52){
            unsigned c=CELL(begin,2).glyph;
            if(c<32||c>126)break;
            if(c==' '&&begin>52&&CELL(begin-1,2).glyph==' ')break;
            --begin;
        }
        ++begin;
        while(begin<=title_end&&CELL(begin,2).glyph==' ')++begin;
        n=title_end-begin+1;
        if(n<=0||n>=(int)sizeof title)continue;
        for(i=0;i<n;++i){unsigned c=CELL(begin+i,2).glyph;if(c<32||c>126)break;title[i]=(char)c;}
        if(i!=n)continue;
        title[n]=0;
        *start=begin;*title_cells=n;*skip_end=pos+1;
        return ZhShortDataTitleTarget(title,n,target,target_cap);
    }
    return 0;
}
/* DATA values are sometimes built as transient partial/glitched ASCII
   (captured En -> Engi -> Engine, PoA -> Powe -> Power). Mask visible letters
   without guessing a translation. Unknown settled text is released after a
   bounded phase, so an untranslated item cannot remain phonetic forever. */
static int ZhShortMaskDataPending(SSurf *surface,int x,int y,int n,const char*source)
{
    int i,letters=0,visible=0;
    if(!ZhShortDataTitleContext()||g_grid.w!=160||g_grid.h!=45||
       (y!=2&&(y<4||y>43))||x<52||x+n>97||n<=0||
       !strcmp(source,"N/A")||!strcmp(source,"ID"))return 0;
    for(i=0;i<n;++i){char ch=source[i];
        if((ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z')){
            ++letters;if(ZhShortVisible(&CELL(x+i,y)))visible=1;}
    }
    if(letters<1||!visible||ZhDecodeGlyphReady(0x44504152u,x,y,n,1,source,
        (unsigned)GetTickCount(),(unsigned)g_frame,0,1))return 0;
    for(i=0;i<n;){int begin=i,len,j;
        while(i<n&&((source[i]>='A'&&source[i]<='Z')||(source[i]>='a'&&source[i]<='z')))++i;
        len=i-begin;if(!len){++i;continue;}
        for(j=0;j<len;++j){const XCell*c=&CELL(x+begin+j,y);BYTE bg[3]={c->bgR,c->bgG,c->bgB};
            DrawSpan(surface,x+begin+j,y,1,1,0,bg,"",0,0,0);}
        for(j=0;j+1<len;j+=2){const XCell*c=&CELL(x+begin+j,y);
            if(ZhShortVisible(c)){BYTE bg[3]={c->bgR,c->bgG,c->bgB};unsigned seed=(unsigned)(x+y*17+begin+j+c->fgG);
                DrawSpan(surface,x+begin+j,y,2,1,0,bg,
                    zh_short_reveal_noise[seed%(sizeof zh_short_reveal_noise/sizeof zh_short_reveal_noise[0])],c->fgR,c->fgG,c->fgB);}
        }
    }return 1;
}

/* Return 1 only while this operation is genuinely mid-reveal.  A return of
   zero delegates the stable/unsupported case to the existing DrawSpan call. */
static int DrawZhShortReveal(SSurf *surface, const Op *op)
{
    int i, ink=0, visiblePrefix=0, hiddenSeen=0;
    int targetCells, eligible, offset=0;
    BYTE inkR[160],inkG[160],inkB[160];
    const char *p;

    if (!surface || !op || !op->zhReveal || op->vs!=1 || op->big || op->bgSet ||
        op->row<0 || op->row>=g_grid.h || op->col<0 || op->cells<=0 || op->cells>160 ||
        op->col+op->cells>g_grid.w || !op->t[0] ||
        !ZhShortHasCjk(op->t) || !ZhShortHasHudHelper()) return 0;
    targetCells=cm_cells(op->t);
    if (targetCells<=0 || targetCells>op->cells) return 0;

    /* ZHm/ZHM are synthetic provider codes used by the manual/layout hooks. */
    if (strstr(op->t,"ZHm") || strstr(op->t,"ZHM")) return 0;
    if (op->cells>=3 && ChAt(op->col,op->row)=='Z' &&
        (ChAt(op->col+1,op->row)=='H' || ChAt(op->col+1,op->row)=='h') &&
        (ChAt(op->col+2,op->row)=='m' || ChAt(op->col+2,op->row)=='M')) return 0;

    /* Validate and measure only the original source span under this Op. */
    for (i=0;i<op->cells;++i) {
        XCell *c=&CELL(op->col+i,op->row);
        char ch=ChAt(op->col+i,op->row);
        if (ch<' ' || ch>'~') return 0;
        if (ch==' ') continue;
        ++ink;
        if (ZhShortVisible(c)) {
            if (hiddenSeen) return 0;       /* visible island after hidden text */
            ++visiblePrefix;
        } else hiddenSeen=1;
        inkR[ink-1]=c->fgR;inkG[ink-1]=c->fgG;inkB[ink-1]=c->fgB;
    }
    if (ink<2) return 0;
    {
        char source[MAX_TEXT];
        if (op->cells>=MAX_TEXT) return 0;
        for (i=0;i<op->cells;++i) source[i]=ChAt(op->col+i,op->row);
        source[op->cells]=0;
        if (ZhShortKnownHudSource(source,op->cells)) return 0;
    }
    if (!hiddenSeen) return 0;              /* stable: use the normal renderer */
    if (!visiblePrefix) return 1;           /* suppress early full Chinese; draw nothing */

    /* Leave two visible source cells beyond the translated frontier so a
       multibyte target glyph is not committed while native reveal is in flight. */
    if (visiblePrefix>=ink) eligible=targetCells;
    else {
        int settled=visiblePrefix>2?visiblePrefix-2:0;
        eligible=(settled*targetCells)/ink;
    }
    if (eligible<0) eligible=0;
    if (eligible>targetCells) eligible=targetCells;

    /* Clear the original English rectangle exactly once. Draw target glyphs
       separately so a dim reveal frontier cannot darken already-settled text. */
    DrawSpan(surface,op->col,op->row,op->cells,1,0,NULL,"",0,0,0);
    p=op->t;
    while (*p) {
        unsigned cp;
        const char *next=cm_u8next(p,&cp);
        int width=cm_wide(cp)?2:1;
        int sample=(offset+width/2)*ink/targetCells;
        BYTE r,g,b;
        char glyph[8];
        size_t n=(size_t)(next-p);
        if (sample>=visiblePrefix) sample=visiblePrefix-1;
        if (sample<0) sample=0;
        r=inkR[sample];g=inkG[sample];b=inkB[sample];
        if (offset+width<=eligible) {
            if (n>=sizeof glyph) break;
            memcpy(glyph,p,n);glyph[n]=0;
            DrawSpan(surface,op->col+offset,op->row,width,1,0,NULL,glyph,r,g,b);
        } else if (offset<eligible && width==2) {
            unsigned seed=(unsigned)(op->row*17+op->col*7+offset+r+g*3+b*5);
            const char *noise=zh_short_reveal_noise[
                seed%(sizeof zh_short_reveal_noise/sizeof zh_short_reveal_noise[0])];
            DrawSpan(surface,op->col+offset,op->row,width,1,0,NULL,noise,r,g,b);
            break;
        } else break;
        offset+=width;p=next;
    }

    /* The source snapshot itself is never modified. */
    return 1;
}

/* Scan only printable raw row fragments on the active HUD.  A fragment is
   delimited by two spaces or a non-printable cell; candidates are looked up
   in the same first-byte/minimum-length bucket as PlanShort, then subjected
   to the real full-consumption matcher. */
static int DrawZhRawShortRows(SSurf *surface)
{
    int y,x,handled=0,data_title_start=-1,data_title_cells=0,data_title_end=0;
    int data_title_rule=0,data_page;
    char data_title_target[MAX_TEXT];
    if (!surface || !g_on || !ZhShortHasHudHelper() || !g_tr ||
        !g_trArr || !g_ntr) return 0;
    data_page=ZhShortDataTitleContext();
    data_title_rule=ZhShortFindDataIconTitle(&data_title_start,&data_title_cells,
                            &data_title_end,data_title_target,sizeof data_title_target);
    if(data_title_rule&&data_page&&
       ZhShortDrawBoundedMapped(surface,data_title_start,2,data_title_cells,
                                data_title_target))++handled;
    if(!data_title_rule||!data_page)data_title_start=-1;
    for (y=0;y<g_grid.h;++y) {
        x=0;
        while (x<g_grid.w) {
#ifdef ZH_LOG_PARAGRAPH_H
            if(x<49&&ZhLogParagraphOwnsRow(y)){x=49;continue;}
#endif
            if(y==2&&data_title_start>=0&&x==data_title_start){x=data_title_end;continue;}
            char source[161];
            int start=x,n=0,tooLong=0,letters=0;
            unsigned glyph;
            while (x<g_grid.w) {
                glyph=CELL(x,y).glyph;
                if (glyph<33 || glyph>126) {++x;break;}
                break;
            }
            if (x>=g_grid.w) break;
            glyph=CELL(x,y).glyph;
            if (glyph<33 || glyph>126) continue;
            start=x;
            while (x<g_grid.w) {
                if(y==2&&data_title_start>=0&&x==data_title_start)break;
                glyph=CELL(x,y).glyph;
                if (glyph<32 || glyph>126) break;
                if (glyph==' ') {
                    int q=x;
                    while (q<g_grid.w && CELL(q,y).glyph==' ') ++q;
                    if (q-x>=2) break;
                    if (n<160) source[n++]=' '; else tooLong=1;
                    x=q;continue;
                }
                if (n<160) source[n++]=(char)glyph; else tooLong=1;
                if ((glyph>='A'&&glyph<='Z') || (glyph>='a'&&glyph<='z')) ++letters;
                ++x;
            }
            while (n>0 && source[n-1]==' ') --n;
            source[n]=0;
            if (x==start) ++x;
            if(tooLong||ZhShortKnownHudSource(source,n))continue;
            if(n<2||letters<2){
                if(ZhShortMaskDataPending(surface,start,y,n,source))++handled;
                continue;
            }
            {
                char target[MAX_TEXT];
                if(ZhLiteralTarget(source,n,target,sizeof target,1)&&
                   ZhShortHasCjk(target)&&cm_cells(target)<=n&&
                   ZhShortDrawBoundedMapped(surface,start,y,n,target))++handled;
                else if(ZhShortMaskDataPending(surface,start,y,n,source))++handled;
            }
            /* Consume the separating two-space run as one boundary. */
            while (x<g_grid.w && CELL(x,y).glyph==' ') ++x;
        }
    }
    return handled;
}
