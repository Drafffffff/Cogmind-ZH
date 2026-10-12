/* Bounded, read-only trace of the observed exclamation-mark map-label sources.
   This is used only by the separate map-label diagnostic package. */
#ifndef ZH_MAP_LABEL_PROBE_H
#define ZH_MAP_LABEL_PROBE_H

#include <string.h>

#define ZH_MAP_LABEL_MAX_SOURCE 512
#define ZH_MAP_LABEL_SOURCE_SLOTS 128
#define ZH_MAP_LABEL_PER_SOURCE 3
#define ZH_MAP_LABEL_TOTAL 256
#define ZH_MAP_LABEL_ROW_CELLS 64

typedef struct {
    char text[ZH_MAP_LABEL_MAX_SOURCE+1];
    unsigned count;
    unsigned last_frame;
    int last_x,last_y;
    void *last_self;
} ZhMapLabelSeen;
static ZhMapLabelSeen zh_map_label_seen[ZH_MAP_LABEL_SOURCE_SLOTS];
static unsigned zh_map_label_seen_count;
static unsigned zh_map_label_log_count;

static int ZhMapLabelRead(const void *address,void *out,SIZE_T size)
{
    SIZE_T got=0;
    if(!address||!out||!size)return 0;
    return ReadProcessMemory(GetCurrentProcess(),address,out,size,&got)&&got==size;
}

static int ZhMapLabelReadSource(void *object,char *out,unsigned *out_length)
{
    BYTE bytes[0x1c];DWORD length,capacity,data_pointer;const void *data;unsigned i;
    if(!object||!ZhMapLabelRead(object,bytes,sizeof bytes))return 0;
    memcpy(&length,bytes+0x10,sizeof length);memcpy(&capacity,bytes+0x14,sizeof capacity);
    if(!length||length>ZH_MAP_LABEL_MAX_SOURCE||capacity<length)return 0;
    if(capacity<16){if(length>=16)return 0;data=object;}
    else {memcpy(&data_pointer,bytes,sizeof data_pointer);if(!data_pointer)return 0;data=(const void *)(ULONG_PTR)data_pointer;}
    if(!ZhMapLabelRead(data,out,(SIZE_T)length))return 0;
    for(i=0;i<length;++i)if((unsigned char)out[i]<32||(unsigned char)out[i]>126)return 0;
    out[length]=0;*out_length=length;return 1;
}

/* The captured batch-label examples all carry a trailing '!'. This keeps
   ordinary help/log text out of the bounded diagnostic stream. */
static int ZhMapLabelCandidate(const char *text)
{
    return text&&strchr(text,'!')!=NULL;
}

static ZhMapLabelSeen *ZhMapLabelGetSeen(const char *text)
{
    unsigned i;
    for(i=0;i<zh_map_label_seen_count;++i)
        if(!strcmp(zh_map_label_seen[i].text,text))return &zh_map_label_seen[i];
    if(zh_map_label_seen_count>=ZH_MAP_LABEL_SOURCE_SLOTS)return NULL;
    {
        ZhMapLabelSeen *s=&zh_map_label_seen[zh_map_label_seen_count++];
        memset(s,0,sizeof *s);strncpy(s->text,text,sizeof s->text-1);return s;
    }
}

static void ZhMapLabelReadExcerpt(const void *cells,int w,int h,int x,int y,
                                  char out[ZH_MAP_LABEL_ROW_CELLS+1])
{
    int start,end,i;out[0]=0;
    if(!cells||w<=0||w>400||h<=0||h>200||y<0||y>=h)return;
    start=x-12;if(start<0)start=0;if(start>w)start=w;
    end=start+ZH_MAP_LABEL_ROW_CELLS;if(end>w)end=w;
    for(i=start;i<end;++i){DWORD glyph=0;
        const BYTE *cell=(const BYTE *)cells+((SIZE_T)i*(SIZE_T)h+(SIZE_T)y)*20u;
        if(!ZhMapLabelRead(cell+4,&glyph,sizeof glyph)){out[0]=0;return;}
        out[i-start]=(glyph>=32&&glyph<=126)?(char)glyph:'?';
    }
    out[end-start]=0;
}

static void ZhObserveMapLabelSource(void *self,int x,int y,void *source,int style,
                                   void *return_address)
{
    char text[ZH_MAP_LABEL_MAX_SOURCE+1],child_row[ZH_MAP_LABEL_ROW_CELLS+1];
    char root_row[ZH_MAP_LABEL_ROW_CELLS+1];unsigned length;ZhMapLabelSeen *seen;
    BYTE object[0x2c];int w=0,h=0,px=0,py=0,rx=-1,ry=-1,is_root;
    DWORD cells_pointer=0;const void *child_cells=NULL;const char *kind;
    if(zh_map_label_log_count>=ZH_MAP_LABEL_TOTAL||!self||!source||
       !ZhMapLabelReadSource(source,text,&length)||!ZhMapLabelCandidate(text))return;
    seen=ZhMapLabelGetSeen(text);if(!seen||seen->count>=ZH_MAP_LABEL_PER_SOURCE)return;
    is_root=(self==g_console);kind=is_root?"root":"child";
    if(!ZhMapLabelRead(self,object,sizeof object))return;
    memcpy(&w,object+0x08,sizeof w);memcpy(&h,object+0x0c,sizeof h);
    memcpy(&cells_pointer,object+0x10,sizeof cells_pointer);
    memcpy(&px,object+0x24,sizeof px);memcpy(&py,object+0x28,sizeof py);
    child_cells=(const void *)(ULONG_PTR)cells_pointer;
    ZhMapLabelReadExcerpt(child_cells,w,h,x,y,child_row);
    root_row[0]=0;
    if(is_root){
        ZhMapLabelReadExcerpt(g_grid.cells,g_grid.w,g_grid.h,x,y,root_row);rx=x;ry=y;
    }else{
        ZhChildTextHeader header;int root_x,root_y;
        /* Reuse the production helper's bounded parent-chain validation; do
           not treat a nested child's project fields as proven root coords. */
        if(ZhChildTextHeaderRead(self,&header)&&
           ZhChildTextResolveRootPosition(self,&header,&root_x,&root_y)){
            rx=root_x+x;ry=root_y+y;
            if(rx>=0&&ry>=0)ZhMapLabelReadExcerpt(g_grid.cells,g_grid.w,g_grid.h,rx,ry,root_row);
        }
    }
    /* Repeated frame callbacks at one location are sampled at most once per
       five frames; different coordinates still expose separate labels. */
    if(seen->count&&seen->last_self==self&&seen->last_x==x&&seen->last_y==y&&
       (unsigned)(g_frame-seen->last_frame)<5u)return;
    ++seen->count;seen->last_frame=(unsigned)g_frame;seen->last_self=self;
    seen->last_x=x;seen->last_y=y;++zh_map_label_log_count;
    L("[zh-map-label-source] n=%u/%u frame=%u style=%d kind=%s self=%p root=%p source_obj=%p ret=%p local=%d,%d console=%dx%d project=%d,%d root_at_validated=%d,%d text_len=%u text=|%s| child_row=|%s| root_row=|%s|",
      zh_map_label_log_count,ZH_MAP_LABEL_TOTAL,(unsigned)g_frame,style,kind,self,g_console,
      source,return_address,x,y,w,h,px,py,rx,ry,length,text,child_row,root_row);
}

#endif
