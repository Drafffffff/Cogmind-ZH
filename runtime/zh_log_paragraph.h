/* Reconstruct and translate complete static LOG paragraphs in the visible
   root-grid window. Include after Run/AddOpF definitions. */
#ifndef ZH_LOG_PARAGRAPH_H
#define ZH_LOG_PARAGRAPH_H
#include <stdlib.h>
#include <string.h>

#define ZH_LOG_X0 1
#define ZH_LOG_X1 49
#define ZH_LOG_Y0 1
#define ZH_LOG_Y_LIMIT 45
#define ZH_LOG_LINES 24
#define ZH_LOG_SOURCE_MAX 1024
#define ZH_LOG_LINE_BYTES 160
#define ZH_LOG_REF_CAP 150000

typedef struct { unsigned hash; int rule_index; unsigned char line_index; unsigned char len; } ZhLogLineRef;
static ZhLogLineRef zh_log_refs[ZH_LOG_REF_CAP];
static unsigned zh_log_ref_count;
static Rule *zh_log_index_table;
static int zh_log_index_count;
static int zh_log_index_overflow;
static unsigned char zh_log_owned_rows[45];
#define ZH_LOG_CACHE_SIZE 128
typedef struct {
    int rows;
    char source[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];
    char target[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];
    int widths[ZH_LOG_LINES];
} ZhLogObserved;
static ZhLogObserved zh_log_observed[ZH_LOG_CACHE_SIZE];
static unsigned zh_log_observed_next;
static void ZhLogRemember(char source[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],
                          char target[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],
                          const int *widths,int rows)
{
    unsigned slot;int i;
    /* Repainting the same visible history must not evict other paragraphs.
       Color fades and scroll animation can replan this window many times. */
    for(slot=0;slot<ZH_LOG_CACHE_SIZE;++slot){
        ZhLogObserved *entry=&zh_log_observed[slot];
        if(entry->rows!=rows)continue;
        for(i=0;i<rows;++i)if(strcmp(entry->source[i],source[i])||
            strcmp(entry->target[i],target[i]))break;
        if(i==rows)return;
    }
    {ZhLogObserved *entry=&zh_log_observed[zh_log_observed_next++%ZH_LOG_CACHE_SIZE];
        entry->rows=rows;
        for(i=0;i<rows;++i){strcpy(entry->source[i],source[i]);
            strcpy(entry->target[i],target[i]);entry->widths[i]=widths[i];}
    }
}
static void ZhLogInvalidateIndex(void)
{
    zh_log_index_table=NULL;zh_log_index_count=-1;
    zh_log_ref_count=0;zh_log_index_overflow=0;
    memset(zh_log_observed,0,sizeof zh_log_observed);zh_log_observed_next=0;
}
static int ZhLogParagraphOwnsRow(int y)
{
    return y>=0&&y<45&&zh_log_owned_rows[y]!=0;
}
static int ZhLogBottomRow(void)
{
    int y;
    if(CELL(0,0).glyph!=136||CELL(49,0).glyph!=137)return -1;
    for(y=1;y<g_grid.h;++y){
        unsigned left=CELL(0,y).glyph,right=CELL(49,y).glyph;
        if(left==135&&right==138)return y;
        if(left!=128||right!=128)return -1;
    }
    return -1;
}

