/* Draw short translated text from child XConsole grids using their native
   per-cell fade colors. The child/source memory is observed through RPM only. */
#ifndef ZH_CHILD_TEXT_DRAW_H
#define ZH_CHILD_TEXT_DRAW_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "zh_decode_state.h"

#define ZH_CHILD_TEXT_MAX 128
#define ZH_CHILD_TEXT_MAX_CONSOLES 16
#define ZH_CHILD_TEXT_MAX_SOURCE 160
#define ZH_CHILD_TEXT_MAX_GRID_CELLS 16000

typedef struct {
    DWORD f0,glyph,f8;
    BYTE fgR,fgG,fgB,bgR,bgG,bgB;
    WORD pad;
} ZhChildTextCell;

typedef struct {
    int w,h,anchor_x,anchor_y,project_x,project_y;
    DWORD cells,parent;
    int scale_index;
} ZhChildTextHeader;

typedef struct {
    BYTE fgR,fgG,fgB,bgR,bgG,bgB;
} ZhChildTextColor;

typedef struct {
    int active;
    void *self;
    void *source_object;
    ZhChildTextHeader header;
    int local_x,local_y,root_x,root_y;
    unsigned length,target_cells;
    unsigned target_contrast;
    unsigned draw_count;
    DWORD registered_tick;
    int dark_baseline;
    unsigned long stamp;
    char source[ZH_CHILD_TEXT_MAX_SOURCE+1];
    char target[MAX_TEXT];
    ZhChildTextColor baseline[ZH_CHILD_TEXT_MAX_SOURCE];
} ZhChildTextEntry;

typedef struct {
    void *self;
    ZhChildTextHeader header;
    ZhChildTextCell *cells;
    int loaded,invalid;
} ZhChildTextFrameConsole;

static ZhChildTextEntry zh_child_text_entries[ZH_CHILD_TEXT_MAX];
static unsigned long zh_child_text_clock;
static unsigned zh_child_text_log_count;

static int ZhChildTextIsDiagnosticSource(const char *source,unsigned length)
{
    static const char *wanted[]={"Click to expand/shrink","- On enemy to target and fire",
        "- HIBERNATE -","(Save & Quit)","Save & Quit","[Save & Quit]"};
    unsigned i,j;
    for(j=0;j<sizeof wanted/sizeof wanted[0];++j){unsigned n=(unsigned)strlen(wanted[j]);if(length!=n)continue;
        for(i=0;i<length;++i){char a=source[i],b=wanted[j][i];if(a>='a'&&a<='z')a-=32;if(b>='a'&&b<='z')b-=32;if(a!=b)break;}
        if(i==length)return 1;
    }
    return 0;
}

static unsigned ZhChildTextNavPrefix(const char *source,unsigned length)
{
    if(length>=4&&source[0]=='['&&source[1]>='0'&&source[1]<='9'&&source[2]==']'&&source[3]==' ')return 4u;
    if(length>=4&&source[0]=='('&&source[1]>=33&&source[1]<=126&&source[1]!=')'&&
       source[2]==')'&&source[3]==' ')return 4u;
    return 0;
}

static int ZhChildTextStableRange(const char *source,unsigned length,unsigned *begin,unsigned *end)
{
    unsigned i,start=0,stop=length,letters=0;
    if(ZhChildTextNavPrefix(source,length))start=4;
    else if(length>=2&&source[0]=='['&&source[length-1]==']'){
        start=1;stop=length-1;
        for(i=start;i<stop;++i){unsigned char c=(unsigned char)source[i];
            if(c=='['||c==']')return 0;
            if((c>='A'&&c<='Z')||(c>='a'&&c<='z'))++letters;
        }
        if(letters<2)return 0;
    }else if(length>=4&&source[0]=='('&&source[1]>=33&&source[1]<=126&&
             source[1]!=')'&&source[2]==')'&&source[3]==' ')start=4;
    *begin=start;*end=stop;return 1;
}

static void ZhChildTextLog(const char *source,unsigned length,const char *stage,
                           void *self,int x,int y,int w,int h,int ax,int ay,
                           int px,int py,int extra)
{
    if (zh_child_text_log_count>=64 || !ZhChildTextIsDiagnosticSource(source,length)) return;
    ++zh_child_text_log_count;
    L("[child-text-diag] stage=%s self=%p root=%p xy=%d,%d grid=%dx%d anchor=%d,%d project=%d,%d extra=%d tick=%u frame=%u src=%.40s",
      stage,self,g_console,x,y,w,h,ax,ay,px,py,extra,(unsigned)GetTickCount(),(unsigned)g_frame,source);
}

