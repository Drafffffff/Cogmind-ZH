/* Narrow map-label overlay candidate. Include after zh_child_text_draw.h.
   It never writes native grids; caller must invoke Remember after original
   0x418320/0x418370 and Forget before the child-console destructor. */
#ifndef ZH_MAP_LABEL_DRAW_H
#define ZH_MAP_LABEL_DRAW_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define ZH_MAP_LABEL_MAX 64
#define ZH_MAP_LABEL_MAX_SOURCE 96
#define ZH_MAP_LABEL_MAX_TARGET 256

typedef struct {
    int active, conflict, root_owned, complete;
    void *self;
    ZhChildTextHeader header;
    int local_x,local_y,root_x,root_y;
    unsigned length,target_cells;
    unsigned long stamp;
    char source[ZH_MAP_LABEL_MAX_SOURCE+1];
    char target[ZH_MAP_LABEL_MAX_TARGET];
    int cap_count,cap_source_ofs[MAX_TOK],cap_source_len[MAX_TOK];
    int cap_target_begin[MAX_TOK],cap_target_end[MAX_TOK];
} ZhMapLabelEntry;

static ZhMapLabelEntry zh_map_label_entries[ZH_MAP_LABEL_MAX];
static unsigned long zh_map_label_clock;
static const char *zh_map_label_noise[]={
    "ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ",
    "ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ",
    "ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"
};

static int ZhMapLabelVisible(const ZhChildTextCell *c)
{
    return abs((int)c->fgR-(int)c->bgR)+abs((int)c->fgG-(int)c->bgG)+
           abs((int)c->fgB-(int)c->bgB)>3;
}

static int ZhMapLabelHud(void)
{
    int x;
    if(!g_on||!g_grid.cells||g_grid.w!=160||g_grid.h!=45||g_cellW<=0||g_cellH!=2*g_cellW)return 0;
    for(x=0;x+6<g_grid.w;++x)
        if(ChAt(x,0)=='/'&&ChAt(x+2,0)=='L'&&ChAt(x+4,0)=='O'&&ChAt(x+6,0)=='G')return 1;
    return 0;
}

/* The accepted chain is intentionally the one live-observed map layer only:
   child(scale 0)->50x35 map(scale 1)->1x1 bridge(scale 0)->g_console. */
static int ZhMapLabelResolve(void *self,const ZhChildTextHeader *initial,int *rx,int *ry)
{
    ZhChildTextHeader map,bridge,root;
    long long ex,ey;
    if(!self||!initial||!rx||!ry||initial->h!=1||initial->w>ZH_MAP_LABEL_MAX_SOURCE||
       initial->scale_index!=0||!initial->parent)return 0;
    if(!ZhChildTextHeaderRead((void *)(ULONG_PTR)initial->parent,&map)||
       map.w!=50||map.h!=35||map.scale_index!=1||!map.parent)return 0;
    /* Native tables 0xb8df48/0xb8df58: index 1 is x2,y1. */
    ex=(long long)map.project_x+2LL*initial->anchor_x;
    ey=(long long)map.project_y+initial->anchor_y;
    if(ex<INT_MIN||ex>INT_MAX||ey<INT_MIN||ey>INT_MAX||
       initial->project_x!=(int)ex||initial->project_y!=(int)ey)return 0;
    if(!ZhChildTextHeaderRead((void *)(ULONG_PTR)map.parent,&bridge)||
       bridge.w!=1||bridge.h!=1||bridge.scale_index!=0||!bridge.parent)return 0;
    ex=(long long)bridge.project_x+map.anchor_x;
    ey=(long long)bridge.project_y+map.anchor_y;
    if(ex<INT_MIN||ex>INT_MAX||ey<INT_MIN||ey>INT_MAX||
       map.project_x!=(int)ex||map.project_y!=(int)ey)return 0;
    if(!ZhChildTextHeaderRead((void *)(ULONG_PTR)bridge.parent,&root)||
       (void *)(ULONG_PTR)bridge.parent!=g_console||root.w!=160||root.h!=45||
       root.scale_index!=0||root.parent!=0||root.project_x!=0||root.project_y!=0)return 0;
    ex=(long long)root.project_x+bridge.anchor_x;
    ey=(long long)root.project_y+bridge.anchor_y;
    if(ex<INT_MIN||ex>INT_MAX||ey<INT_MIN||ey>INT_MAX||
       bridge.project_x!=(int)ex||bridge.project_y!=(int)ey)return 0;
    *rx=initial->project_x;*ry=initial->project_y;
    /* Observed map panel occupies root x=[0,99], y=[10,44]. */
    return *rx>=0&&*ry>=10&&*rx<100&&*ry<45;
}

