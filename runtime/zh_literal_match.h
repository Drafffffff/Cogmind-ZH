/* Complete static literal lookup shared by all reveal overlays. Table
   placement must not determine whether a label gets a Chinese animation. */
#ifndef ZH_LITERAL_MATCH_H
#define ZH_LITERAL_MATCH_H
static int ZhLiteralEqual(const char*a,const char*b,int n,int fold)
{
    int i;for(i=0;i<n;++i){unsigned char x=a[i],y=b[i];
        if(fold){if(x>='A'&&x<='Z')x+=32;if(y>='A'&&y<='Z')y+=32;}
        if(x!=y)return 0;
    }return 1;
}
static int ZhLiteralInTable(Rule*table,int*arr,int*ofs,int count,
                            const char*source,int n,char*out,int cap,int fold)
{
    int b,k,first=(unsigned char)source[0];
    if(!table||!arr||!ofs||count<=0||n<=0||cap<=0)return 0;
    for(b=0;b<256;++b){
        char a=(char)first,c=(char)b;
        if(!ZhLiteralEqual(&a,&c,1,fold))continue;
        for(k=BkFit(table,arr,ofs[b],ofs[b+1],n);k<ofs[b+1];++k){
            Rule*r=&table[arr[k]];Caps cp;int lit[MAX_TOK];
            if(r->ns!=1||r->nd!=1||r->ncap||r->s[0].cap||r->d[0].cap||
               r->s[0].len!=n||r->d[0].len<=0||r->d[0].len>=cap||
               !ZhLiteralEqual(source,r->s[0].lit,n,fold)||
               !cm_match(r,r->s[0].lit,n,&cp,lit)||
               memchr(r->d[0].lit,CM_BR,(size_t)r->d[0].len)||
               memchr(r->d[0].lit,CM_SHL,(size_t)r->d[0].len))continue;
            memcpy(out,r->d[0].lit,(size_t)r->d[0].len);out[r->d[0].len]=0;return 1;
        }
    }return 0;
}
static int ZhLiteralTarget(const char*source,int n,char*out,int cap,int allow_fold)
{
    int fold;if(!source||n<=0||!out)return 0;
    for(fold=0;fold<=allow_fold;++fold){
        if(ZhLiteralInTable(g_tr,g_trArr,g_trOfs,g_ntr,source,n,out,cap,fold)||
           ZhLiteralInTable(g_pa,g_paArr,g_paOfs,g_npa,source,n,out,cap,fold))return 1;
    }return 0;
}
#endif