static void ZhChildTextLogRootDuplicate(const ZhChildTextEntry *e,
                                         const ZhChildTextCell *child,
                                         const char *stage)
{
    const XCell *root=NULL;
    const ZhChildTextColor *base=&e->baseline[0];
    const ZhChildTextCell *c=&child[(SIZE_T)e->local_x*(SIZE_T)e->header.h+(SIZE_T)e->local_y];
    if(g_grid.cells&&e->root_x>=0&&e->root_y>=0&&e->root_x<g_grid.w&&e->root_y<g_grid.h)
        root=&CELL(e->root_x,e->root_y);
    if(zh_child_text_log_count>=64||!ZhChildTextIsDiagnosticSource(e->source,e->length))return;
    ++zh_child_text_log_count;
    L("[child-text-diag] stage=%s self=%p root=%p at=%d,%d rootglyph=%u rootfg=%u,%u,%u rootbg=%u,%u,%u childglyph=%u childfg=%u,%u,%u childbg=%u,%u,%u basefg=%u,%u,%u basebg=%u,%u,%u draws=%u tick=%u registered=%u age=%u frame=%u src=%.40s",
      stage,e->self,g_console,e->root_x,e->root_y,root?root->glyph:0,
      root?root->fgR:0,root?root->fgG:0,root?root->fgB:0,root?root->bgR:0,root?root->bgG:0,root?root->bgB:0,
      c->glyph,c->fgR,c->fgG,c->fgB,c->bgR,c->bgG,c->bgB,
      base->fgR,base->fgG,base->fgB,base->bgR,base->bgG,base->bgB,e->draw_count,
      (unsigned)GetTickCount(),(unsigned)e->registered_tick,(unsigned)(GetTickCount()-e->registered_tick),
      (unsigned)g_frame,e->source);
}

static int ZhChildTextRead(const void *address,void *out,SIZE_T size)
{
    SIZE_T got=0;
    if (!address || !out || !size) return 0;
    return ReadProcessMemory(GetCurrentProcess(),address,out,size,&got) && got==size;
}

static int ZhChildTextReadString(void *object,char *out,unsigned *out_length)
{
    BYTE bytes[0x1c];
    DWORD length,capacity,data_pointer;
    const void *data;
    unsigned i;
    if (!object || !ZhChildTextRead(object,bytes,sizeof bytes)) return 0;
    memcpy(&length,bytes+0x10,4);memcpy(&capacity,bytes+0x14,4);
    if (!length || length>ZH_CHILD_TEXT_MAX_SOURCE || capacity<length) return 0;
    if (capacity<16) {
        if (length>=16) return 0;
        data=object;
    } else {
        memcpy(&data_pointer,bytes,4);
        if (!data_pointer) return 0;
        data=(const void *)(ULONG_PTR)data_pointer;
    }
    if (!ZhChildTextRead(data,out,(SIZE_T)length)) return 0;
    for (i=0;i<length;++i)
        if ((unsigned char)out[i]<32 || (unsigned char)out[i]>126) return 0;
    out[length]=0;*out_length=length;return 1;
}

static int ZhChildTextHeaderRead(void *self,ZhChildTextHeader *h)
{
    BYTE bytes[0x2c];
    if (!self || !h || !ZhChildTextRead(self,bytes,sizeof bytes)) return 0;
    memcpy(&h->w,bytes+0x08,4);memcpy(&h->h,bytes+0x0c,4);
    memcpy(&h->cells,bytes+0x10,4);
    memcpy(&h->parent,bytes+0x04,4);memcpy(&h->scale_index,bytes+0x18,4);
    memcpy(&h->anchor_x,bytes+0x1c,4);memcpy(&h->anchor_y,bytes+0x20,4);
    memcpy(&h->project_x,bytes+0x24,4);memcpy(&h->project_y,bytes+0x28,4);
    if (h->w<=0 || h->h<=0 || h->w>400 || h->h>200 || !h->cells ||
        (SIZE_T)h->w*(SIZE_T)h->h>ZH_CHILD_TEXT_MAX_GRID_CELLS) return 0;
    return 1;
}

