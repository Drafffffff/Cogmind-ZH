/* Bounded read-only observer for paragraph-sized child-console source text.
   Include after zh_child_text_draw.h; this does not register or draw text. */
#ifndef ZH_CHILD_PARAGRAPH_DRAW_H
#define ZH_CHILD_PARAGRAPH_DRAW_H

#include <string.h>

#define ZH_CHILD_PARAGRAPH_MAX_SOURCE 512
#define ZH_CHILD_PARAGRAPH_LOG_MAX 96

static unsigned zh_child_paragraph_log_count;

static int ZhParagraphHasFolded(const char *source,unsigned length,const char *needle)
{
    unsigned i,n=(unsigned)strlen(needle),j;
    if(!n||n>length)return 0;
    for(i=0;i+n<=length;++i){
        for(j=0;j<n;++j){char a=source[i+j],b=needle[j];
            if(a>='A'&&a<='Z')a=(char)(a+('a'-'A'));
            if(b>='A'&&b<='Z')b=(char)(b+('a'-'A'));
            if(a!=b)break;
        }
        if(j==n)return 1;
    }
    return 0;
}

static int ZhParagraphInteresting(const char *source,unsigned length)
{
    static const char *terms[]={"light treads","crush","stacks","recoil",
        "core integrity","knockback","immunity"};
    unsigned i;
    for(i=0;i<sizeof terms/sizeof terms[0];++i)
        if(ZhParagraphHasFolded(source,length,terms[i]))return 1;
    return 0;
}

static int ZhParagraphReadString(void *object,char *out,unsigned *out_length,
                                 unsigned *out_capacity)
{
    BYTE bytes[0x1c];DWORD length,capacity,data_pointer;const void *data;unsigned i;
    if(!object||!out||!out_length||!out_capacity||
       !ZhChildTextRead(object,bytes,sizeof bytes))return 0;
    memcpy(&length,bytes+0x10,4);memcpy(&capacity,bytes+0x14,4);
    if(!length||length>ZH_CHILD_PARAGRAPH_MAX_SOURCE||capacity<length||capacity>4096)return 0;
    if(capacity<16){if(length>=16)return 0;data=object;}
    else{memcpy(&data_pointer,bytes,4);if(!data_pointer)return 0;data=(const void *)(ULONG_PTR)data_pointer;}
    if(!ZhChildTextRead(data,out,(SIZE_T)length))return 0;
    for(i=0;i<length;++i)if((unsigned char)out[i]<32||(unsigned char)out[i]>126)return 0;
    out[length]=0;*out_length=length;*out_capacity=capacity;return 1;
}

/* Call after the native child-text wrapper returns. The native source object
   is read only; logging is capped and limited to Light Treads effect phrases. */
static void ZhObserveChildParagraphSource(void *self,int x,int y,void *source_object)
{
    char source[ZH_CHILD_PARAGRAPH_MAX_SOURCE+1];unsigned length,capacity;
    ZhChildTextHeader h;int resolved=0,root_x=0,root_y=0;
    if(zh_child_paragraph_log_count>=ZH_CHILD_PARAGRAPH_LOG_MAX||!source_object)return;
    memset(&h,0,sizeof h);
    if(!ZhParagraphReadString(source_object,source,&length,&capacity)||
       !ZhParagraphInteresting(source,length))return;
    if(ZhChildTextHeaderRead(self,&h))
        resolved=ZhChildTextResolveRootPosition(self,&h,&root_x,&root_y);
    ++zh_child_paragraph_log_count;
    if(resolved){
        L("[child-paragraph-source] self=%p root=%p xy=%d,%d len=%u cap=%u grid=%dx%d cells=%08x parent=%08x scale=%d anchor=%d,%d project=%d,%d rootxy=%d,%d frame=%u src=%.*s",
          self,g_console,x,y,length,capacity,h.w,h.h,(unsigned)h.cells,(unsigned)h.parent,
          h.scale_index,h.anchor_x,h.anchor_y,h.project_x,h.project_y,root_x+x,root_y+y,
          (unsigned)g_frame,(int)length,source);
    }else if(h.w>0){
        L("[child-paragraph-source] self=%p root=%p xy=%d,%d len=%u cap=%u grid=%dx%d cells=%08x parent=%08x scale=%d anchor=%d,%d project=%d,%d rootxy=unresolved frame=%u src=%.*s",
          self,g_console,x,y,length,capacity,h.w,h.h,(unsigned)h.cells,(unsigned)h.parent,
          h.scale_index,h.anchor_x,h.anchor_y,h.project_x,h.project_y,(unsigned)g_frame,
          (int)length,source);
    }else{
        L("[child-paragraph-source] self=%p root=%p xy=%d,%d len=%u cap=%u header=unreadable frame=%u src=%.*s",
          self,g_console,x,y,length,capacity,(unsigned)g_frame,(int)length,source);
    }
}

#endif
