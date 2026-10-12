/* Source-confirmed live reveal overlay for observed robot DATA fields. */
#ifndef ZH_ROBOT_REVEAL_DRAW_H
#define ZH_ROBOT_REVEAL_DRAW_H
#include <string.h>
#include <stdlib.h>
#include "zh_decode_state.h"

static const char *zh_robot_noise[]={"ㄅ","ㄆ","ㄇ","ㄈ","ㄉ","ㄊ","ㄋ","ㄌ","ㄍ","ㄎ","ㄏ","ㄐ","ㄑ","ㄒ","ㄓ","ㄔ","ㄕ","ㄖ","ㄗ","ㄘ","ㄙ","ㄚ","ㄛ","ㄜ","ㄝ","ㄞ","ㄟ","ㄠ","ㄡ","ㄢ","ㄣ","ㄤ","ㄥ","ㄦ","ㄧ","ㄨ","ㄩ"};
static char ZhRobotChar(int x,int y){DWORD c=CELL(x,y).glyph;return c>=32&&c<=126?(char)c:' ';}
static int ZhRobotRowHas(int y,const char*s){int x,n=(int)strlen(s),j;for(x=0;x+n<=g_grid.w;++x){for(j=0;j<n;++j)if(ZhRobotChar(x+j,y)!=s[j])break;if(j==n)return 1;}return 0;}
static int ZhRobotPage(SSurf*s){return s&&g_on&&g_grid.w==160&&g_grid.h==45&&g_cellW>0&&g_cellH==2*g_cellW&&s->w>=g_grid.w*g_cellW&&s->h>=g_grid.h*g_cellH&&ZhRobotRowHas(0,"/ D A T A /")&&ZhRobotRowHas(4,"Overview")&&ZhRobotRowHas(18,"Components")&&ZhRobotRowHas(5,"Class")&&ZhRobotRowHas(9,"Movement");}
static int ZhRobotVisible(const XCell*c){return abs((int)c->fgR-(int)c->bgR)+abs((int)c->fgG-(int)c->bgG)+abs((int)c->fgB-(int)c->bgB)>3;}
/* This narrow vocabulary is backed by the captured DATA grid and loaded table. */
static int ZhRobotAllowed(const char*s){return !strcmp(s,"Overview")||!strcmp(s,"Recycler")||!strcmp(s,"Tunneler")||!strcmp(s,"Rolling ({n}%)")||!strcmp(s,"Rolling")||!strcmp(s,"Light Ion Engine ({n}%)")||!strcmp(s,"Light Ion Engine")||!strcmp(s,"Medium")||!strcmp(s,"Compact Wheel")||!strcmp(s,"Wheel")||!strcmp(s,"Improved Terrain Scanner")||!strcmp(s,"Seismic Detector")||!strcmp(s,"Tractor Beam")||!strcmp(s,"Small Storage Unit")||!strcmp(s,"Core Integrity")||!strcmp(s,"Core Temp")||!strcmp(s,"Core Exposure")||!strcmp(s,"Salvage Potential")||!strcmp(s,"Armament")||!strcmp(s,"Components")||!strcmp(s,"Resistances")||!strcmp(s,"Movement")||!strcmp(s,"Class")||!strcmp(s,"Size")||!strcmp(s,"Rating")||!strcmp(s,"ID")||!strcmp(s,"Type")||!strcmp(s,"Slot")||!strcmp(s,"Leg")||!strcmp(s,"Propulsion");}
static int ZhRobotTableLookup(Rule*t,int*arr,int*ofs,int count,const char*src,int n,Rule**hit,Caps*hitcaps,char*out,int cap){int b,k,hi,lit[MAX_TOK];Caps cp;if(!t||!arr||count<=0||n<=0)return 0;b=(unsigned char)src[0];hi=ofs[b+1];for(k=BkFit(t,arr,ofs[b],hi,n);k<hi;++k){Rule*r=&t[arr[k]];int i,valid=1;if(!r->raw||!ZhRobotAllowed(r->raw)||!cm_match(r,src,n,&cp,lit))continue;for(i=0;i<r->ns;++i)if(r->s[i].cap){int j;if(cp.len[i]<=0||cp.ofs[i]<0||cp.ofs[i]+cp.len[i]>n){valid=0;break;}for(j=0;j<cp.len[i];++j)if(src[cp.ofs[i]+j]<'0'||src[cp.ofs[i]+j]>'9'){valid=0;break;}if(!valid)break;}if(!valid)continue;cm_buildfree(r,src,&cp,out,cap);if(out[0]){*hit=r;*hitcaps=cp;return 1;}}return 0;}
/* If a captured component row is available only through the opening of its
   measured percentage suffix, do not let the shorter base-name rule turn that
   partial source into final Chinese. ZhRobotPartialField masks the visible
   name glyphs until the full captured row can be matched. */
