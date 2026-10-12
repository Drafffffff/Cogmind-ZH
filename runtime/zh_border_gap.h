/* Border-title extension needs horizontal endpoints, not arbitrary box cells.
   Observed native atlas: vertical128, horizontal129, top corners136/137. */
#ifndef ZH_BORDER_GAP_H
#define ZH_BORDER_GAP_H
static int ZhBorderGapAllowed(int left,int row,int right)
{
    unsigned a,b;
    if(left<0||right>=g_grid.w||left>=right||row<0||row>=g_grid.h)return 0;
    a=CELL(left,row).glyph;b=CELL(right,row).glyph;
    return (a==129||a==136)&&(b==129||b==137);
}
#endif