static int ZhChildTextHeaderEqual(const ZhChildTextHeader *a,const ZhChildTextHeader *b)
{
    return a->w==b->w && a->h==b->h && a->cells==b->cells &&
        a->anchor_x==b->anchor_x && a->anchor_y==b->anchor_y &&
        a->project_x==b->project_x && a->project_y==b->project_y &&
        a->parent==b->parent && a->scale_index==b->scale_index;
}

/* Resolve only the observed unit-scale native parent hierarchy. The EXE's
   scale tables make index 0 exactly (1,1); unknown/non-unit scales are refused. */
static int ZhChildTextResolveRootPosition(void *self,const ZhChildTextHeader *initial,
                                          int *root_x,int *root_y)
{
    void *seen[8];void *node=self;ZhChildTextHeader current;int depth=0;
    if(!self||!initial||!root_x||!root_y)return 0;
    current=*initial;
    for(;;){
        void *parent;
        int i;
        if(depth>=8||current.scale_index!=0)return 0;
        for(i=0;i<depth;++i)if(seen[i]==node)return 0;
        seen[depth++]=node;
        parent=(void *)(ULONG_PTR)current.parent;
        if(!parent){
            if(node!=g_console||current.w!=160||current.h!=45||
               current.project_x!=0||current.project_y!=0)return 0;
            *root_x=initial->project_x;*root_y=initial->project_y;return 1;
        }
        {
            ZhChildTextHeader ph;
            long long ex,ey;
            if(!ZhChildTextHeaderRead(parent,&ph)||ph.scale_index!=0)return 0;
            ex=(long long)ph.project_x+(long long)current.anchor_x;
            ey=(long long)ph.project_y+(long long)current.anchor_y;
            if(ex<INT_MIN||ex>INT_MAX||ey<INT_MIN||ey>INT_MAX||
               current.project_x!=(int)ex||current.project_y!=(int)ey)return 0;
            node=parent;current=ph;
        }
    }
}

static int ZhChildTextCellAt(const ZhChildTextCell *cells,int h,int x,int y,
                             ZhChildTextCell *out)
{
    if (!cells || !out || x<0 || y<0 || h<=0) return 0;
    *out=cells[(SIZE_T)x*(SIZE_T)h+(SIZE_T)y];return 1;
}

static int ZhChildTextFoldChar(char a,char b)
{
    unsigned char x=(unsigned char)a,y=(unsigned char)b;
    if (x>='a' && x<='z') x=(unsigned char)(x-('a'-'A'));
    if (y>='a' && y<='z') y=(unsigned char)(y-('a'-'A'));
    return x==y;
}

static int ZhChildTextEqualFold(const char *a,const char *b,unsigned len)
{
    unsigned i;
    for (i=0;i<len;++i) if (!ZhChildTextFoldChar(a[i],b[i])) return 0;
    return 1;
}

static int ZhChildTextHasCjk(const char *text)
{
    const char *p=text;
    while (*p) {unsigned cp;const char *next=cm_u8next(p,&cp);if(cm_wide(cp))return 1;p=next;}
    return 0;
}

static int ZhChildTextRootHud(void)
{
    int x;
    if (g_grid.w!=160 || g_grid.h!=45) return 0;
    for (x=0;x+6<g_grid.w;++x)
        if (ChAt(x,0)=='/' && ChAt(x+2,0)=='L' &&
            ChAt(x+4,0)=='O' && ChAt(x+6,0)=='G') return 1;
    return 0;
}

static Rule *ZhChildTextFindRule(const char *source,int length)
{
    int first=(unsigned char)source[0],hi,k;
    if (!g_tr || !g_trArr || g_ntr<=0) return NULL;
    hi=g_trOfs[first+1];
    for (k=BkFit(g_tr,g_trArr,g_trOfs[first],hi,length);k<hi;++k) {
        Rule *r=&g_tr[g_trArr[k]];Caps cp;int litOfs[MAX_TOK];
        if (r->ns!=1 || r->ncap || r->s[0].cap || r->s[0].len!=length ||
            r->nd!=1 || r->d[0].cap || r->d[0].len<=0 || r->d[0].len>=MAX_TEXT ||
            !cm_match(r,source,length,&cp,litOfs)) continue;
        if (!ZhChildTextHasCjk(r->d[0].lit) || cm_cells(r->d[0].lit)>length ||
            memchr(r->d[0].lit,CM_BR,(size_t)r->d[0].len) ||
            memchr(r->d[0].lit,CM_SHL,(size_t)r->d[0].len)) continue;
        return r;
    }
    return NULL;
}

