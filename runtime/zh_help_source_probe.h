/* Read-only observer for candidate help-text source wrappers.
   EngineString is inspected as bytes only; this probe never calls game code. */
#ifndef ZH_HELP_SOURCE_PROBE_H
#define ZH_HELP_SOURCE_PROBE_H

#include <stdio.h>
#include <string.h>

#define ZH_HELP_PROBE_LIMIT 128
#define ZH_HELP_FRAME_LOG_LIMIT 256
#define ZH_HELP_CONSOLE_LIMIT 16
#define ZH_HELP_SOURCE_MAX 512
#define ZH_HELP_SELF_BYTES 0x40
#define ZH_HELP_STRING_BYTES 0x1c

typedef struct {
    const void *self;
    const void *source;
    unsigned hash;
    int x,y,kind;
    unsigned length;
} ZhHelpProbeSeen;

typedef struct {
    DWORD f0,glyph,f8;
    BYTE fgR,fgG,fgB,bgR,bgG,bgB;
    WORD pad;
} ZhHelpProbeCell;

static ZhHelpProbeSeen zh_help_probe_seen[ZH_HELP_PROBE_LIMIT];
static unsigned zh_help_probe_seen_count;
static unsigned zh_help_probe_log_count;
static void *zh_help_probe_consoles[ZH_HELP_CONSOLE_LIMIT];
static unsigned zh_help_probe_console_count;
static unsigned zh_help_probe_frame_log_count;

static int ZhHelpProbeRead(const void *address,void *out,SIZE_T size)
{
    SIZE_T got=0;
    if (!address || !out || !size) return 0;
    return ReadProcessMemory(GetCurrentProcess(),address,out,size,&got) && got==size;
}

static int ZhHelpProbeContains(const char *text,unsigned len,const char *word)
{
    unsigned i;size_t j,n=strlen(word);
    if (n>len) return 0;
    for (i=0;i+n<=len;++i) {
        for (j=0;j<n;++j) {
            unsigned char a=(unsigned char)text[i+j],b=(unsigned char)word[j];
            if (a>='A' && a<='Z') a=(unsigned char)(a+('a'-'A'));
            if (b>='A' && b<='Z') b=(unsigned char)(b+('a'-'A'));
            if (a!=b) break;
        }
        if (j==n) return 1;
    }
    return 0;
}

static unsigned ZhHelpProbeHash(const unsigned char *text,unsigned len)
{
    unsigned h=2166136261u,i;
    for (i=0;i<len;++i) {h^=text[i];h*=16777619u;}
    return h;
}

static int ZhHelpProbeAlreadySeen(const void *self,const void *source,unsigned hash,
                                  int x,int y,int kind,unsigned length)
{
    unsigned i;
    for (i=0;i<zh_help_probe_seen_count;++i) {
        ZhHelpProbeSeen *s=&zh_help_probe_seen[i];
        if (s->self==self && s->source==source && s->hash==hash && s->x==x &&
            s->y==y && s->kind==kind && s->length==length) return 1;
    }
    return 0;
}

static void ZhHelpProbeRememberConsole(void *self)
{
    unsigned i;
    for (i=0;i<zh_help_probe_console_count;++i)
        if (zh_help_probe_consoles[i]==self) return;
    if (zh_help_probe_console_count<ZH_HELP_CONSOLE_LIMIT)
        zh_help_probe_consoles[zh_help_probe_console_count++]=self;
}

/* `source` is a 0x1c-byte EngineString object. Verified layout from the
   native accessors: length at +0x10, capacity at +0x14; capacity<16 uses the
   inline buffer at +0, otherwise +0 holds the heap data pointer. */
static void ZhObserveHelpSource(void *self,int x,int y,void *source,int kind)
{
    BYTE self_bytes[ZH_HELP_SELF_BYTES];
    BYTE string_object[ZH_HELP_STRING_BYTES];
    char self_hex[ZH_HELP_SELF_BYTES*2+1];
    char string_hex[ZH_HELP_STRING_BYTES*2+1];
    char text[ZH_HELP_SOURCE_MAX+1];
    const void *data;
    DWORD length,capacity,data_pointer;
    unsigned hash,i;
    int relevant;
    static const char hex[]="0123456789ABCDEF";

    if (zh_help_probe_log_count>=ZH_HELP_PROBE_LIMIT || !self || !source) return;
    if (!ZhHelpProbeRead(self,self_bytes,sizeof self_bytes) ||
        !ZhHelpProbeRead(source,string_object,sizeof string_object)) return;
    memcpy(&length,string_object+0x10,sizeof length);
    memcpy(&capacity,string_object+0x14,sizeof capacity);
    if (!length || length>ZH_HELP_SOURCE_MAX || capacity<length) return;
    if (capacity<16) {
        if (length>=16) return;
        data=source;
    } else {
        memcpy(&data_pointer,string_object,sizeof data_pointer);
        if (!data_pointer) return;
        data=(const void *)(ULONG_PTR)data_pointer;
    }
    if (!ZhHelpProbeRead(data,text,(SIZE_T)length)) return;
    for (i=0;i<length;++i)
        if ((unsigned char)text[i]<32 || (unsigned char)text[i]>126) return;
    text[length]=0;
    relevant=ZhHelpProbeContains(text,length,"click") ||
             ZhHelpProbeContains(text,length,"scroll") ||
             ZhHelpProbeContains(text,length,"expand") ||
             ZhHelpProbeContains(text,length,"shrink");
    if (!relevant) return;
    ZhHelpProbeRememberConsole(self);

    hash=ZhHelpProbeHash((const unsigned char *)text,length);
    if (ZhHelpProbeAlreadySeen(self,source,hash,x,y,kind,length)) return;
    if (zh_help_probe_seen_count>=ZH_HELP_PROBE_LIMIT) return;
    {
        ZhHelpProbeSeen *s=&zh_help_probe_seen[zh_help_probe_seen_count++];
        s->self=self;s->source=source;s->hash=hash;s->x=x;s->y=y;s->kind=kind;s->length=length;
    }
    for (i=0;i<sizeof self_bytes;++i) {
        self_hex[i*2]=hex[self_bytes[i]>>4];self_hex[i*2+1]=hex[self_bytes[i]&15];
    }
    self_hex[sizeof self_hex-1]=0;
    for (i=0;i<sizeof string_object;++i) {
        string_hex[i*2]=hex[string_object[i]>>4];string_hex[i*2+1]=hex[string_object[i]&15];
    }
    string_hex[sizeof string_hex-1]=0;
    ++zh_help_probe_log_count;
    L("[zh-help-source] frame=%u self=%p x=%d y=%d kind=%d source=%p len=%u text=|%s| srcobj0x00_1b=%s self0x00_3f=%s",
      (unsigned)g_frame,self,x,y,kind,source,(unsigned)length,text,string_hex,self_hex);
}