static int ZhMapLabelCaptureDigits(const char *p,int n)
{
    int i=0;
    if(n>=3&&p[0]=='['&&p[n-1]==']'){i=1;--n;}
    if(i==n)return 0;
    for(;i<n;++i)if(p[i]<'0'||p[i]>'9')return 0;
    return 1;
}

static int ZhMapLabelFindCaptureTarget(const char *out,int outlen,const char *cap,int caplen,
                                       int *begin,int *end)
{
    int i,count=0,at=-1;
    if(caplen<=0||caplen>outlen)return 0;
    for(i=0;i+caplen<=outlen;++i)if(!memcmp(out+i,cap,(size_t)caplen)){++count;at=i;}
    if(count!=1)return 0;
    *begin=at;*end=at+caplen;
    if(at>0&&out[at-1]=='['&&*end<outlen&&out[*end]==']'){--*begin;++*end;}
    return 1;
}

static Rule *ZhMapLabelFindRule(const char *source,int length,Caps *bestcp,
                                char *target,int target_cap,int *capn,
                                int *capso,int *capsl,int *capdb,int *capde)
{
    int first,hi,k,matches=0,lit[MAX_TOK],pass;Rule *best=NULL;Caps cp;
    if(!source||length<=0||!g_tr||!g_trArr||!g_trOfs||g_ntr<=0)return NULL;
    first=(unsigned char)source[0];*capn=0;
    for(pass=0;pass<2;++pass){
    int bucket=pass?256:first,lo=g_trOfs[bucket],end=g_trOfs[bucket+1];
    hi=end;
    for(k=BkFit(g_tr,g_trArr,g_trOfs[bucket],hi,length);k<hi;++k){
        Rule *r=&g_tr[g_trArr[k]];char tmp[ZH_MAP_LABEL_MAX_TARGET];int i,c=0,valid=1;
        int cs[MAX_TOK]={0},cl[MAX_TOK]={0},cb[MAX_TOK]={0},ce[MAX_TOK]={0};
        if(r->ns<=0||r->nd<=0||!cm_match(r,source,length,&cp,lit))continue;
        for(i=0;i<r->ns;++i)if(r->s[i].cap){
            if(cp.ofs[i]<0||cp.len[i]<=0||cp.ofs[i]+cp.len[i]>length||
               c>=MAX_TOK||!ZhMapLabelCaptureDigits(source+cp.ofs[i],cp.len[i])){valid=0;break;}
            cs[c]=cp.ofs[i];cl[c]=cp.len[i];++c;
        }
        if(!valid||r->ncap!=c)continue;
        cm_buildfree(r,source,&cp,tmp,sizeof tmp);
        if(!tmp[0]||strlen(tmp)>=sizeof tmp||!ZhChildTextHasCjk(tmp)||
           cm_cells(tmp)>length||memchr(tmp,CM_BR,strlen(tmp))||memchr(tmp,CM_SHL,strlen(tmp)))continue;
        for(i=0;i<c;++i)if(!ZhMapLabelFindCaptureTarget(tmp,(int)strlen(tmp),
              source+cs[i],cl[i],&cb[i],&ce[i])){valid=0;break;}
        if(!valid)continue;
        if(++matches>1)return NULL;
        best=r;*bestcp=cp;*capn=c;memcpy(target,tmp,strlen(tmp)+1);
        if(c){int j;for(j=0;j<c;++j){capso[j]=cs[j];capsl[j]=cl[j];capdb[j]=cb[j];capde[j]=ce[j];}}
    }
    }
    if(matches!=1)return NULL;
    return best;
}

