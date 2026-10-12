/* Bounded, caller-driven Zhuyin-to-Chinese reveal phase state.
   Call only for a source span that the caller has independently confirmed is
   visible. This helper never decides visibility, color, or whether to draw. */
#ifndef ZH_DECODE_STATE_H
#define ZH_DECODE_STATE_H

#include <string.h>

#ifndef ZH_DECODE_STATE_CAP
#define ZH_DECODE_STATE_CAP 512
#endif
#ifndef ZH_DECODE_TEXT_MAX
#define ZH_DECODE_TEXT_MAX 1024
#endif

typedef struct {
    int active;
    unsigned family;
    int x,y,w,h;
    unsigned text_hash;
    char text[ZH_DECODE_TEXT_MAX];
    unsigned started_ms;
    unsigned last_frame;
    unsigned duration_ms;
    unsigned long long stamp;
} ZhDecodeEntry;

static ZhDecodeEntry zh_decode_entries[ZH_DECODE_STATE_CAP];
static unsigned long long zh_decode_clock;

static unsigned ZhDecodeHash(const char *text)
{
    unsigned h=2166136261u;
    while(*text){h^=(unsigned char)*text++;h*=16777619u;}
    return h;
}

static unsigned ZhDecodeDuration(unsigned hash,unsigned family,int x,int y,int w,int h)
{
    unsigned mix=hash^(family*0x9e3779b9u)^(unsigned)x*0x85ebca6bu^
                 (unsigned)y*0xc2b2ae35u^(unsigned)w*0x27d4eb2fu^(unsigned)h;
    mix^=mix>>16;mix*=0x7feb352du;mix^=mix>>15;
    return 180u+mix%181u;
}

/* Returns nonzero when the target glyph at cell offset is decoded. `now_ms`
   should be GetTickCount(); unsigned subtraction makes its 32-bit wrap safe.
   Calling only while visible means a missed frame or a changed key restarts
   the bounded phase. Stable calls retain their completed state. */
static int ZhDecodeGlyphReady(unsigned family,int x,int y,int w,int h,
                             const char *text,unsigned now_ms,
                             unsigned frame,unsigned offset,unsigned total)
{
    ZhDecodeEntry *entry=NULL,*free_entry=NULL,*oldest=NULL,*same_rect=NULL;
    unsigned hash,elapsed,threshold;size_t length;unsigned i;
    if(!text||!*text||w<=0||h<=0||total==0||offset>=total)return 0;
    length=strlen(text);if(length>=ZH_DECODE_TEXT_MAX)return 0;
    hash=ZhDecodeHash(text);
    for(i=0;i<ZH_DECODE_STATE_CAP;++i){ZhDecodeEntry *e=&zh_decode_entries[i];
        if(e->active&&e->family==family&&e->x==x&&e->y==y&&e->w==w&&e->h==h&&
           e->text_hash==hash&&strcmp(e->text,text)==0){entry=e;break;}
        if(e->active&&e->family==family&&e->x==x&&e->y==y&&e->w==w&&e->h==h)
            same_rect=e;
        if(!e->active&&!free_entry)free_entry=e;
        if(e->active&&(!oldest||e->stamp<oldest->stamp))oldest=e;
    }
    if(!entry){
        /* One visible rectangle has one current owner: A→B→A must not revive
           A's earlier completed phase. Reuse its previous slot on text change. */
        entry=same_rect?same_rect:(free_entry?free_entry:oldest);
        if(!entry)return 0;
        memset(entry,0,sizeof *entry);entry->active=1;entry->family=family;
        entry->x=x;entry->y=y;entry->w=w;entry->h=h;entry->text_hash=hash;
        memcpy(entry->text,text,length+1);entry->started_ms=now_ms;
        entry->last_frame=frame;
        entry->duration_ms=ZhDecodeDuration(hash,family,x,y,w,h);
    } else if(frame!=entry->last_frame){
        /* Exactly one sequential frame may elapse between visible samples.
           A missing frame, including non-consecutive wrap, denotes departure. */
        if((unsigned)(frame-entry->last_frame)!=1u){
            entry->started_ms=now_ms;entry->duration_ms=ZhDecodeDuration(hash,family,x,y,w,h);
        }
        entry->last_frame=frame;
    }
    entry->stamp=++zh_decode_clock;
    elapsed=(unsigned)(now_ms-entry->started_ms);
    if(elapsed>=entry->duration_ms)return 1;
    threshold=(unsigned)(((unsigned long long)(elapsed+1u)*total)/entry->duration_ms);
    return offset<threshold;
}

#endif
