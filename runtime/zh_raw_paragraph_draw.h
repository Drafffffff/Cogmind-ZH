/* Exact-rule paragraph reveal for structurally validated item DATA pages.
   Match complete root-grid source before painting; never alter native cells. */
#ifndef ZH_RAW_PARAGRAPH_DRAW_H
#define ZH_RAW_PARAGRAPH_DRAW_H
#include <string.h>
#include <stdlib.h>
#include "zh_decode_state.h"
#define ZH_RAW_CRUSH_X 52
#define ZH_RAW_CRUSH_W 45
#define ZH_RAW_CRUSH_H 4
#define ZH_RAW_CRUSH_MAX 256
#define ZH_RAW_EFFECT_X 52
#define ZH_RAW_EFFECT_W 45
#define ZH_RAW_EFFECT_H 15
#define ZH_RAW_EFFECT_MAX 512
#define ZH_RAW_OUT_MAX 1024
#define ZH_RAW_CACHE_N 8
#define ZH_RAW_DATA_END 43
static const char zh_raw_crush_key[]="{n}% chance to crush robots when ramming them. Cannot crush targets of large or greater size, or those with more than {n} core integrity.";
static int ZhRawContainsRow(int y,const char *needle)
{
    char text[161];int x,n=0;
    for(x=0;x<g_grid.w;++x){char c=ChAt(x,y);if(c!=' '&&c)text[n++]=c;}
    text[n]=0;return strstr(text,needle)!=NULL;
}
static int ZhRawVisible(const XCell *c)
{return abs((int)c->fgR-c->bgR)+abs((int)c->fgG-c->bgG)+abs((int)c->fgB-c->bgB)>3;}
static int ZhRawEffectLabelRow(void)
{
    int y,x;
    for(y=12;y<ZH_RAW_DATA_END;++y){char label[16];int n=0;
        for(x=ZH_RAW_EFFECT_X;x<ZH_RAW_EFFECT_X+ZH_RAW_EFFECT_W;++x){char c=ChAt(x,y);
            if(c==' ')continue;if(c<32||c>126||n>=(int)sizeof(label)-1){n=0;break;}label[n++]=c;
        }
        label[n]=0;if(n&&strcmp(label,"Effect")==0)return y;
    }
    return -1;
}
static int ZhRawDataPageContext(void)
{
    return g_on&&g_grid.w==160&&g_grid.h==45&&g_cellW>0&&g_cellH==2*g_cellW&&
        ZhRawContainsRow(0,"/DATA/")&&ZhRawContainsRow(4,"Overview")&&
        ZhRawContainsRow(5,"Type")&&ZhRawContainsRow(6,"Slot");
}
static int ZhRawDataPageMode(void)
{
    return ZhRawDataPageContext()?1:0;
}
static int zh_raw_diag_count;
static void ZhRawDiag(int stage,const char *key) {
#ifndef ZH_RAW_HOST_TEST
 if(zh_raw_diag_count<20){ L("[zh-raw-para] stage=%d key=%s",stage,key?key:""); ++zh_raw_diag_count; }
#else
 (void)stage;(void)key;
#endif
}
static int DrawZhRawCrushParagraph(SSurf *surface)
{
    char key[ZH_RAW_CRUSH_MAX],out[MAX_TEXT],lines[ZH_RAW_CRUSH_H][ZH_RAW_CRUSH_MAX];
    int px[ZH_RAW_CRUSH_MAX],py[ZH_RAW_CRUSH_MAX],ink[ZH_RAW_CRUSH_MAX];
    int capidx[2],capvisible[2]={1,1},digitgroup=-1,wasdigit=0;
    int len=0,ni=0,row,x,i,k,hi,nlines=0,targetcells=0,offset=0,targetCellCursor=0,totalOutputCells;
    int sourceY=-1;
    Caps caps;Rule *hit=NULL;const char *q;
    if(!surface||!ZhRawDataPageContext()||surface->w<g_grid.w*g_cellW||surface->h<g_grid.h*g_cellH||
       !g_pa||!g_paArr||g_npa<=0)return 0;
    sourceY=ZhRawEffectLabelRow()+1;
    if(sourceY<=0||sourceY+ZH_RAW_CRUSH_H>ZH_RAW_DATA_END)return 0;
    /* Source rows are all present before their foreground is revealed. */
    for(row=0;row<ZH_RAW_CRUSH_H;++row){int n=ZH_RAW_CRUSH_W;
        while(n>0&&ChAt(ZH_RAW_CRUSH_X+n-1,sourceY+row)==' ')--n;
        if(!n||len+n+(row?1:0)>=ZH_RAW_CRUSH_MAX)return 0;
        if(row){key[len]=' ';px[len]=py[len]=-1;++len;}
        for(x=0;x<n;++x){char c=ChAt(ZH_RAW_CRUSH_X+x,sourceY+row);
            if(c<32||c>126)return 0;
            key[len]=c;px[len]=ZH_RAW_CRUSH_X+x;py[len]=sourceY+row;
            if(c!=' ')ink[ni++]=len;
            ++len;
        }
    }
    key[len]=0;if(!ni)return 0; ZhRawDiag(1,key);
    hi=g_paOfs[257];
    for(k=BkFit(g_pa,g_paArr,g_paOfs[256],hi,len);k<hi;++k){
        Rule *r=&g_pa[g_paArr[k]];int lit[MAX_TOK],nc=0,ok=1,j;
        if(!r->raw||strcmp(r->raw,zh_raw_crush_key)||r->ncap!=2||!cm_match(r,key,len,&caps,lit))continue;
        for(i=0;i<r->ns;++i)if(r->s[i].cap){
            if(nc>=2||r->s[i].cap!=1||caps.len[i]<=0){ok=0;break;}
            capidx[nc++]=i;
            for(j=0;j<caps.len[i];++j)if(key[caps.ofs[i]+j]<'0'||key[caps.ofs[i]+j]>'9')ok=0;
        }
        if(!ok||nc!=2)continue;
        cm_buildfree(r,key,&caps,out,sizeof out);hit=r;break;
    }
    if(!hit||!out[0]){ZhRawDiag(2,key);return 0;}
    totalOutputCells=cm_cells(out);if(totalOutputCells<=0)return 0;
    /* Numeric gates below require exactly these two preserved numeric groups. */
    {int nc=0;const char *p=out;
     while(*p){if(*p>='0'&&*p<='9'){const char *a=p;int token,n;
         while(*p>='0'&&*p<='9')++p;
         if(nc>=2)return 0;token=capidx[nc++];n=(int)(p-a);
         if(n!=caps.len[token]||memcmp(a,key+caps.ofs[token],(size_t)n))return 0;
     }else ++p;}if(nc!=2)return 0;}
    q=out;
    while(*q&&nlines<ZH_RAW_CRUSH_H){const char *next=cm_wrap(q,ZH_RAW_CRUSH_W,lines[nlines],sizeof lines[0]);
        if(next<=q||!lines[nlines][0]||cm_cells(lines[nlines])>ZH_RAW_CRUSH_W)return 0;
        targetcells+=cm_cells(lines[nlines++]);q=next;
    }
    if(*q||!targetcells)return 0;
    /* Captured native reveal includes green foreground/background pulses,
       not just the settled black-background palette. Conceal the matched
       source immediately; unrelated transient colors remain hidden. */
    for(i=0;i<2;++i){int t=capidx[i],j;
        for(j=0;j<caps.len[t];++j){int at=caps.ofs[t]+j;
            if(at<0||at>=len||px[at]<0||!ZhRawVisible(&CELL(px[at],py[at])))capvisible[i]=0;
        }
    }
    for(row=0;row<ZH_RAW_CRUSH_H;++row)for(x=0;x<ZH_RAW_CRUSH_W;++x){BYTE bg[3]={0,0,0};
        DrawSpan(surface,ZH_RAW_CRUSH_X+x,sourceY+row,1,1,0,bg,"",0,0,0);
    }
    for(row=0;row<nlines;++row){int col=ZH_RAW_CRUSH_X;const char *p=lines[row];
        while(*p){unsigned cp;const char *next=cm_u8next(p,&cp);int width=cm_wide(cp)?2:1;
            int sample=(offset+width/2)*ni/targetcells,at,digit=(cp>='0'&&cp<='9');
            const XCell *c;char glyph[8];size_t bytes=(size_t)(next-p);int show;
            if(sample>=ni)sample=ni-1;at=ink[sample];c=&CELL(px[at],py[at]);
            if(digit&&!wasdigit)++digitgroup;
            show=!c->fgR&&!c->fgB&&!c->bgR&&!c->bgB&&c->bgG<=64&&ZhRawVisible(c)&&(!digit||(digitgroup>=0&&digitgroup<2&&capvisible[digitgroup]));
            if(show&&bytes<sizeof glyph&&(!digit||c->fgG==178)){
                memcpy(glyph,p,bytes);glyph[bytes]=0;
                if(width==2&&!digit&&!ZhDecodeGlyphReady(0x52504152u,
                    ZH_RAW_CRUSH_X,sourceY,ZH_RAW_CRUSH_W,
                    ZH_RAW_CRUSH_H,out,(unsigned)GetTickCount(),(unsigned)g_frame,
                    (unsigned)targetCellCursor,(unsigned)totalOutputCells)){
                    unsigned seed=(unsigned)(col+row*17+c->fgR*3+c->fgG);
                    const char *noise=zh_animation_noise[seed%(sizeof zh_animation_noise/sizeof zh_animation_noise[0])];
                    DrawSpan(surface,col,sourceY+row,width,1,0,NULL,noise,c->fgR,c->fgG,c->fgB);
                }else
                    DrawSpan(surface,col,sourceY+row,width,1,0,NULL,glyph,c->fgR,c->fgG,c->fgB);
            }
            wasdigit=digit;col+=width;offset+=width;targetCellCursor+=width;p=next;
        }
    }
    ZhRawDiag(9,key);return 1;
}