static unsigned ZhLogLineHash(const char *s,int n)
{
    unsigned h=2166136261u;int i;
    for(i=0;i<n;++i){h^=(unsigned char)s[i];h*=16777619u;}
    h^=(unsigned)n;h*=16777619u;return h;
}
static int ZhLogRefCompare(const void *a,const void *b)
{
    const ZhLogLineRef *x=(const ZhLogLineRef*)a,*y=(const ZhLogLineRef*)b;
    return x->hash<y->hash?-1:x->hash>y->hash?1:
           x->len<y->len?-1:x->len>y->len?1:0;
}
static int ZhLogHasHeader(void)
{
    static const char marker[]="/ L O G /";int x,i,n=(int)sizeof(marker)-1;
    if(g_grid.w!=160||g_grid.h!=45)return 0;
    for(x=0;x+n<=49;++x){
        for(i=0;i<n;++i)if(ChAt(x+i,0)!=marker[i])break;
        if(i==n)return 1;
    }
    return 0;
}
static int ZhLogStaticRule(const Rule *r)
{
    int i;
    if(!r||r->ns!=1||r->nd!=1||r->ncap||r->s[0].cap||r->d[0].cap||
       r->s[0].len<=0||r->s[0].len>=ZH_LOG_SOURCE_MAX||r->d[0].len<=0||
       r->d[0].len>=ZH_LOG_SOURCE_MAX)return 0;
    for(i=0;i<r->s[0].len;++i){unsigned char c=(unsigned char)r->s[0].lit[i];if(c<32||c>126)return 0;}
    for(i=0;i<r->d[0].len;++i)if((unsigned char)r->d[0].lit[i]==CM_BR||
                                      (unsigned char)r->d[0].lit[i]==CM_SHL)return 0;
    return 1;
}
static int ZhLogSourceLayout(const Rule *r,char lines[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],int *nlines)
{
    char source[ZH_LOG_SOURCE_MAX];const char *p;int n=0,cap;
    if(!ZhLogStaticRule(r))return 0;
    memcpy(source,r->s[0].lit,(size_t)r->s[0].len);source[r->s[0].len]=0;p=source;
    while(*p){const char *next;cap=n?46:48;if(n>=ZH_LOG_LINES)return 0;
        next=cm_wrap(p,cap,lines[n],ZH_LOG_LINE_BYTES);
        if(next<=p||!lines[n][0]||cm_cells(lines[n])>cap)return 0;
        ++n;p=next;
    }
    *nlines=n;return n>0;
}
static void ZhLogBuildIndex(void)
{
    int k;
    if(zh_log_index_table==g_pa&&zh_log_index_count==g_npa)return;
    memset(zh_log_observed,0,sizeof zh_log_observed);zh_log_observed_next=0;
    zh_log_index_table=g_pa;zh_log_index_count=g_npa;
    zh_log_ref_count=0;zh_log_index_overflow=0;
    if(!g_pa||g_npa<=0)return;
    for(k=0;k<g_npa;++k){char lines[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];int nl=0,j;
        if(!ZhLogSourceLayout(&g_pa[k],lines,&nl))continue;
        for(j=0;j<nl;++j){ZhLogLineRef *ref;
            int n=(int)strlen(lines[j]);if(n<=0||n>255)continue;
            if(zh_log_ref_count>=ZH_LOG_REF_CAP){zh_log_index_overflow=1;break;}
            ref=&zh_log_refs[zh_log_ref_count++];
            ref->hash=ZhLogLineHash(lines[j],n);ref->rule_index=k;
            ref->line_index=(unsigned char)j;ref->len=(unsigned char)n;
        }
        if(zh_log_index_overflow)break;
    }
    qsort(zh_log_refs,zh_log_ref_count,sizeof zh_log_refs[0],ZhLogRefCompare);
}
static int ZhLogFindRefs(unsigned hash,int len)
{
    unsigned lo=0,hi=zh_log_ref_count;
    while(lo<hi){unsigned m=lo+(hi-lo)/2;ZhLogLineRef *r=&zh_log_refs[m];
        if(r->hash<hash||(r->hash==hash&&r->len<len))lo=m+1;else hi=m;}
    return (int)lo;
}
static int ZhLogReadLine(int y,int bottom,char *text,int *x0,int *span,BYTE *r,BYTE *g,BYTE *b)
{
    int x,start,cap,end=-1;
    if(y<ZH_LOG_Y0||y>=bottom)return 0;
    if(ChAt(ZH_LOG_X0,y)!=' ')start=ZH_LOG_X0;
    else if(ChAt(2,y)==' '&&ChAt(3,y)!=' ')start=3;
    else return 0;
    cap=start==ZH_LOG_X0?48:46;
    *r=CELL(start,y).fgR;*g=CELL(start,y).fgG;*b=CELL(start,y).fgB;
    for(x=0;x<cap;++x){int col=start+x;char c=ChAt(col,y);const XCell *cell=&CELL(col,y);
        int contrast=abs((int)cell->fgR-cell->bgR)+abs((int)cell->fgG-cell->bgG)+abs((int)cell->fgB-cell->bgB);
        if(c<' '||c>'~')return 0;
        /* Spaces are layout, not visible ink; renderers often give them a
           different foreground. Keep their positions but validate only ink. */
        if(c!=' '&&(contrast<=3||cell->fgR!=*r||cell->fgG!=*g||cell->fgB!=*b))return 0;
        text[x]=c;if(c!=' ')end=x;
    }
    if(end<0)return 0;text[end+1]=0;*x0=start;*span=cap;
    return 1;
}
static int ZhLogTargetLayout(const char *text,int wanted,char lines[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],int *widths)
{
    int cap,i,best=0x7fffffff,found=0;
    for(cap=48;cap>=1;--cap){const char *p=text;char tmp[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];int w[ZH_LOG_LINES],count=0,valid=1;
        while(*p){const char *next;if(count>=ZH_LOG_LINES){valid=0;break;}
            next=cm_wrap(p,cap,tmp[count],sizeof tmp[0]);
            if(next<=p||!tmp[count][0]){valid=0;break;}
            w[count]=cm_cells(tmp[count]);if(w[count]>48){valid=0;break;}
            ++count;p=next;
        }
        if(valid&&count==wanted){int low=48,high=0,score;
            for(i=0;i<count;++i){if(w[i]<low)low=w[i];if(w[i]>high)high=w[i];}
            /* Keep one stable row per native scroll unit, but balance the
               translated rows rather than stranding punctuation on its own. */
            score=(high-low)*100+48-cap;
            if(score<best){best=score;found=1;for(i=0;i<count;++i){strcpy(lines[i],tmp[i]);widths[i]=w[i];}}
        }
    }
    return found;
}