static int ZhChildTextSnapshot(const ZhChildTextHeader *h,ZhChildTextCell **out)
{
    SIZE_T bytes=(SIZE_T)h->w*(SIZE_T)h->h*sizeof(ZhChildTextCell);
    ZhChildTextCell *cells=(ZhChildTextCell *)malloc(bytes);
    if (!cells) return 0;
    if (!ZhChildTextRead((const void *)(ULONG_PTR)h->cells,cells,bytes)) {free(cells);return 0;}
    *out=cells;return 1;
}

/* 0=no matching root source, 1=handoff ready, 2=matching source still fades.
   Non-dark baselines retain the prior immediate handoff behavior. */
static int ZhChildTextRootDuplicate(const ZhChildTextEntry *e,
                                    const ZhChildTextCell *child)
{
    int i,have=0,root_uniform=1,child_uniform=1,colors_equal=1;
    unsigned body_start=0,body_end=e->length;
    BYTE rfr=0,rfg=0,rfb=0,rbr=0,rbg=0,rbb=0,cfr=0,cfg=0,cfb=0,cbr=0,cbg=0,cbb=0;
    if(e->root_x<0||e->root_y<0||e->root_y>=g_grid.h||e->root_x+(int)e->length>g_grid.w||!g_grid.cells)return 0;
    for(i=0;i<(int)e->length;++i){
        const XCell *r=&CELL(e->root_x+i,e->root_y);
        if(!ZhChildTextFoldChar((char)r->glyph,e->source[i]))return 0;
    }
    if(!e->dark_baseline)return 1;
    if(!ZhChildTextStableRange(e->source,e->length,&body_start,&body_end))return 2;
    for(i=0;i<(int)e->length;++i){
        const XCell *r=&CELL(e->root_x+i,e->root_y);
        const ZhChildTextCell *c=&child[(SIZE_T)(e->local_x+i)*(SIZE_T)e->header.h+(SIZE_T)e->local_y];
        if((unsigned)i<body_start||(unsigned)i>=body_end||e->source[i]==' ')continue;
        if(!have){rfr=r->fgR;rfg=r->fgG;rfb=r->fgB;rbr=r->bgR;rbg=r->bgG;rbb=r->bgB;
            cfr=c->fgR;cfg=c->fgG;cfb=c->fgB;cbr=c->bgR;cbg=c->bgG;cbb=c->bgB;have=1;}
        else{
            if(r->fgR!=rfr||r->fgG!=rfg||r->fgB!=rfb||r->bgR!=rbr||r->bgG!=rbg||r->bgB!=rbb)root_uniform=0;
            if(c->fgR!=cfr||c->fgG!=cfg||c->fgB!=cfb||c->bgR!=cbr||c->bgG!=cbg||c->bgB!=cbb)child_uniform=0;
        }
        if(r->fgR!=c->fgR||r->fgG!=c->fgG||r->fgB!=c->fgB||r->bgR!=c->bgR||r->bgG!=c->bgG||r->bgB!=c->bgB)colors_equal=0;
    }
    if(!have)return 2;
    if(abs((int)rfr-(int)rbr)+abs((int)rfg-(int)rbg)+abs((int)rfb-(int)rbb)<=3||
       abs((int)cfr-(int)cbr)+abs((int)cfg-(int)cbg)+abs((int)cfb-(int)cbb)<=3)root_uniform=child_uniform=0;
    return root_uniform&&child_uniform&&colors_equal?1:2;
}

static void ZhForgetChildText(void *self)
{
    int i;
    if (!self) return;
    for (i=0;i<ZH_CHILD_TEXT_MAX;++i)
        if (zh_child_text_entries[i].active && zh_child_text_entries[i].self==self)
            zh_child_text_entries[i].active=0;
}