/* Sample only keyword-bearing source owners saved above. Call from OnFrame;
   this reads but never mutates the candidate child XConsole or its cells. */
static void ZhObserveHelpFrames(void)
{
    unsigned ci;
    if (g_frame>70 || (g_frame%3)!=1 ||
        zh_help_probe_frame_log_count>=ZH_HELP_FRAME_LOG_LIMIT) return;
    for (ci=0;ci<zh_help_probe_console_count &&
         zh_help_probe_frame_log_count<ZH_HELP_FRAME_LOG_LIMIT;++ci) {
        BYTE object[0x24];
        int w,h,offset_x,offset_y,x,y;
        DWORD cells_pointer;
        ZhHelpProbeCell *cells;
        ZhHelpProbeCell *snapshot;
        SIZE_T bytes;
        char raw0[ZH_HELP_SOURCE_MAX+1],raw1[ZH_HELP_SOURCE_MAX+1];
        char colors[2][1200];
        unsigned row;
        void *self=zh_help_probe_consoles[ci];
        if (!self || !ZhHelpProbeRead(self,object,sizeof object)) continue;
        memcpy(&w,object+0x08,sizeof w);
        memcpy(&h,object+0x0c,sizeof h);
        memcpy(&cells_pointer,object+0x10,sizeof cells_pointer);
        memcpy(&offset_x,object+0x1c,sizeof offset_x);
        memcpy(&offset_y,object+0x20,sizeof offset_y);
        if (w<=0 || h<=0 || w>400 || h>200 || !cells_pointer) continue;
        if ((SIZE_T)w>(SIZE_T)-1/(SIZE_T)h/sizeof(ZhHelpProbeCell)) continue;
        bytes=(SIZE_T)w*(SIZE_T)h*sizeof(ZhHelpProbeCell);
        cells=(ZhHelpProbeCell *)(ULONG_PTR)cells_pointer;
        snapshot=(ZhHelpProbeCell *)malloc(bytes);
        if (!snapshot) continue;
        if (!ZhHelpProbeRead(cells,snapshot,bytes)) {free(snapshot);continue;}
        raw0[0]=raw1[0]=0;
        for (y=0;y<2 && y<h;++y) {
            char *raw=y?raw1:raw0;
            int limit=w<ZH_HELP_SOURCE_MAX? w:ZH_HELP_SOURCE_MAX;
            for (x=0;x<limit;++x) {
                DWORD glyph=snapshot[(SIZE_T)x*(SIZE_T)h+(SIZE_T)y].glyph;
                raw[x]=(glyph>=32 && glyph<=126)?(char)glyph:'?';
            }
            raw[limit]=0;
        }
        for (row=0;row<2 && row<(unsigned)h;++row) {
            int used=0,limit=w<24?w:24;
            char *out=colors[row];out[0]=0;
            for (x=0;x<limit;++x) {
                ZhHelpProbeCell *c=&snapshot[(SIZE_T)x*(SIZE_T)h+row];
                int n=snprintf(out+used,sizeof colors[row]-(size_t)used,
                    "%s%d:%02X%02X%02X/%02X%02X%02X",
                    x?",":"",x,(unsigned)c->fgR,(unsigned)c->fgG,(unsigned)c->fgB,
                    (unsigned)c->bgR,(unsigned)c->bgG,(unsigned)c->bgB);
                if (n<0 || (size_t)n>=sizeof colors[row]-(size_t)used) break;
                used+=n;
            }
        }
        free(snapshot);
        ++zh_help_probe_frame_log_count;
        L("[zh-help-frame] frame=%u self=%p offset=%d,%d grid=%dx%d row0=|%s| row1=|%s| colors0=%s colors1=%s",
          (unsigned)g_frame,self,offset_x,offset_y,w,h,raw0,raw1,colors[0],colors[1]);
    }
}

#endif