static void ZhForgetMapLabels(void *self)
{
    int i;if(!self)return;for(i=0;i<ZH_MAP_LABEL_MAX;++i)
        if(zh_map_label_entries[i].active&&zh_map_label_entries[i].self==self)
            zh_map_label_entries[i].active=0;
}

/* Completion is deliberately an explicit external input. New entries default
   to incomplete; no color uniformity or elapsed time is treated as completion. */
static int ZhSetMapLabelComplete(void *self,const char *source,unsigned length,int complete)
{
    int i;if(!self||!source||!length)return 0;
    for(i=0;i<ZH_MAP_LABEL_MAX;++i){ZhMapLabelEntry*e=&zh_map_label_entries[i];
        if(e->active&&e->self==self&&e->length==length&&!memcmp(e->source,source,length)){e->complete=!!complete;return 1;}}
    return 0;
}

static int ZhRememberMapLabelText(void *self,int x,int y,void *source_object)
{
    char source[ZH_CHILD_TEXT_MAX_SOURCE+1],target[ZH_MAP_LABEL_MAX_TARGET];unsigned n,lead=0,tail,i;
    ZhChildTextHeader h;ZhChildTextCell *cells=NULL;Caps cp;Rule *rule;int rx,ry,slot=-1;
    int capn=0,capso[MAX_TOK]={0},capsl[MAX_TOK]={0},capdb[MAX_TOK]={0},capde[MAX_TOK]={0};
    ZhMapLabelEntry *e;
    if(!ZhMapLabelHud()||!self||self==g_console||!source_object||
       !ZhChildTextReadString(source_object,source,&n))return 0;
    while(lead<n&&source[lead]==' ')++lead;tail=n;while(tail>lead&&source[tail-1]==' ')--tail;
    if(tail<=lead)return 0;if(lead){memmove(source,source+lead,(size_t)(tail-lead));x+=(int)lead;}
    n=tail-lead;source[n]=0;
    if(n>ZH_MAP_LABEL_MAX_SOURCE||!ZhChildTextHeaderRead(self,&h)||h.h!=1||
       x<0||y!=0||x+(int)n>h.w)return 0;
    if(!ZhMapLabelResolve(self,&h,&rx,&ry)||rx+x<0||rx+x+(int)n>100||ry<10||ry>=45||rx+x+(int)n>g_grid.w||ry>=g_grid.h)return 0;
    rule=ZhMapLabelFindRule(source,(int)n,&cp,target,sizeof target,&capn,capso,capsl,capdb,capde);
    if(!rule)return 0;
    if(!ZhChildTextSnapshot(&h,&cells))return 0;
    for(i=0;i<n;++i){char ch=(char)cells[(SIZE_T)(x+(int)i)*(SIZE_T)h.h+(SIZE_T)y].glyph;
        if(!ZhChildTextFoldChar(ch,source[i])){free(cells);return 0;}}
    free(cells);
    for(i=0;i<ZH_MAP_LABEL_MAX;++i){ZhMapLabelEntry*c=&zh_map_label_entries[i];
        if(c->active&&c->self==self&&c->local_x==x&&c->local_y==y){slot=(int)i;break;}
        if(!c->active&&slot<0)slot=(int)i;
    }
    if(slot<0){unsigned long least=~0UL;for(i=0;i<ZH_MAP_LABEL_MAX;++i)if(zh_map_label_entries[i].stamp<least){least=zh_map_label_entries[i].stamp;slot=(int)i;}}
    e=&zh_map_label_entries[slot];memset(e,0,sizeof *e);e->active=1;e->self=self;e->header=h;
    e->local_x=x;e->local_y=y;e->root_x=rx+x;e->root_y=ry+y;e->length=n;
    e->target_cells=(unsigned)cm_cells(target);e->stamp=++zh_map_label_clock;
    memcpy(e->source,source,n+1);memcpy(e->target,target,strlen(target)+1);
    e->cap_count=capn;for(i=0;i<(unsigned)capn;++i){e->cap_source_ofs[i]=capso[i];e->cap_source_len[i]=capsl[i];e->cap_target_begin[i]=capdb[i];e->cap_target_end[i]=capde[i];}
    return 1;
}