/* Optional evidence-fed completion threshold. Call only when another native
   hook has established the real target foreground/background contrast for
   this exact text row. Zero means no verified target is available. */
static int ZhSetChildTextTargetContrast(void *self,int x,int y,unsigned contrast)
{
    int i;
    if(!self || !contrast || contrast>765) return 0;
    for(i=0;i<ZH_CHILD_TEXT_MAX;++i){ZhChildTextEntry *e=&zh_child_text_entries[i];
        if(e->active&&e->self==self&&e->local_x==x&&e->local_y==y){e->target_contrast=contrast;return 1;}}
    return 0;
}

/* Call only after the native 0x418320/0x418370 function returns. */
static void ZhRememberChildText(void *self,int x,int y,void *source_object)
{
    char source[ZH_CHILD_TEXT_MAX_SOURCE+1];unsigned length,i,leading=0,trailing;
    ZhChildTextHeader header;ZhChildTextCell *cells=NULL;Rule *rule;
    ZhChildTextEntry *entry=NULL;int root_x,root_y,project_x,project_y,slot=-1;
    ZhChildTextColor baseline[ZH_CHILD_TEXT_MAX_SOURCE];
    unsigned baseline_valid=0;
    int marker=0,mi;
    if (!source_object || !ZhChildTextReadString(source_object,source,&length)) return;
    /* Native callers may include alignment padding in their EngineString.
       Match only the visible span and keep the padding cells untouched. */
    while(leading<length&&source[leading]==' ')++leading;
    trailing=length;
    while(trailing>leading&&source[trailing-1]==' ')--trailing;
    if(trailing<=leading)return;
    if(leading){memmove(source,source+leading,(size_t)(trailing-leading));x+=(int)leading;}
    length=trailing-leading;source[length]=0;
    if (!ZhChildTextIsDiagnosticSource(source,length)) {
        /* The diagnostic is intentionally narrow; normal sources stay silent. */
    }
    for(mi=0;mi+6<g_grid.w;++mi) if(ChAt(mi,0)=='/'&&ChAt(mi+2,0)=='L'&&ChAt(mi+4,0)=='O'&&ChAt(mi+6,0)=='G'){marker=1;break;}
    if (!g_on) {ZhChildTextLog(source,length,"register:g_on",self,x,y,0,0,0,0,0,0,0);return;}
    if (!self) {ZhChildTextLog(source,length,"register:null-self",self,x,y,0,0,0,0,0,0,0);return;}
    if (self==g_console) {ZhChildTextLog(source,length,"register:root-self",self,x,y,0,0,0,0,0,0,0);return;}
    if (g_grid.w!=160 || g_grid.h!=45 || !marker) {ZhChildTextLog(source,length,"register:hud",self,x,y,g_grid.w,g_grid.h,0,0,0,0,marker);return;}
    if (g_cellW<=0 || g_cellH!=g_cellW*2) {ZhChildTextLog(source,length,"register:geometry",self,x,y,g_cellW,g_cellH,0,0,0,0,0);return;}
    if (!ZhChildTextHeaderRead(self,&header)) {
        BYTE hb[0x2c];int w=0,h=0,ax=0,ay=0,px=0,py=0,reason=0;DWORD cp=0;
        if(ZhChildTextRead(self,hb,sizeof hb)){memcpy(&w,hb+8,4);memcpy(&h,hb+0xc,4);memcpy(&cp,hb+0x10,4);memcpy(&ax,hb+0x1c,4);memcpy(&ay,hb+0x20,4);memcpy(&px,hb+0x24,4);memcpy(&py,hb+0x28,4);reason=(w<=0||h<=0||w>400||h>200||!cp||(SIZE_T)w*(SIZE_T)h>ZH_CHILD_TEXT_MAX_GRID_CELLS)?1:2;}
        ZhChildTextLog(source,length,"register:header-invalid",self,x,y,w,h,ax,ay,px,py,reason);return;
    }
    if (x<0 || y<0 || x+(int)length>header.w || y>=header.h) {ZhChildTextLog(source,length,"register:child-bounds",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,(int)length);return;}
    if(!ZhChildTextResolveRootPosition(self,&header,&project_x,&project_y)){ZhChildTextLog(source,length,"register:parent-chain",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,header.scale_index);return;}
    root_x=project_x+x;root_y=project_y+y;
    if (root_x<0 || root_y<0 || root_x+(int)length>g_grid.w || root_y>=g_grid.h) {ZhChildTextLog(source,length,"register:root-bounds",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,(int)length);return;}
    rule=ZhChildTextFindRule(source,(int)length);
    if (!rule) {ZhChildTextLog(source,length,"register:rule-not-found",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);return;}
    if (!ZhChildTextSnapshot(&header,&cells)) {ZhChildTextLog(source,length,"register:cells-read",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);return;}
    for (i=0;i<length;++i) {
        ZhChildTextCell c;
        char ch=(char)cells[(SIZE_T)(x+(int)i)*(SIZE_T)header.h+(SIZE_T)y].glyph;
        if (!ZhChildTextFoldChar(ch,source[i])) {free(cells);ZhChildTextLog(source,length,"register:glyph-mismatch",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,(int)i);return;}
        c=cells[(SIZE_T)(x+(int)i)*(SIZE_T)header.h+(SIZE_T)y];
        baseline[i].fgR=c.fgR;baseline[i].fgG=c.fgG;baseline[i].fgB=c.fgB;
        baseline[i].bgR=c.bgR;baseline[i].bgG=c.bgG;baseline[i].bgB=c.bgB;
        if(source[i]!=' ' && abs((int)c.fgR-(int)c.bgR)+abs((int)c.fgG-(int)c.bgG)+
           abs((int)c.fgB-(int)c.bgB)>3) ++baseline_valid;
    }
    free(cells);
    for (i=0;i<ZH_CHILD_TEXT_MAX;++i) {
        ZhChildTextEntry *candidate=&zh_child_text_entries[i];
        if (candidate->active && candidate->self==self &&
            candidate->local_x==x && candidate->local_y==y) {entry=candidate;break;}
        if (!candidate->active && slot<0) slot=(int)i;
    }
    if (!entry) {
        int consoles=0,found=0,j;void *oldest_self=NULL;unsigned long oldest=~0UL;
        for (i=0;i<ZH_CHILD_TEXT_MAX;++i) if (zh_child_text_entries[i].active) {
            int first=1;
            for (j=0;j<(int)i;++j) if (zh_child_text_entries[j].active &&
                zh_child_text_entries[j].self==zh_child_text_entries[i].self) {first=0;break;}
            if (!first) continue;
            ++consoles;if (zh_child_text_entries[i].self==self) found=1;
            if (zh_child_text_entries[i].stamp<oldest) {oldest=zh_child_text_entries[i].stamp;oldest_self=zh_child_text_entries[i].self;}
        }
        if (!found && consoles>=ZH_CHILD_TEXT_MAX_CONSOLES && oldest_self) ZhForgetChildText(oldest_self);
        if (slot<0) {
            unsigned long least=~0UL;
            for (i=0;i<ZH_CHILD_TEXT_MAX;++i) if (zh_child_text_entries[i].stamp<least) {
                least=zh_child_text_entries[i].stamp;slot=(int)i;
            }
        }
        entry=&zh_child_text_entries[slot];memset(entry,0,sizeof *entry);
    }
    entry->active=1;entry->self=self;entry->source_object=source_object;
    entry->header=header;entry->local_x=x;entry->local_y=y;entry->root_x=root_x;entry->root_y=root_y;
    entry->length=length;entry->target_cells=(unsigned)cm_cells(rule->d[0].lit);
    entry->target_contrast=0;
    entry->dark_baseline=(baseline_valid==0);
    memcpy(entry->source,source,length+1);memcpy(entry->target,rule->d[0].lit,(size_t)rule->d[0].len);
    entry->target[rule->d[0].len]=0;memcpy(entry->baseline,baseline,(size_t)length*sizeof baseline[0]);
    entry->stamp=++zh_child_text_clock;entry->registered_tick=GetTickCount();entry->draw_count=0;
    ZhChildTextLog(source,length,"register:ok",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,(int)entry->target_cells);
    if(!baseline_valid)ZhChildTextLog(source,length,"register:baseline-pending",self,x,y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);
}

