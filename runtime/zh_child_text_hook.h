
/* Exact Beta17.1 text-wrapper interception; UTF-8 stays in our renderer. */
typedef int (__thiscall *ZhHelpLineFn)(void *,int,int,void *);
typedef int (__thiscall *ZhHelpStyledFn)(void *,int,int,int,void *);
static ZhHelpLineFn zh_help_line;
static ZhHelpStyledFn zh_help_styled;
typedef void (__thiscall *ZhChildDtorFn)(void *);
static ZhChildDtorFn zh_child_dtor;
static int zh_child_hooks_ready;
static void __fastcall ZhChildDtor(void *self,void *unused)
{
    (void)unused;
    ZhForgetChildText(self);
    zh_child_dtor(self);
}
static int __fastcall ZhHelpLine(void *self,void *unused,int x,int y,void *source)
{
    (void)unused;
    int result=zh_help_line(self,x,y,source);
    if(zh_child_hooks_ready) { ZhRememberChildText(self,x,y,source); ZhObserveChildParagraphSource(self,x,y,source); }
    return result;
}
static int __fastcall ZhHelpStyled(void *self,void *unused,int x,int y,int style,void *source)
{
    (void)unused;
    int result=zh_help_styled(self,x,y,style,source);
    if(zh_child_hooks_ready) { ZhRememberChildText(self,x,y,source); ZhObserveChildParagraphSource(self,x,y,source); }
    return result;
}
static void *ZhHelpDetour(DWORD rva,void *hook,const BYTE *expected,int size)
{
    BYTE *target=(BYTE *)GetModuleHandleA(NULL)+rva,*tr;
    DWORD old,ignored;
    if(size<5 || size>10 || IsBadReadPtr(target,size) || memcmp(target,expected,size)) return NULL;
    tr=(BYTE *)VirtualAlloc(NULL,32,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
    if(!tr) return NULL;
    memcpy(tr,target,size);tr[size]=0xe9;
    *(DWORD *)(tr+size+1)=(DWORD)((ULONG_PTR)(target+size)-(ULONG_PTR)(tr+size+5));
    if(!VirtualProtect(target,size,PAGE_EXECUTE_READWRITE,&old)) {
        VirtualFree(tr,0,MEM_RELEASE);return NULL;
    }
    target[0]=0xe9;*(DWORD *)(target+1)=(DWORD)((ULONG_PTR)hook-(ULONG_PTR)(target+5));
    memset(target+5,0x90,(size_t)size-5);VirtualProtect(target,size,old,&ignored);
    FlushInstructionCache(GetCurrentProcess(),target,size);
    return tr;
}
static void InstallZhChildTextHooks(void)
{
    static const BYTE line[6]={0x55,0x8b,0xec,0x83,0xec,0x0c};
    static const BYTE dtor[10]={0x55,0x8b,0xec,0x6a,0xff,0x68,0x16,0x49,0xac,0x00};
    zh_child_dtor=(ZhChildDtorFn)ZhHelpDetour(0x28610,(void *)ZhChildDtor,dtor,10);
    zh_help_line=(ZhHelpLineFn)ZhHelpDetour(0x18320,(void *)ZhHelpLine,line,6);
    zh_help_styled=(ZhHelpStyledFn)ZhHelpDetour(0x18370,(void *)ZhHelpStyled,line,6);
    zh_child_hooks_ready=zh_help_line && zh_help_styled && zh_child_dtor;
    L("[zh-child-text-hooks] line=%p styled=%p dtor=%p ready=%d",
      zh_help_line,zh_help_styled,zh_child_dtor,zh_child_hooks_ready);
}
