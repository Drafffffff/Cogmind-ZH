/* Read-only, bounded diagnostics for non-root XConsole grids.
   Call from hk_blit before the original blit; this never edits game memory or
   draws. The caller supplies the XConsole `this` and destination pointers. */
#ifndef ZH_CHILD_CONSOLE_PROBE_H
#define ZH_CHILD_CONSOLE_PROBE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZH_CHILD_PROBE_LOG_LIMIT 256
#define ZH_CHILD_PROBE_STATE_LIMIT 256
#define ZH_CHILD_PROBE_MAX_W 400
#define ZH_CHILD_PROBE_MAX_H 200

typedef struct {
    void *self;
    unsigned last_scan_frame;
    int summary_logged;
} ZhChildProbeState;

typedef struct {
    void *self;
    int row, col;
    char keyword[8];
} ZhChildProbeHit;

static ZhChildProbeState zh_child_probe_states[ZH_CHILD_PROBE_STATE_LIMIT];
static unsigned zh_child_probe_state_count;
static ZhChildProbeHit zh_child_probe_hits[ZH_CHILD_PROBE_LOG_LIMIT];
static unsigned zh_child_probe_hit_count;
static unsigned zh_child_probe_log_count;

static int ZhChildProbeRead(const void *address, void *out, SIZE_T size)
{
    SIZE_T got=0;
    if (!address || !out || !size) return 0;
    return ReadProcessMemory(GetCurrentProcess(),address,out,size,&got) && got==size;
}

static ZhChildProbeState *ZhChildProbeStateFor(void *self)
{
    unsigned i;
    for (i=0;i<zh_child_probe_state_count;++i)
        if (zh_child_probe_states[i].self==self) return &zh_child_probe_states[i];
    if (zh_child_probe_state_count>=ZH_CHILD_PROBE_STATE_LIMIT) return NULL;
    zh_child_probe_states[zh_child_probe_state_count].self=self;
    zh_child_probe_states[zh_child_probe_state_count].last_scan_frame=(unsigned)-1;
    zh_child_probe_states[zh_child_probe_state_count].summary_logged=0;
    return &zh_child_probe_states[zh_child_probe_state_count++];
}

static int ZhChildProbeEqualAscii(unsigned char a,unsigned char b)
{
    if (a>='a' && a<='z') a=(unsigned char)(a-('a'-'A'));
    if (b>='a' && b<='z') b=(unsigned char)(b-('a'-'A'));
    return a==b;
}

static int ZhChildProbeFind(const char *row,int width,int col,const char *word)
{
    int j,n=(int)strlen(word);
    if (col+n>width) return 0;
    for (j=0;j<n;++j)
        if (!ZhChildProbeEqualAscii((unsigned char)row[col+j],(unsigned char)word[j])) return 0;
    return 1;
}

static int ZhChildProbeAlreadyHit(void *self,int row,int col,const char *keyword)
{
    unsigned i;
    for (i=0;i<zh_child_probe_hit_count;++i) {
        ZhChildProbeHit *h=&zh_child_probe_hits[i];
        if (h->self==self && h->row==row && h->col==col && !strcmp(h->keyword,keyword)) return 1;
    }
    return 0;
}

static void ZhChildProbeLogHit(void *self,void *dst,int w,int h,int row,int col,
                               const char *keyword,const char *line)
{
    int from=col-12,to=col+(int)strlen(keyword)+20,n,i;
    char excerpt[64];
    if (from<0) from=0;
    if (to>w) to=w;
    n=to-from;
    if (n>(int)sizeof excerpt-1) n=(int)sizeof excerpt-1;
    for (i=0;i<n;++i) {
        unsigned char c=(unsigned char)line[from+i];
        excerpt[i]=(c>=32 && c<=126)?(char)c:'?';
    }
    excerpt[n]=0;
    if (zh_child_probe_log_count>=ZH_CHILD_PROBE_LOG_LIMIT ||
        zh_child_probe_hit_count>=ZH_CHILD_PROBE_LOG_LIMIT) return;
    zh_child_probe_hits[zh_child_probe_hit_count].self=self;
    zh_child_probe_hits[zh_child_probe_hit_count].row=row;
    zh_child_probe_hits[zh_child_probe_hit_count].col=col;
    strncpy(zh_child_probe_hits[zh_child_probe_hit_count].keyword,keyword,
            sizeof zh_child_probe_hits[zh_child_probe_hit_count].keyword-1);
    zh_child_probe_hits[zh_child_probe_hit_count].keyword[
        sizeof zh_child_probe_hits[zh_child_probe_hit_count].keyword-1]=0;
    ++zh_child_probe_hit_count;
    ++zh_child_probe_log_count;
    L("[zh-child-hit] self=%p dst=%p grid=%dx%d row=%d col=%d keyword=%s raw=|%s|",
      self,dst,w,h,row,col,keyword,excerpt);
}

