/* Beta17.1 read-only event-0x323 message observation. No source replacement. */
typedef DWORD (__cdecl *ZhHudDispatchFn)(DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
static ZhHudDispatchFn zh_hud_dispatch;
static unsigned zh_hud_observations;
static char zh_hud_observed[128][256];
static DWORD __cdecl ZhObserveHudDispatch(DWORD event,DWORD source,DWORD a3,DWORD a4,
    DWORD a5,DWORD a6,DWORD a7,DWORD a8)
{
    if(event==0x323) ++zh_hud_event_count;
    if(event==0x323 && source && !IsBadReadPtr((void *)source,0x1c)) {
        unsigned size=zh_string_length((void *)source),i;
        const char *bytes=size && size<256?zh_string_at((void *)source,0):NULL;
        if(bytes && !IsBadReadPtr(bytes,size)) {
            for(i=0;i<zh_hud_observations;++i)
                if(strlen(zh_hud_observed[i])==size && !memcmp(zh_hud_observed[i],bytes,size)) break;
            if(i==zh_hud_observations && i<128) {
                memcpy(zh_hud_observed[i],bytes,size);zh_hud_observed[i][size]=0;
                ++zh_hud_observations;
                L("[zh-hud-source] event=%x a3=%x a4=%x a5=%x a6=%x a7=%x a8=%x source=%s",
                  event,a3,a4,a5,a6,a7,a8,zh_hud_observed[i]);
            }
        }
    }
    return zh_hud_dispatch(event,source,a3,a4,a5,a6,a7,a8);
}
static int InstallZhHudProbe(void)
{
    BYTE *target=(BYTE *)GetModuleHandleA(NULL)+0x1122d0,*trampoline;
    const BYTE expected[10]={0x55,0x8b,0xec,0x6a,0xff,0x68,0xb4,0x1c,0xad,0x00};
    DWORD protection,ignored;
    if(IsBadReadPtr(target,10) || memcmp(target,expected,10)) {
        L("[zh-hud-source] unsupported dispatcher bytes; disabled");return 0;
    }
    trampoline=(BYTE *)VirtualAlloc(NULL,32,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    if(!trampoline) return 0;
    memcpy(trampoline,target,10);trampoline[10]=0xe9;
    *(DWORD *)(trampoline+11)=(DWORD)((ULONG_PTR)(target+10)-(ULONG_PTR)(trampoline+15));
    zh_hud_dispatch=(ZhHudDispatchFn)trampoline;
    if(!VirtualProtect(target,10,PAGE_EXECUTE_READWRITE,&protection)) {
        VirtualFree(trampoline,0,MEM_RELEASE);return 0;
    }
    target[0]=0xe9;*(DWORD *)(target+1)=(DWORD)((ULONG_PTR)ZhObserveHudDispatch-(ULONG_PTR)(target+5));
    memset(target+5,0xcc,5);VirtualProtect(target,10,protection,&ignored);
    FlushInstructionCache(GetCurrentProcess(),target,10);
    L("[zh-hud-source] read-only dispatcher probe installed");return 1;
}