static const char *zh_child_text_zhuyin[]={
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

static int ZhChildTextIntensity(const ZhChildTextCell *c)
{
    return abs((int)c->fgR-(int)c->bgR)+abs((int)c->fgG-(int)c->bgG)+
           abs((int)c->fgB-(int)c->bgB);
}

static void ZhChildTextDrawEntry(SSurf *surface,ZhChildTextEntry *e,
                                 const ZhChildTextCell *grid)
{
    int ink_index[ZH_CHILD_TEXT_MAX_SOURCE],ink=0,i,offset=0;
    const char *p=e->target;
    BYTE clear_bg[3];
    for (i=0;i<(int)e->length;++i) if (e->source[i]!=' ') ink_index[ink++]=i;
    if (!ink) return;
    /* Erase native text cell by cell with that same cell's current background. */
    for (i=0;i<(int)e->length;++i) {
        const ZhChildTextCell *c=&grid[(SIZE_T)(e->local_x+i)*(SIZE_T)e->header.h+(SIZE_T)e->local_y];
        clear_bg[0]=c->bgR;clear_bg[1]=c->bgG;clear_bg[2]=c->bgB;
        DrawSpan(surface,e->root_x+i,e->root_y,1,1,0,clear_bg,"",c->fgR,c->fgG,c->fgB);
    }
    while (*p) {
        unsigned cp;const char *next=cm_u8next(p,&cp);int width=cm_wide(cp)?2:1;
        int sample=(offset+width/2)*ink/(int)e->target_cells;
        int source_i;const ZhChildTextCell *current;const ZhChildTextColor *base;
        int cur,denom;double ratio;BYTE bg[3];char glyph[8];size_t bytes=(size_t)(next-p);
        if (sample>=ink) sample=ink-1;
        source_i=ink_index[sample];
        current=&grid[(SIZE_T)(e->local_x+source_i)*(SIZE_T)e->header.h+(SIZE_T)e->local_y];
        base=&e->baseline[source_i];cur=ZhChildTextIntensity(current);
        denom=e->target_contrast?(int)e->target_contrast:
              abs((int)base->fgR-(int)base->bgR)+abs((int)base->fgG-(int)base->bgG)+
              abs((int)base->fgB-(int)base->bgB);
        ratio=denom>0?(double)cur/(double)denom:0.0;
        if (cur>3 && offset+width<=e->target_cells && bytes<sizeof glyph) {
            /* Native consoles can be destroyed/recreated on every HUD frame.
               Track visible root-span continuity independently of their lifetime. */
            int decoded=cm_wide(cp)?ZhDecodeGlyphReady(0x4348494cu,
                e->root_x,e->root_y,(int)e->length,1,e->target,
                (unsigned)GetTickCount(),(unsigned)g_frame,
                (unsigned)offset,e->target_cells):1;
            memcpy(glyph,p,bytes);glyph[bytes]=0;
            bg[0]=current->bgR;bg[1]=current->bgG;bg[2]=current->bgB;
            /* Native colors still supply visibility and opacity. Decoding is
               our own bounded effect: a dark/unknown baseline must never
               leave phonetics waiting forever for an unknowable ratio. */
            if (ratio>=0.80 || !cm_wide(cp) ||
                decoded)
                DrawSpan(surface,e->root_x+offset,e->root_y,width,1,0,bg,glyph,
                         current->fgR,current->fgG,current->fgB);
            else {
                unsigned seed=(unsigned)(e->root_x*7+e->root_y*13+offset+
                    current->fgR+current->fgG*3+current->fgB*5);
                DrawSpan(surface,e->root_x+offset,e->root_y,width,1,0,bg,
                    zh_child_text_zhuyin[seed%(sizeof zh_child_text_zhuyin/sizeof zh_child_text_zhuyin[0])],
                    current->fgR,current->fgG,current->fgB);
            }
        }
        offset+=width;p=next;
    }
}

static void DrawZhChildTexts(SSurf *surface)
{
    ZhChildTextFrameConsole consoles[ZH_CHILD_TEXT_MAX_CONSOLES];
    int ncon=0,i,j;
    int marker=0,mx;
    for(mx=0;mx+6<g_grid.w;++mx)if(ChAt(mx,0)=='/'&&ChAt(mx+2,0)=='L'&&ChAt(mx+4,0)=='O'&&ChAt(mx+6,0)=='G'){marker=1;break;}
    if (!surface || !g_on || g_grid.w!=160 || g_grid.h!=45 || !marker || g_cellW<=0 ||
        g_cellH!=g_cellW*2 || surface->w<g_grid.w*g_cellW || surface->h<g_grid.h*g_cellH) {
        for(i=0;i<ZH_CHILD_TEXT_MAX;++i)if(zh_child_text_entries[i].active)
            ZhChildTextLog(zh_child_text_entries[i].source,zh_child_text_entries[i].length,
                !surface?"draw:null-surface":!g_on?"draw:g_on":!marker?"draw:hud-marker":
                (g_cellW<=0||g_cellH!=g_cellW*2)?"draw:geometry":"draw:surface-bounds",
                zh_child_text_entries[i].self,zh_child_text_entries[i].local_x,zh_child_text_entries[i].local_y,
                g_grid.w,g_grid.h,0,0,0,0,surface?surface->w:0);
        return;
    }
    memset(consoles,0,sizeof consoles);
    for (i=0;i<ZH_CHILD_TEXT_MAX;++i) {
        ZhChildTextEntry *e=&zh_child_text_entries[i];
        ZhChildTextHeader header;ZhChildTextFrameConsole *fc=NULL;int resolved_x,resolved_y;
        if (!e->active) continue;
        /* source_object is caller-owned and may be destroyed after this hook.
           The bytes were copied at registration; the console dtor bounds life. */
        if (!ZhChildTextHeaderRead(e->self,&header)) {ZhChildTextLog(e->source,e->length,"draw:header-read",e->self,e->local_x,e->local_y,0,0,0,0,0,0,0);e->active=0;continue;}
        if (!ZhChildTextHeaderEqual(&header,&e->header)) {ZhChildTextLog(e->source,e->length,"draw:header-changed",e->self,e->local_x,e->local_y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);e->active=0;continue;}
        if(!ZhChildTextResolveRootPosition(e->self,&header,&resolved_x,&resolved_y)||
           resolved_x+e->local_x!=e->root_x||resolved_y+e->local_y!=e->root_y){ZhChildTextLog(e->source,e->length,"draw:parent-chain",e->self,e->local_x,e->local_y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);e->active=0;continue;}
        for (j=0;j<ncon;++j) if (consoles[j].self==e->self) {fc=&consoles[j];break;}
        if (!fc) {
            if (ncon>=ZH_CHILD_TEXT_MAX_CONSOLES) {e->active=0;continue;}
            fc=&consoles[ncon++];fc->self=e->self;fc->header=header;fc->loaded=1;
            if (!ZhChildTextSnapshot(&header,&fc->cells)) fc->invalid=1;
        }
        if (fc->invalid || !fc->cells) {ZhChildTextLog(e->source,e->length,"draw:cells-read",e->self,e->local_x,e->local_y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);e->active=0;continue;}
        for (j=0;j<(int)e->length;++j) {
            ZhChildTextCell *c=&fc->cells[(SIZE_T)(e->local_x+j)*(SIZE_T)e->header.h+(SIZE_T)e->local_y];
            char ch=(c->glyph>=32 && c->glyph<=126)?(char)c->glyph:0;
            if (!ch || !ZhChildTextFoldChar(ch,e->source[j])) break;
        }
        if (j!=(int)e->length) {ZhChildTextLog(e->source,e->length,"draw:glyph-mismatch",e->self,e->local_x,e->local_y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,j);e->active=0;continue;}
        ++e->draw_count;
        {int root_state=ZhChildTextRootDuplicate(e,fc->cells);
         if(root_state==0){ZhChildTextLog(e->source,e->length,"draw:root-absent",e->self,e->local_x,e->local_y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);e->active=0;continue;}
         if(root_state==1){ZhChildTextLogRootDuplicate(e,fc->cells,e->dark_baseline?"draw:root-settled":"draw:root-duplicate");e->active=0;continue;}
         if(root_state==2&&e->draw_count<=3)ZhChildTextLogRootDuplicate(e,fc->cells,"draw:root-pending");}
        ZhChildTextDrawEntry(surface,e,fc->cells);
        ZhChildTextLog(e->source,e->length,"draw:ok",e->self,e->local_x,e->local_y,header.w,header.h,header.anchor_x,header.anchor_y,header.project_x,header.project_y,0);
    }
    for (i=0;i<ncon;++i) free(consoles[i].cells);
}

#endif