static void ZhMapLabelUpdateConflicts(void)
{
    int i,j;for(i=0;i<ZH_MAP_LABEL_MAX;++i)zh_map_label_entries[i].conflict=0;
    for(i=0;i<ZH_MAP_LABEL_MAX;++i){ZhMapLabelEntry*a=&zh_map_label_entries[i];if(!a->active||!a->root_owned)continue;
        for(j=i+1;j<ZH_MAP_LABEL_MAX;++j){ZhMapLabelEntry*b=&zh_map_label_entries[j];if(!b->active||!b->root_owned||a->root_y!=b->root_y)continue;
            if(a->root_x<b->root_x+(int)b->length&&b->root_x<a->root_x+(int)a->length)a->conflict=b->conflict=1;
        }
    }
}

static int ZhMapLabelCaptureReady(const ZhMapLabelEntry *e,const ZhChildTextCell *cells,int cap)
{
    int j,k;for(k=0;k<e->cap_source_len[cap];++k){
        const ZhChildTextCell*c=&cells[(SIZE_T)(e->local_x+e->cap_source_ofs[cap]+k)*(SIZE_T)e->header.h+e->local_y];
        if(!ZhMapLabelVisible(c))return 0;
    }(void)j;return 1;
}

static void DrawZhMapLabels(SSurf *surface)
{
    int i;
    if(!surface||!ZhMapLabelHud()||surface->w<g_grid.w*g_cellW||surface->h<g_grid.h*g_cellH)return;
    /* Validate child identity first. Root absence/color lag is a pending frame,
       not proof that the child source or cached identity has expired. */
    for(i=0;i<ZH_MAP_LABEL_MAX;++i){
        ZhMapLabelEntry*e=&zh_map_label_entries[i];ZhChildTextHeader h;ZhChildTextCell*cells=NULL;
        int rx,ry,j,valid=1,owner=1;
        if(!e->active)continue;e->root_owned=0;
        if(!ZhChildTextHeaderRead(e->self,&h)||!ZhChildTextHeaderEqual(&h,&e->header)||
           !ZhMapLabelResolve(e->self,&h,&rx,&ry)||rx+e->local_x!=e->root_x||
           ry+e->local_y!=e->root_y||!ZhChildTextSnapshot(&h,&cells)){e->active=0;continue;}
        for(j=0;j<(int)e->length;++j){
            const ZhChildTextCell*c=&cells[(SIZE_T)(e->local_x+j)*(SIZE_T)h.h+e->local_y];
            const XCell*r=&CELL(e->root_x+j,e->root_y);
            if(!ZhChildTextFoldChar((char)c->glyph,e->source[j])){valid=0;break;}
            if(!ZhChildTextFoldChar((char)r->glyph,e->source[j])||r->fgR!=c->fgR||r->fgG!=c->fgG||
               r->fgB!=c->fgB||r->bgR!=c->bgR||r->bgG!=c->bgG||r->bgB!=c->bgB)owner=0;
        }
        free(cells);
        if(!valid){e->active=0;continue;}
        e->root_owned=owner;
    }
    ZhMapLabelUpdateConflicts();
    for(i=0;i<ZH_MAP_LABEL_MAX;++i){
        ZhMapLabelEntry*e=&zh_map_label_entries[i];ZhChildTextHeader h;ZhChildTextCell*cells=NULL;
        int rx,ry,j,ink_idx[ZH_MAP_LABEL_MAX_SOURCE],ink=0;
        const char*p;int off=0;
        if(!e->active||!e->root_owned||e->conflict)continue;
        if(!ZhChildTextHeaderRead(e->self,&h)||!ZhChildTextHeaderEqual(&h,&e->header)||
           !ZhMapLabelResolve(e->self,&h,&rx,&ry)||rx+e->local_x!=e->root_x||
           ry+e->local_y!=e->root_y||!ZhChildTextSnapshot(&h,&cells)){e->active=0;continue;}
        for(j=0;j<(int)e->length;++j){
            ZhChildTextCell*c=&cells[(SIZE_T)(e->local_x+j)*(SIZE_T)h.h+e->local_y];
            const XCell*r=&CELL(e->root_x+j,e->root_y);
            if(!ZhChildTextFoldChar((char)c->glyph,e->source[j]))break;
            if(!ZhChildTextFoldChar((char)r->glyph,e->source[j])||r->fgR!=c->fgR||r->fgG!=c->fgG||
               r->fgB!=c->fgB||r->bgR!=c->bgR||r->bgG!=c->bgG||r->bgB!=c->bgB){free(cells);cells=NULL;break;}
            if(e->source[j]!=' ')ink_idx[ink++]=j;
        }
        if(!cells)continue; /* ownership/color changed after prepass: skip only */
        if(j!=(int)e->length||!ink){free(cells);e->active=0;continue;}
        for(j=0;j<(int)e->length;++j){ZhChildTextCell*c=&cells[(SIZE_T)(e->local_x+j)*(SIZE_T)h.h+e->local_y];BYTE bgc[3]={c->bgR,c->bgG,c->bgB};
            DrawSpan(surface,e->root_x+j,e->root_y,1,1,0,bgc,"",c->fgR,c->fgG,c->fgB);}
        p=e->target;
        while(*p){
            unsigned cpv;const char*next=cm_u8next(p,&cpv);int width=cm_wide(cpv)?2:1;
            int byteoff=(int)(p-e->target),sample=(off+width/2)*ink/(int)e->target_cells,si,show;
            size_t nb=(size_t)(next-p);const ZhChildTextCell*c;char glyph[8];
            if(sample>=ink)sample=ink-1;si=ink_idx[sample];c=&cells[(SIZE_T)(e->local_x+si)*(SIZE_T)h.h+e->local_y];
            show=e->complete||ZhMapLabelVisible(c);
            for(j=0;j<e->cap_count;++j)
                if(byteoff<e->cap_target_end[j]&&(int)(next-e->target)>e->cap_target_begin[j]&&
                   !ZhMapLabelCaptureReady(e,cells,j))show=0;
            if(show&&off+width<=(int)e->target_cells&&nb<sizeof glyph){
                BYTE bgc[3]={c->bgR,c->bgG,c->bgB};memcpy(glyph,p,nb);glyph[nb]=0;
                if(!e->complete&&width==2){unsigned seed=(unsigned)(e->root_x*7+e->root_y*13+off+c->fgR+c->fgG*3+c->fgB*5);
                    DrawSpan(surface,e->root_x+off,e->root_y,width,1,0,bgc,zh_map_label_noise[seed%(sizeof zh_map_label_noise/sizeof zh_map_label_noise[0])],c->fgR,c->fgG,c->fgB);}
                else DrawSpan(surface,e->root_x+off,e->root_y,width,1,0,bgc,glyph,c->fgR,c->fgG,c->fgB);
            }
            off+=width;p=next;
        }
        free(cells);
    }
}

#endif