/* Bound each console to one grid scan per g_frame, and all emitted lines to
   256 for the whole process lifetime. Typical insertion is the first line of
   hk_blit: ZhObserveChildConsole(self,dst); then immediately call g_tramp. */
static void ZhObserveChildConsole(void *self,void *dst)
{
    BYTE object[0x40];
    char object_hex[0x40*2+1];
    int w,h,x,y;
    XCell *cells;
    SIZE_T bytes;
    XCell *snapshot;
    ZhChildProbeState *state;
    static const char *keywords[]={"click","expand","shrink","scroll"};
    unsigned i;

    if (!self || zh_child_probe_log_count>=ZH_CHILD_PROBE_LOG_LIMIT) return;
    state=ZhChildProbeStateFor(self);
    if (!state || state->last_scan_frame==(unsigned)g_frame) return;
    state->last_scan_frame=(unsigned)g_frame;
    if (!ZhChildProbeRead(self,object,sizeof object) ||
        !ZhChildProbeRead((const BYTE *)self+0x08,&w,sizeof w) ||
        !ZhChildProbeRead((const BYTE *)self+0x0c,&h,sizeof h) ||
        !ZhChildProbeRead((const BYTE *)self+0x10,&cells,sizeof cells)) return;
    if (w<=0 || h<=0 || w>ZH_CHILD_PROBE_MAX_W || h>ZH_CHILD_PROBE_MAX_H || !cells) return;
    if ((SIZE_T)w>(SIZE_T)-1/(SIZE_T)h/sizeof(XCell)) return;
    bytes=(SIZE_T)w*(SIZE_T)h*sizeof(XCell);

    if (!state->summary_logged && zh_child_probe_log_count<ZH_CHILD_PROBE_LOG_LIMIT) {
        static const char hex[]="0123456789ABCDEF";
        int k;
        for (k=0;k<(int)sizeof object;++k) {
            object_hex[k*2]=hex[object[k]>>4];
            object_hex[k*2+1]=hex[object[k]&15];
        }
        object_hex[sizeof object_hex-1]=0;
        state->summary_logged=1;
        ++zh_child_probe_log_count;
        L("[zh-child-console] self=%p dst=%p grid=%dx%d object0x00_3f=%s",
          self,dst,w,h,object_hex);
    }
    if (zh_child_probe_log_count>=ZH_CHILD_PROBE_LOG_LIMIT) return;
    snapshot=(XCell *)malloc(bytes);
    if (!snapshot) return;
    if (!ZhChildProbeRead(cells,snapshot,bytes)) { free(snapshot); return; }

    /* XGrid cells are column-major in the known XConsole layout. */
    for (y=0;y<h && zh_child_probe_log_count<ZH_CHILD_PROBE_LOG_LIMIT;++y) {
        char line[ZH_CHILD_PROBE_MAX_W+1];
        for (x=0;x<w;++x) {
            DWORD glyph=snapshot[(SIZE_T)x*(SIZE_T)h+(SIZE_T)y].glyph;
            line[x]=(glyph>=32 && glyph<=126)?(char)glyph:' ';
        }
        line[w]=0;
        for (x=0;x<w && zh_child_probe_log_count<ZH_CHILD_PROBE_LOG_LIMIT;++x) {
            for (i=0;i<sizeof keywords/sizeof keywords[0];++i) {
                if (!ZhChildProbeFind(line,w,x,keywords[i]) ||
                    ZhChildProbeAlreadyHit(self,y,x,keywords[i])) continue;
                ZhChildProbeLogHit(self,dst,w,h,y,x,keywords[i],line);
            }
        }
    }
    free(snapshot);
}

#endif