/* A dynamic paragraph is cached only after a complete, visible native
   message has matched. Later clips use those concrete source lines; hidden
   captures are never reconstructed from a wildcard or suffix. */
static int ZhLogObserveFull(char text[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],int *x0,
                            BYTE *r,BYTE *g,BYTE *b,int rows)
{
    char source[ZH_LOG_SOURCE_MAX],out[ZH_LOG_SOURCE_MAX];
    char target[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];int widths[ZH_LOG_LINES];
    int i,n=0,pass;
    if(rows<1||x0[0]!=1)return 0;
    for(i=0;i<rows;++i){int len=(int)strlen(text[i]);
        if(i&&(x0[i]!=3||r[i]!=r[0]||g[i]!=g[0]||b[i]!=b[0]))return 0;
        if(n+len+2>=sizeof source)return 0;
        if(i)source[n++]=' ';memcpy(source+n,text[i],(size_t)len);n+=len;
    }
    source[n]=0;
    for(i=0;i<ZH_LOG_CACHE_SIZE;++i){ZhLogObserved *entry=&zh_log_observed[i];
        int j;if(entry->rows!=rows)continue;
        for(j=0;j<rows&&strcmp(entry->source[j],text[j])==0;++j){}
        if(j==rows)return 1;
    }
    for(pass=0;pass<2;++pass){int bucket=pass?256:(unsigned char)source[0];
        int end=g_paOfs[bucket+1],k;
        for(k=BkFit(g_pa,g_paArr,g_paOfs[bucket],end,n);k<end;++k){
            Rule *rule=&g_pa[g_paArr[k]];Caps caps;int literals[MAX_TOK];
            if(!rule->ncap||rule->minLen<8||(n>100&&rule->minLen*2<n))continue;
            /* An open-ended wildcard cannot establish that a bottom-clipped
               paragraph has reached its end. */
            if(rule->ns<=0||rule->s[rule->ns-1].cap)continue;
            if(!cm_match(rule,source,n,&caps,literals)||
               cm_capover(rule,&caps,64)||cm_capsent(rule,source,&caps))continue;
            cm_buildfree(rule,source,&caps,out,sizeof out);
            if(!out[0]||strcmp(out,source)==0||strchr(out,CM_BR)||strchr(out,CM_SHL)||
               !ZhLogTargetLayout(out,rows,target,widths))continue;
            ZhLogRemember(text,target,widths,rows);
            return 1;
        }
    }
    return 0;
}
static int ZhLogMatchObserved(char text[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],int *x0,
                              BYTE *r,BYTE *g,BYTE *b,int rows,
                              char target[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],int *widths)
{
    int i,offset,chosen=-1,at=-1;
    for(i=0;i<ZH_LOG_CACHE_SIZE;++i){ZhLogObserved *entry=&zh_log_observed[i];
        for(offset=0;offset+rows<=entry->rows;++offset){int j;
            if(x0[0]!=(offset?3:1))continue;
            for(j=0;j<rows;++j)if(strcmp(text[j],entry->source[offset+j])||
                (j&&x0[j]!=3)||r[j]!=r[0]||g[j]!=g[0]||b[j]!=b[0])break;
            if(j!=rows)continue;
            if(chosen>=0){for(j=0;j<rows;++j)
                if(strcmp(zh_log_observed[chosen].target[at+j],entry->target[offset+j]))return 0;
            }else{chosen=i;at=offset;}
        }
    }
    if(chosen<0)return 0;
    for(i=0;i<rows;++i){strcpy(target[i],zh_log_observed[chosen].target[at+i]);widths[i]=zh_log_observed[chosen].widths[at+i];}
    return 1;
}
static int ZhLogMatchWindow(char text[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],int *x0,int *span,
                            BYTE *r,BYTE *g,BYTE *b,int rows,int *rule_index,int *first_line,
                            char target[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],int *target_width)
{
    unsigned hash;int n,ref;Rule *chosen=NULL;int chosen_line=-1,chosen_ns=0,ambiguous=0;
    char sourcelines[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];int ns;
    hash=ZhLogLineHash(text[0],(int)strlen(text[0]));ref=ZhLogFindRefs(hash,(int)strlen(text[0]));
    while((unsigned)ref<zh_log_ref_count&&zh_log_refs[ref].hash==hash&&
          zh_log_refs[ref].len==strlen(text[0])){
        ZhLogLineRef *entry=&zh_log_refs[ref++];Rule *candidate=&g_pa[entry->rule_index];int j,offset=entry->line_index;
        if(offset+rows>ZH_LOG_LINES||!ZhLogSourceLayout(candidate,sourcelines,&ns)||offset+rows>ns)continue;
        if((offset==0&&x0[0]!=1)||(offset>0&&x0[0]!=3))continue;
        for(j=0;j<rows;++j){if(strcmp(text[j],sourcelines[offset+j])!=0)break;
            if(j>0&&x0[j]!=3)break;
            if(r[j]!=r[0]||g[j]!=g[0]||b[j]!=b[0])break;
        }
        if(j!=rows)continue;
        if(chosen&&(chosen!=candidate||chosen_line!=offset)){ambiguous=1;break;}
        chosen=candidate;chosen_line=offset;chosen_ns=ns;
    }
    if(!chosen||ambiguous)return 0;
    {char out[ZH_LOG_SOURCE_MAX];char target_lines[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];int widths[ZH_LOG_LINES],i;
        Caps caps;int lit[MAX_TOK];
        if(!cm_match(chosen,chosen->s[0].lit,chosen->s[0].len,&caps,lit))return 0;
        cm_buildfree(chosen,chosen->s[0].lit,&caps,out,sizeof out);
        if(!out[0]||strchr(out,CM_BR)||strchr(out,CM_SHL)||
           !ZhLogTargetLayout(out,chosen_ns,target_lines,widths))return 0;
        for(i=0;i<rows;++i){int at=chosen_line+i;
            if(widths[at]>48)return 0;
            strcpy(target[i],target_lines[at]);target_width[i]=widths[at];
        }
        if(chosen_line==0&&rows==chosen_ns){
            ZhLogRemember(text,target_lines,widths,rows);
        }
    }
    *rule_index=(int)(chosen-g_pa);*first_line=chosen_line;return 1;
}
static void ZhLogMarkRunsUsed(int row,int x0,int span)
{
    int i,end=x0+span;
    for(i=0;i<g_nrun;++i){Run *r=&g_runs[i];int re=r->x0+r->len;
        if(r->row==row&&r->len>0&&r->x0<end&&re>x0)r->used=1;
    }
}
static void PlanZhLogParagraphs(void)
{
    char text[ZH_LOG_LINES][ZH_LOG_LINE_BYTES],target[ZH_LOG_LINES][ZH_LOG_LINE_BYTES];
    int x0[ZH_LOG_LINES],span[ZH_LOG_LINES],tw[ZH_LOG_LINES],y,bottom,used[ZH_LOG_Y_LIMIT]={0};
    BYTE rr[ZH_LOG_LINES],gg[ZH_LOG_LINES],bb[ZH_LOG_LINES];
    memset(zh_log_owned_rows,0,sizeof zh_log_owned_rows);
    if(!g_on||!ZhLogHasHeader()||!g_pa||!g_paArr||g_npa<=0)return;
    bottom=ZhLogBottomRow();if(bottom<=ZH_LOG_Y0||bottom>g_grid.h-1)return;
    ZhLogBuildIndex();if(zh_log_index_overflow)return;
    for(y=ZH_LOG_Y0;y<bottom;++y){int i,max=bottom-y,matched=0,ri=-1,first=-1;
        Run synthetic;
        if(used[y-ZH_LOG_Y0]||!ZhLogReadLine(y,bottom,text[0],&x0[0],&span[0],&rr[0],&gg[0],&bb[0]))continue;
        if(max>ZH_LOG_LINES)max=ZH_LOG_LINES;
        for(i=1;i<max;++i){
            if(!ZhLogReadLine(y+i,bottom,text[i],&x0[i],&span[i],&rr[i],&gg[i],&bb[i]))break;
        }
        max=i;
        {int complete=1,probe=max;
            while(complete<max&&x0[complete]==3&&rr[complete]==rr[0]&&gg[complete]==gg[0]&&bb[complete]==bb[0])++complete;
            /* Exact static evidence takes precedence over a wildcard rule. */
            while(probe>0){if(ZhLogMatchWindow(text,x0,span,rr,gg,bb,probe,&ri,&first,target,tw)||
                ZhLogMatchObserved(text,x0,rr,gg,bb,probe,target,tw)){matched=probe;break;}--probe;}
            if(!matched&&ZhLogObserveFull(text,x0,rr,gg,bb,complete)&&
                ZhLogMatchObserved(text,x0,rr,gg,bb,complete,target,tw))matched=complete;
        }
        if(!matched)continue;
        if(g_nop+matched>MAX_RUN)continue;
        memset(&synthetic,0,sizeof synthetic);synthetic.f=1;synthetic.vs=1;
        for(i=0;i<matched;++i){int source_row=y+i;
            synthetic.row=source_row;synthetic.x0=x0[i];
            synthetic.fr=rr[i];synthetic.fg=gg[i];synthetic.fb=bb[i];
            AddOpF(&synthetic,1,48,target[i],rr[i],gg[i],bb[i]);
            ZhLogMarkRunsUsed(source_row,x0[i],span[i]);used[source_row-ZH_LOG_Y0]=1;
            if(source_row>=0&&source_row<45)zh_log_owned_rows[source_row]=1;
        }
    }
}
#endif