static int ZhRobotDynamicPrefixAt(int x,int y,const char*src,int n){const char*base=NULL;int bl,k,wheel=0;if(y==20&&x==53){base="Wheel";wheel=1;}else if(y==21&&x==53)base="Improved Terrain Scanner";else if(y==22&&x==53)base="Seismic Detector";if(!base)return 0;bl=(int)strlen(base);if(n<=bl||memcmp(src,base,(size_t)bl))return 0;k=bl;if(k<n&&src[k++]!=' ')return 0;if(k<n&&src[k++]!='(')return 0;while(k<n&&src[k]>='0'&&src[k]<='9')++k;if(k<n&&src[k++]!='%')return 0;if(k<n&&src[k++]!=')')return 0;if(wheel){if(k<n&&src[k++]!=' ')return 0;if(k<n&&src[k++]!='x')return 0;while(k<n&&src[k]>='0'&&src[k]<='9')++k;}return 1;}
static int ZhRobotAsciiLetter(char c){return (c>='A'&&c<='Z')||(c>='a'&&c<='z');}
/* Pending dynamic rows must hide visible native glyphs too, not just decline
   to draw the Chinese result. Alphabetic pairs get Bopomofo; visible digits
   and punctuation are cleared until the actual capture is accepted. */
static void ZhRobotMaskPending(SSurf*s,int n,const int*px,const int*py){int i;for(i=0;i<n;i+=2){int j=i+1<n?i+1:i;char a=ZhRobotChar(px[i],py[i]),b=ZhRobotChar(px[j],py[j]);const XCell*ca=&CELL(px[i],py[i]);const XCell*cb=&CELL(px[j],py[j]);int va=ZhRobotVisible(ca),vb=j!=i&&ZhRobotVisible(cb);int digit=(va&&a>='0'&&a<='9')||(vb&&b>='0'&&b<='9');if(va){BYTE bg[3]={ca->bgR,ca->bgG,ca->bgB};DrawSpan(s,px[i],py[i],1,1,0,bg,"",0,0,0);}if(vb){BYTE bg[3]={cb->bgR,cb->bgG,cb->bgB};DrawSpan(s,px[j],py[j],1,1,0,bg,"",0,0,0);}if(!digit&&((va&&ZhRobotAsciiLetter(a))||(vb&&ZhRobotAsciiLetter(b)))){const XCell*c=(va&&ZhRobotAsciiLetter(a))?ca:cb;unsigned seed=(unsigned)(px[i]*7+py[i]*13+i+c->fgR+c->fgG*3+c->fgB*5);DrawSpan(s,px[i],py[i],2,1,0,NULL,zh_robot_noise[seed%(sizeof zh_robot_noise/sizeof zh_robot_noise[0])],c->fgR,c->fgG,c->fgB);}}}
static Rule zh_robot_dynamic[7];static int zh_robot_dynamic_ready;
static const char *zh_robot_dynamic_src[7]={"Light Ion Engine ({n}%)","Compact Wheel ({n}%) x{n}","Small Storage Unit ({n}%)","Tractor Beam ({n}%)","Wheel ({n}%) x{n}","Improved Terrain Scanner ({n}%)","Seismic Detector ({n}%)"};
static int ZhRobotDynamicInit(void){int i;if(zh_robot_dynamic_ready)return 1;for(i=0;i<7;++i)if(!cm_rulebuild(&zh_robot_dynamic[i],zh_robot_dynamic_src[i],"{1}"))return 0;zh_robot_dynamic_ready=1;return 1;}
static int ZhRobotIsDynamic(const Rule*r){int i;for(i=0;i<7;++i)if(r==&zh_robot_dynamic[i])return 1;return 0;}
static int ZhRobotDynamicLookup(const char*src,int n,Rule**hit,Caps*hitcaps,char*out,int cap){static const char*base[7]={"Light Ion Engine","Compact Wheel","Small Storage Unit","Tractor Beam","Wheel","Improved Terrain Scanner","Seismic Detector"};int i,j,lit[MAX_TOK];Caps cp;if(!ZhRobotDynamicInit())return 0;for(i=0;i<7;++i){Rule*r=&zh_robot_dynamic[i];Rule*basehit=NULL;Caps basecp;char translated[MAX_TEXT];int valid=1,bl=(int)strlen(base[i]),tl,sl=n-bl;if(!cm_match(r,src,n,&cp,lit)||sl<0)continue;for(j=0;j<r->ns;++j)if(r->s[j].cap){int k;if(cp.len[j]<=0||cp.ofs[j]<0||cp.ofs[j]+cp.len[j]>n){valid=0;break;}for(k=0;k<cp.len[j];++k)if(src[cp.ofs[j]+k]<'0'||src[cp.ofs[j]+k]>'9'){valid=0;break;}if(!valid)break;}if(!valid||!ZhRobotTableLookup(g_tr,g_trArr,g_trOfs,g_ntr,base[i],bl,&basehit,&basecp,translated,sizeof translated))continue;tl=(int)strlen(translated);if(tl+sl>=cap)continue;memcpy(out,translated,(size_t)tl);memcpy(out+tl,src+bl,(size_t)sl);out[tl+sl]=0;*hit=r;*hitcaps=cp;return 1;}return 0;}
static int ZhRobotLookup(const char*src,int n,Rule**hit,Caps*cp,char*out,int cap){return ZhRobotDynamicLookup(src,n,hit,cp,out,cap)||ZhRobotTableLookup(g_tr,g_trArr,g_trOfs,g_ntr,src,n,hit,cp,out,cap)||ZhRobotTableLookup(g_pa,g_paArr,g_paOfs,g_npa,src,n,hit,cp,out,cap);}
static int ZhRobotCaptureState(const Rule*r,const Caps*cp,int tok,const int*px,const int*py,int*visible){int i;(void)r;*visible=1;for(i=0;i<cp->len[tok];++i){const XCell*c=&CELL(px[cp->ofs[tok]+i],py[cp->ofs[tok]+i]);if(!ZhRobotVisible(c))*visible=0;}return 1;}
static int ZhRobotDrawSegment(SSurf*s,int x,int y,const char*source,int n,const int*px,const int*py)
{
    Rule *r=NULL;Caps cp;char out[MAX_TEXT];int ink[96],ni=0,i,tc,off=0;
    int capok[MAX_TOK]={0},digitgroup=0;const char*p;
    if(n<=0||n>=96)return 0;
    if(!ZhRobotLookup(source,n,&r,&cp,out,sizeof out)){
        if(ZhRobotDynamicPrefixAt(x,y,source,n)){ZhRobotMaskPending(s,n,px,py);return 2;}
        return 0;
    }
    tc=cm_cells(out);if(tc<=0||tc>n)return 0;
    for(i=0;i<n;++i)if(source[i]!=' ')ink[ni++]=i;
    if(!ni)return 0;

    /* Keep captured values behind their native visibility gate. */
    for(i=0;i<r->ns;++i)if(r->s[i].cap){int vis;
        ZhRobotCaptureState(r,&cp,i,px,py,&vis);capok[i]=vis;
        if(!vis){if(ZhRobotIsDynamic(r)){ZhRobotMaskPending(s,n,px,py);return 2;}return 0;}
    }
    for(i=0;i<n;++i){const XCell*c=&CELL(px[i],py[i]);BYTE bg[3]={c->bgR,c->bgG,c->bgB};
        DrawSpan(s,px[i],py[i],1,1,0,bg,"",0,0,0);
    }
    p=out;
    while(*p){unsigned code;const char*q=cm_u8next(p,&code);
        int w=cm_wide(code)?2:1,at=(off+w/2)*ni/tc,show=1,isnum=(code>='0'&&code<='9');
        const XCell*c;char glyph[8];size_t z=(size_t)(q-p);
        if(at>=ni)at=ni-1;c=&CELL(px[ink[at]],py[ink[at]]);
        if(isnum){int j;for(j=0;j<r->ns;++j)if(r->s[j].cap&&cp.len[j]>0){
            int a=cp.ofs[j],b=a+cp.len[j];if(at>=a&&at<b){if(!capok[j])show=0;break;}
        }
            ++digitgroup;
        }
        if(show&&ZhRobotVisible(c)&&off+w<=tc&&z<sizeof glyph){
            memcpy(glyph,p,z);glyph[z]=0;
            if(w==2&&!ZhDecodeGlyphReady(0x524f424fu,(unsigned)x,(unsigned)y,
                    (unsigned)n,1,out,(unsigned)GetTickCount(),(unsigned)g_frame,
                    (unsigned)off,(unsigned)tc)){
                unsigned seed=(unsigned)(x*7+y*13+off+digitgroup+
                    c->fgR+c->fgG*3+c->fgB*5);
                DrawSpan(s,x+off,y,w,1,0,NULL,
                    zh_robot_noise[seed%(sizeof zh_robot_noise/sizeof zh_robot_noise[0])],
                    c->fgR,c->fgG,c->fgB);
            }else DrawSpan(s,x+off,y,w,1,0,NULL,glyph,c->fgR,c->fgG,c->fgB);
        }
        off+=w;p=q;
    }
    return 1;
}
static void ZhRobotPartialField(SSurf*s,int y,int x,const char*word,unsigned salt,int complete_ok){int i,n=(int)strlen(word),matched=0;for(i=0;i<n;++i){char c=ZhRobotChar(x+i,y);if(c==word[i])++matched;else if(c!=' ')return;}if(!matched||(matched==n&&!complete_ok))return;for(i=0;i<matched;i+=2){int second=i+1<matched;const XCell*a=&CELL(x+i,y),*b=&CELL(x+i+1,y);if(!second&&ZhRobotChar(x+i+1,y)!=' ')continue;if(ZhRobotVisible(a)&&(second?ZhRobotVisible(b):1)){BYTE ba[3]={a->bgR,a->bgG,a->bgB},bb[3]={b->bgR,b->bgG,b->bgB};unsigned k=(unsigned)(x*7+y*13+i+salt+a->fgR+a->fgG*3+a->fgB*5);DrawSpan(s,x+i,y,1,1,0,ba,"",0,0,0);DrawSpan(s,x+i+1,y,1,1,0,bb,"",0,0,0);DrawSpan(s,x+i,y,2,1,0,NULL,zh_robot_noise[k%(sizeof zh_robot_noise/sizeof zh_robot_noise[0])],a->fgR,a->fgG,a->fgB);}}}
static void ZhRobotScanLine(SSurf*s,int y,int x0,int x1){int x=x0;while(x<x1){int start,n,px[96],py[96];char text[96];while(x<x1&&ZhRobotChar(x,y)==' ')++x;if(x>=x1)break;start=x;n=0;while(x<x1&&n<95){if(ZhRobotChar(x,y)==' '&&x+1<x1&&ZhRobotChar(x+1,y)==' ')break;text[n]=ZhRobotChar(x,y);px[n]=x;py[n]=y;++n;++x;}while(n>0&&text[n-1]==' ')--n;text[n]=0;if(n){int trim=n;while(trim>0){if(ZhRobotDrawSegment(s,start,y,text,trim,px,py))break;while(trim>0&&text[trim-1]!=' ')--trim;while(trim>0&&text[trim-1]==' ')--trim;}}if(x< x1&&ZhRobotChar(x,y)==' ')while(x<x1&&ZhRobotChar(x,y)==' ')++x;}}
static int ZhRobotAt(int x,int y,const char*s){int i;for(i=0;s[i];++i)if(ZhRobotChar(x+i,y)!=s[i])return 0;return 1;}
static int ZhRobotItemPage(SSurf*s){return s&&g_on&&g_grid.w==160&&g_grid.h==45&&g_cellW>0&&g_cellH==2*g_cellW&&s->w>=g_grid.w*g_cellW&&s->h>=g_grid.h*g_cellH&&ZhRobotRowHas(0,"/ D A T A /")&&ZhRobotAt(66,2,"Aluminum Leg")&&ZhRobotAt(52,4,"Overview")&&ZhRobotAt(53,5,"Type")&&ZhRobotAt(53,6,"Slot");}
static void DrawZhRobotReveal(SSurf*s){static const int ys[]={4,5,6,7,8,9,10,11,12,13,15,16,18,19,20,21,22,24,25};size_t i;if(ZhRobotPage(s)){/* Component-field words are masked while an enclosing captured row is incomplete. */ZhRobotPartialField(s,19,53,"Light",12,1);ZhRobotPartialField(s,19,59,"Ion",13,1);ZhRobotPartialField(s,19,63,"Engine",14,1);ZhRobotPartialField(s,20,53,"Compact",4,1);ZhRobotPartialField(s,20,61,"Wheel",6,1);ZhRobotPartialField(s,20,53,"Wheel",31,1);ZhRobotPartialField(s,21,53,"Small",8,1);ZhRobotPartialField(s,21,59,"Storage",9,1);ZhRobotPartialField(s,21,67,"Unit",10,1);ZhRobotPartialField(s,21,53,"Improved",32,1);ZhRobotPartialField(s,21,62,"Terrain",33,1);ZhRobotPartialField(s,21,70,"Scanner",34,1);ZhRobotPartialField(s,22,53,"Tractor",5,1);ZhRobotPartialField(s,22,61,"Beam",7,1);ZhRobotPartialField(s,22,53,"Seismic",35,1);ZhRobotPartialField(s,22,61,"Detector",36,1);for(i=0;i<sizeof ys/sizeof ys[0];++i)ZhRobotScanLine(s,ys[i],50,101);ZhRobotPartialField(s,5,74,"Recycler",1,0);ZhRobotPartialField(s,5,74,"Tunneler",37,0);ZhRobotPartialField(s,6,74,"Medium",11,0);ZhRobotPartialField(s,9,74,"Rolling",2,0);}if(ZhRobotItemPage(s)){ZhRobotScanLine(s,4,50,101);ZhRobotScanLine(s,5,50,101);ZhRobotScanLine(s,6,50,101);ZhRobotScanLine(s,18,50,101);ZhRobotPartialField(s,6,74,"Propulsion",3,0);}}
#endif