/* Generic collector for item DATA descriptions. It operates only on exact,
   complete line groups in the confirmed 45-cell body column. */
typedef struct { unsigned hash; const Rule *table; int ntable,index; char key[ZH_RAW_EFFECT_MAX]; int used; } ZhRawRuleCache;
static ZhRawRuleCache zh_raw_rule_cache[ZH_RAW_CACHE_N];
static unsigned zh_raw_cache_clock;
static int ZhRawEffectPaletteKnown(const XCell *c)
{
    return ((!c->fgR&&!c->fgG&&!c->fgB&&!c->bgR&&!c->bgG&&!c->bgB)||
            (!c->fgR&&c->fgG==178&&!c->fgB&&!c->bgR&&!c->bgG&&!c->bgB)||
            (!c->fgR&&c->fgG==255&&!c->fgB&&!c->bgR&&c->bgG==64&&!c->bgB));
}
static int ZhRawEffectStable(const XCell *c)
{return !c->fgR&&c->fgG==178&&!c->fgB&&!c->bgR&&!c->bgG&&!c->bgB;}
static unsigned ZhRawHash(const char *s)
{unsigned h=2166136261u;while(*s){h^=(unsigned char)*s++;h*=16777619u;}return h;}
static int ZhRawSameCaptures(const Rule *a,const Caps *ac,const Rule *b,const Caps *bc)
{
    int i=0,j=0;
    for(;;){while(i<a->ns&&!a->s[i].cap)++i;while(j<b->ns&&!b->s[j].cap)++j;
        if(i==a->ns||j==b->ns)return i==a->ns&&j==b->ns;
        if(ac->ofs[i]!=bc->ofs[j]||ac->len[i]!=bc->len[j])return 0;
        ++i;++j;
    }
}
static int ZhRawLiteralState(const Rule *r,const int *litOfs,const int *px,const int *py,
                            int keylen,int *any,int *allvis,const XCell **sample)
{
    int i,j,stable=1;*any=0;*allvis=1;*sample=NULL;
    for(i=0;i<r->ns;++i)if(!r->s[i].cap)for(j=0;j<r->s[i].len;++j){
        int at=litOfs[i]+j;const XCell *c;
        if(at<0||at>=keylen)return 0;
        if(r->s[i].lit[j]==' '||px[at]<0||py[at]<0)continue;
        c=&CELL(px[at],py[at]);if(!ZhRawEffectPaletteKnown(c))return 0;
        if(ZhRawVisible(c)){*any=1;if(!*sample)*sample=c;}else *allvis=0;
        if(!ZhRawEffectStable(c))stable=0;
    }
    return stable;
}
static int ZhRawCaptureState(const Rule *r,const Caps *caps,int token,const char *key,
                             int len,const int *px,const int *py,int *vis,int *stable,
                             const XCell **sample)
{
    int j,any=0,all=1;*sample=NULL;
    if(token<0||token>=r->ns||!r->s[token].cap||caps->len[token]<=0||
       caps->ofs[token]<0||caps->ofs[token]+caps->len[token]>len)return 0;
    for(j=0;j<caps->len[token];++j){int at=caps->ofs[token]+j;const XCell *c;
        if(px[at]<0||py[at]<0)return 0;
        if(key[at]==' ')continue;
        c=&CELL(px[at],py[at]);if(!ZhRawEffectPaletteKnown(c))return 0;
        if(ZhRawVisible(c)){any=1;if(!*sample)*sample=c;}else all=0;
        if(!ZhRawEffectStable(c))all=0;
    }
    *vis=any;*stable=all;return 1;
}
static int ZhRawMatchRule(const char *key,int len,const int *px,const int *py,
                          Rule **hit,Caps *hitcaps,int *hitlit,char *out,int outsz)
{
    int k,pass,found=0,best=-1;unsigned hash;char bestout[ZH_RAW_OUT_MAX];
    Caps bestcaps={0};int bestlit[MAX_TOK]={0};
    if(!key||!out||outsz<=0)return 0;
    hash=ZhRawHash(key);
    for(k=0;k<ZH_RAW_CACHE_N;++k){ZhRawRuleCache *c=&zh_raw_rule_cache[k];
        if(c->used&&c->hash==hash&&c->table==g_pa&&c->ntable==g_npa&&!strcmp(c->key,key)&&c->index>=0&&c->index<g_npa){
            Rule *r=&g_pa[c->index];Caps cp={0};int lo[MAX_TOK]={0};char tmp[ZH_RAW_OUT_MAX];
            if(cm_match(r,key,len,&cp,lo)){cm_buildfree(r,key,&cp,tmp,sizeof tmp);
                if(tmp[0]&&strlen(tmp)<sizeof tmp-1&&strlen(tmp)<(size_t)outsz){*hit=r;*hitcaps=cp;memcpy(hitlit,lo,sizeof lo);memcpy(out,tmp,strlen(tmp)+1);return 1;}}
        }
    }
    for(pass=0;pass<2;++pass){int b=pass?256:(unsigned char)key[0],hi=g_paOfs[b+1];
        for(k=BkFit(g_pa,g_paArr,g_paOfs[b],hi,len);k<hi;++k){int idx=g_paArr[k],literal=0,i,capcount=0,lo[MAX_TOK]={0};
            Caps cp={0};Rule *r=&g_pa[g_paArr[k]];char tmp[ZH_RAW_OUT_MAX];
            if(r->raw&&!strcmp(r->raw,zh_raw_crush_key))continue; /* old validated path */
            if(r->sig&~cm_sig(key,len))continue;
            for(i=0;i<r->ns;++i)if(r->s[i].cap)++capcount;else literal+=r->s[i].len;
            if(literal<8||capcount!=r->ncap||!cm_match(r,key,len,&cp,lo))continue;
            if(cm_capover(r,&cp,40)||cm_capsent(r,key,&cp))continue;
            for(i=0;i<r->ns;++i)if(r->s[i].cap){int j;
                if(cp.ofs[i]<0||cp.len[i]<=0||cp.ofs[i]+cp.len[i]>len)break;
                for(j=0;j<cp.len[i];++j)if(px[cp.ofs[i]+j]<0||py[cp.ofs[i]+j]<0)break;
                if(j<cp.len[i])break;
            }
            if(i<r->ns)continue;
            cm_buildfree(r,key,&cp,tmp,sizeof tmp);if(!tmp[0]||strlen(tmp)>=sizeof tmp-1||strlen(tmp)>=(size_t)outsz)continue;
            if(found){
                if(strcmp(bestout,tmp))return 0;
                if(!ZhRawSameCaptures(&g_pa[best],&bestcaps,r,&cp))return 0;
                continue;
            }
            found=1;best=idx;memcpy(bestout,tmp,strlen(tmp)+1);bestcaps=cp;memcpy(bestlit,lo,sizeof lo);
        }
    }
    if(!found)return 0;
    if(strlen(bestout)>=(size_t)outsz)return 0;
    *hit=&g_pa[best];*hitcaps=bestcaps;memcpy(hitlit,bestlit,sizeof bestlit);memcpy(out,bestout,strlen(bestout)+1);
    {ZhRawRuleCache *c=&zh_raw_rule_cache[zh_raw_cache_clock++%ZH_RAW_CACHE_N];
     c->used=1;c->hash=hash;c->table=g_pa;c->ntable=g_npa;c->index=best;strcpy(c->key,key);}
    return 1;
}
static int ZhRawBuildProvenance(const Rule *r,const char *key,const Caps *caps,const char *full,int *owner)
{
    int i,prev=0,n=(int)strlen(full);
    if(n>=ZH_RAW_OUT_MAX-1)return 0;
    for(i=0;i<r->nd;++i){Rule prefix=*r;char partial[ZH_RAW_OUT_MAX];int end,j;
        prefix.nd=i+1;cm_buildfree(&prefix,key,caps,partial,sizeof partial);end=(int)strlen(partial);
        if(end>=sizeof partial-1)return 0;
        if(end<prev||end>n||memcmp(partial,full,(size_t)end))return 0;
        for(j=prev;j<end;++j)owner[j]=i;prev=end;
    }
    return prev==n;
}
static int ZhRawDrawMatchedBlock(SSurf *surface,int y0,int rows,const char *key,int len,
    int *px,int *py,Rule *r,Caps *caps,int *lit,const char *out)
{
    char lines[ZH_RAW_EFFECT_H][ZH_RAW_EFFECT_MAX];int owner[ZH_RAW_OUT_MAX];
    int litink[ZH_RAW_EFFECT_MAX],nlitink=0,targetLitCells=0;
    int capvis[MAX_TOK]={0},capstable[MAX_TOK]={0},litany=0,litall=0,litstable;
    const XCell *capsample[MAX_TOK]={0},*litsample=NULL;int captok[MAX_TOK],ncap=0;
    const char *lineEnd[ZH_RAW_EFFECT_H],*q=out;int nl=0,i,j;
    int targetCells=cm_cells(out),targetCellCursor=0;
    if(!ZhRawBuildProvenance(r,key,caps,out,owner))return 0;
    litstable=ZhRawLiteralState(r,lit,px,py,len,&litany,&litall,&litsample);
    /* Track literal glyphs independently. This lets early source letters
       reveal their mapped target letters before the whole paragraph settles. */
    for(i=0;i<r->ns;++i)if(!r->s[i].cap)for(j=0;j<r->s[i].len;++j){
        int at=lit[i]+j;
        if(at<0||at>=len)return 0;
        if(r->s[i].lit[j]!=' '){if(px[at]<0||py[at]<0)return 0;litink[nlitink++]=at;}
    }
    for(i=0;i<r->nd;++i)if(!r->d[i].cap){const char *p=r->d[i].lit;int remain=r->d[i].len;
        while(remain>0){unsigned cp;const char *nx=cm_u8next(p,&cp);int bytes=(int)(nx-p);
            if(bytes<=0||bytes>remain)return 0;targetLitCells+=cm_wide(cp)?2:1;p=nx;remain-=bytes;
        }
    }
    for(i=0;i<r->ns;++i)if(r->s[i].cap){
        captok[ncap]=i;if(!ZhRawCaptureState(r,caps,i,key,len,px,py,&capvis[i],&capstable[i],&capsample[i]))return 0;++ncap;
    }
    if(!litany&&ncap==0)return 0;
    /* Refuse unmeasured colors before hiding native text. */
    for(i=0;i<len;++i)if(px[i]>=0&&py[i]>=0&&key[i]!=' '&&!ZhRawEffectPaletteKnown(&CELL(px[i],py[i])))return 0;
    while(*q&&nl<rows){const char *next=cm_wrap(q,ZH_RAW_EFFECT_W,lines[nl],sizeof lines[0]);
        if(next<=q||!lines[nl][0]||cm_cells(lines[nl])>ZH_RAW_EFFECT_W)return 0;
        lineEnd[nl]=next;++nl;q=next;
    }
    if(*q||!nl||strchr(out,CM_BR)||strchr(out,CM_SHL))return 0;
    /* Validate wrapped-byte provenance before the first surface write. */
    {int cursor=0;
     for(i=0;i<nl;++i){const char *p=lines[i];
        while(*p){unsigned cp;const char *nx=cm_u8next(p,&cp);int bytes=(int)(nx-p);
            if(cursor<0||cursor+bytes>(int)strlen(out)||memcmp(out+cursor,p,(size_t)bytes)||owner[cursor]<0||owner[cursor]>=r->nd)return 0;
            cursor+=bytes;p=nx;
        }
        cursor=(int)(lineEnd[i]-out);
     }
    }
    for(i=0;i<rows;++i)for(j=ZH_RAW_EFFECT_X;j<ZH_RAW_EFFECT_X+ZH_RAW_EFFECT_W;++j){
        const XCell *c=&CELL(j,y0+i);BYTE bg[3]={c->bgR,c->bgG,c->bgB};DrawSpan(surface,j,y0+i,1,1,0,bg,"",0,0,0);
    }
    {int cursor=0,targetLitCursor=0;for(i=0;i<nl;++i){const char *p=lines[i];int col=ZH_RAW_EFFECT_X;
        while(*p){unsigned cp;const char *nx=cm_u8next(p,&cp);int bytes=(int)(nx-p),width=cm_wide(cp)?2:1,ti,sourceTok=-1,ready=0,vis=0;const XCell *sample=NULL;
            if(cursor<0||cursor+bytes>(int)strlen(out)||memcmp(out+cursor,p,(size_t)bytes))return 0;
            ti=owner[cursor];if(ti<0||ti>=r->nd)return 0;
            if(r->d[ti].cap){int ord,kord=0;
                if(r->d[ti].cap>=3)ord=r->d[ti].cap-3;
                else {for(j=0;j<ti;++j)if(r->d[j].cap&&r->d[j].cap<3)++kord;ord=kord;}
                if(ord<0||ord>=ncap)return 0;sourceTok=captok[ord];ready=capstable[sourceTok];vis=capvis[sourceTok];sample=capsample[sourceTok];
                if(r->s[sourceTok].cap==1&&!ready)vis=0; /* never tease a numeric capture */
            }else {
                int si;
                if(!nlitink||!targetLitCells)return 0;
                si=((targetLitCursor+width/2)*nlitink)/targetLitCells;
                if(si>=nlitink)si=nlitink-1;
                sample=&CELL(px[litink[si]],py[litink[si]]);
                if(!ZhRawEffectPaletteKnown(sample))return 0;
                vis=ZhRawVisible(sample);
                /* The caller owns palette visibility; bounded decode owns
                   only phonetic versus Chinese completion. */
                ready=vis;
            }
            if(ready&&sample){char glyph[8];int decoded=1;memcpy(glyph,p,(size_t)bytes);glyph[bytes]=0;
                /* Native palette/visibility remains the outer gate. The
                   phonetic-to-Chinese phase is keyed to the complete decoded
                   target and root paragraph rectangle, never to a native
                   color being interpreted as an animation completion signal. */
                if(width==2&&!(r->d[ti].cap&&r->s[sourceTok].cap==1))
                    decoded=ZhDecodeGlyphReady(0x52504152u,ZH_RAW_EFFECT_X,y0,
                        ZH_RAW_EFFECT_W,rows,out,(unsigned)GetTickCount(),
                        (unsigned)g_frame,(unsigned)targetCellCursor,
                        (unsigned)targetCells);
                if(decoded)DrawSpan(surface,col,y0+i,width,1,0,NULL,glyph,sample->fgR,sample->fgG,sample->fgB);
                else if(width==2){unsigned seed=(unsigned)(col+(y0+i)*17+sample->fgR*3+sample->fgG);
                    const char *noise=zh_animation_noise[seed%(sizeof zh_animation_noise/sizeof zh_animation_noise[0])];
                    DrawSpan(surface,col,y0+i,width,1,0,NULL,noise,sample->fgR,sample->fgG,sample->fgB);
                }
            }
            if(!r->d[ti].cap)targetLitCursor+=width;
            targetCellCursor+=width;
            cursor+=bytes;col+=width;p=nx;
        }
        cursor=(int)(lineEnd[i]-out);
    }}
    return 1;
}
static int ZhRawCollectParagraph(int first,int *last,char *key,int *px,int *py)
{
    int y=first,len=0,rows=0;
    while(y<ZH_RAW_DATA_END&&rows<8){int a=ZH_RAW_EFFECT_X,b=ZH_RAW_EFFECT_X+ZH_RAW_EFFECT_W,x;
        while(a<b&&ChAt(a,y)==' ')++a;while(b>a&&ChAt(b-1,y)==' ')--b;
        if(a==b||ChAt(a,y)=='<')break;
        if(rows){if(len+1>=ZH_RAW_EFFECT_MAX)return 0;key[len]=' ';px[len]=py[len]=-1;++len;}
        for(x=a;x<b;++x){char c=ChAt(x,y);if(c<32||c>126||len+1>=ZH_RAW_EFFECT_MAX)return 0;
            key[len]=c;px[len]=x;py[len]=y;++len;
        }
        ++rows;++y;
    }
    if(!rows||!len)return 0;key[len]=0;*last=y-1;return rows;
}
static int DrawZhRawEffectParagraphs(SSurf *surface)
{
    int mode=ZhRawDataPageMode(),effectRow,y,painted=0;
    if(!mode)return 0;
    effectRow=ZhRawEffectLabelRow();y=effectRow>=0?effectRow+1:27;
    while(y<ZH_RAW_DATA_END){char key[ZH_RAW_EFFECT_MAX],out[ZH_RAW_OUT_MAX];
        int px[ZH_RAW_EFFECT_MAX],py[ZH_RAW_EFFECT_MAX],last,rows,len,lit[MAX_TOK]={0};Rule *r=NULL;Caps caps={0};
        rows=ZhRawCollectParagraph(y,&last,key,px,py);
        if(!rows){++y;continue;}
        len=(int)strlen(key);
        if(ZhRawMatchRule(key,len,px,py,&r,&caps,lit,out,sizeof out)&&
           ZhRawDrawMatchedBlock(surface,y,rows,key,len,px,py,r,&caps,lit,out))painted=1;
        y=last+1;
        while(y<ZH_RAW_DATA_END){int a=ZH_RAW_EFFECT_X,b=ZH_RAW_EFFECT_X+ZH_RAW_EFFECT_W;
            while(a<b&&ChAt(a,y)==' ')++a;if(a==b||ChAt(a,y)=='<'){++y;break;}break;
        }
        if(rows>=8){while(y<ZH_RAW_DATA_END){int a=ZH_RAW_EFFECT_X,b=ZH_RAW_EFFECT_X+ZH_RAW_EFFECT_W;while(a<b&&ChAt(a,y)==' ')++a;if(a==b||ChAt(a,y)=='<')break;++y;}}
    }
    return painted;
}
static int DrawZhRawParagraphs(SSurf *surface)
{
    int mode,a=0,b;if(!surface||!(mode=ZhRawDataPageMode())||surface->w<g_grid.w*g_cellW||surface->h<g_grid.h*g_cellH||!g_pa||!g_paArr||g_npa<=0)return 0;
    if(mode==1)a=DrawZhRawCrushParagraph(surface);b=DrawZhRawEffectParagraphs(surface);return a||b;
}
#endif
