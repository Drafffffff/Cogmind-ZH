/* Beta 17.1 source hook: read-only by default; optional intro provider
   uses a temporary native string copied by the original constructor. */
typedef unsigned (__thiscall *ZhStringLengthFn)(const void *);
typedef const char *(__thiscall *ZhStringAtFn)(const void *, unsigned);
typedef void *(__thiscall *ZhAnimatedCtorFn)(void *, void *, void *, void *, int, int, int);
static ZhAnimatedCtorFn zh_animated_ctor;
static ZhStringLengthFn zh_string_length;
static ZhStringAtFn zh_string_at;
static LONG zh_animation_observations;
#ifdef ZH_ANIMATION_LAYOUT
typedef void *(__thiscall *ZhStringCtorFn)(void *,const char *);
typedef void (__thiscall *ZhStringDtorFn)(void *);
static ZhStringCtorFn zh_string_ctor;
static ZhStringDtorFn zh_string_dtor;
#endif

static void *__fastcall ZhObserveAnimatedCtor(void *self, void *unused,
    void *a1, void *pair, void *source, int a4, int a5, int a6)
{
    char text[256];
    unsigned i, length;
    int meaningful=0;
    (void)unused;
#ifdef ZH_ANIMATION_LAYOUT
    if (source && !IsBadReadPtr(source,0x1c)) {
        unsigned size=zh_string_length(source);
        const char *bytes=size<256?zh_string_at(source,0):NULL;
        size_t index;
        if(bytes && !IsBadReadPtr(bytes,size)) for(index=0;index<ZH_ANIMATION_COUNT;++index) {
            const ZhAnimationText *entry=&zh_animation_texts[index];
            if(strlen(entry->source)==size && !memcmp(entry->source,bytes,size)) {
                union { DWORD align; BYTE bytes[0x1c]; } scratch;
                char provider[256];
                int cells=(int)size,required=cm_cells(entry->text);
                void *result;
                int status=entry->status_after>=0;
                if(cells<required) cells=required;
                if(cells>=sizeof provider) break;
                memset(provider,entry->marker,(size_t)cells);provider[cells]=0;
                memset(&scratch,0,sizeof scratch);
                zh_string_ctor(scratch.bytes,provider);
                zh_animation_states[index].cells=cells;
                zh_animation_states[index].found=status && pair && !IsBadReadPtr(pair,8);
                if(zh_animation_states[index].found) {
                    /* Keep the original native grid footprint: moving it would
                       overwrite the preceding label's English-sized provider. */
                    zh_animation_states[index].col=((const int *)pair)[0]*2;
                    zh_animation_states[index].row=((const int *)pair)[1];
                    L("[zh-status-layout] index=%u native_col=%d row=%d translated_col=%d block_width=%d provider_cells=%d",
                      (unsigned)index,zh_animation_states[index].col,zh_animation_states[index].row,
                      ZhStatusColumn(index),required+2,cells);
                }
                zh_animation_states[index].active=1;
                L("[zh-animation-provider] index=%u cells=%d source=%s",(unsigned)index,cells,entry->source);
                result=zh_animated_ctor(self,a1,pair,scratch.bytes,a4,a5,a6);
                zh_string_dtor(scratch.bytes);
                return result;
            }
        }
    }
#endif
    if (source && !IsBadReadPtr(source, 0x1c)) {
        length = zh_string_length(source);
        if (length && length < sizeof text) {
            const char *bytes = zh_string_at(source, 0);
            if (bytes && !IsBadReadPtr(bytes, length)) {
                for (i = 0; i < length; ++i)
                    text[i] = ((unsigned char)bytes[i] >= 32 && (unsigned char)bytes[i] <= 126) ? bytes[i] : ' ';
                text[length] = 0;
                for(i=0;i<length;++i) if((text[i]>='A' && text[i]<='Z') ||
                    (text[i]>='a' && text[i]<='z')) {meaningful=1;break;}
                if (!meaningful || InterlockedIncrement(&zh_animation_observations)>256)
                    return zh_animated_ctor(self,a1,pair,source,a4,a5,a6);
                if (pair && !IsBadReadPtr(pair, 8))
                    L("[zh-animation-source] self=%p pair=%p p0=%d p1=%d a4=%d a5=%d a6=%d length=%u source=%s",
                      self, pair, ((const int *)pair)[0], ((const int *)pair)[1], a4, a5, a6, length, text);
            }
        }
    }
    return zh_animated_ctor(self, a1, pair, source, a4, a5, a6);
}

static int InstallZhAnimationProbe(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *target = base + 0x8d2c0;
    static const BYTE expected[10] = {0x55,0x8b,0xec,0x6a,0xff,0x68,0xd3,0xab,0xac,0x00};
    BYTE *trampoline;
    DWORD protection, ignored;
    if (IsBadReadPtr(target, sizeof expected) || memcmp(target, expected, sizeof expected)) {
        L("[zh-animation-source] unsupported constructor bytes; probe disabled");
        return 0;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) return 0;
    memcpy(trampoline, target, 10);
    trampoline[10] = 0xe9;
    *(DWORD *)(trampoline + 11) = (DWORD)((ULONG_PTR)(target + 10) - (ULONG_PTR)(trampoline + 15));
    zh_animated_ctor = (ZhAnimatedCtorFn)trampoline;
    zh_string_length = (ZhStringLengthFn)(base + 0x5b14b0);
    zh_string_at = (ZhStringAtFn)(base + 0x5b13e0);
#ifdef ZH_ANIMATION_LAYOUT
    zh_string_ctor = (ZhStringCtorFn)(base + 0x5b08d0);
    zh_string_dtor = (ZhStringDtorFn)(base + 0x5b0b30);
#endif
    if (!VirtualProtect(target, 10, PAGE_EXECUTE_READWRITE, &protection)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return 0;
    }
    target[0] = 0xe9;
    *(DWORD *)(target + 1) = (DWORD)((ULONG_PTR)ZhObserveAnimatedCtor - (ULONG_PTR)(target + 5));
    memset(target + 5, 0xcc, 5);
    VirtualProtect(target, 10, protection, &ignored);
    FlushInstructionCache(GetCurrentProcess(), target, 10);
#ifdef ZH_ANIMATION_LAYOUT
    L("[zh-animation-provider] constructor interception installed");
#else
    L("[zh-animation-source] constructor probe installed; text unchanged");
#endif
    return 1;
}
