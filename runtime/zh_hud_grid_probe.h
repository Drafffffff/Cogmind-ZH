/* Bounded read-only snapshots for correlating event sources with visible HUD rows. */
static unsigned zh_hud_event_count;
static unsigned zh_hud_grid_frames;
static void ObserveZhHudGrid(void)
{
    int x,y;
    if(zh_hud_event_count<2 || zh_hud_grid_frames>=24 || !ZhIntroGameHud()) return;
    ++zh_hud_grid_frames;
    for(y=2;y<8 && y<g_grid.h;++y) {
        char row[49];
        XCell *c=&CELL(1,y);
        for(x=1;x<49 && x<g_grid.w;++x) {
            char ch=ChAt(x,y);row[x-1]=ch>=32 && ch<=126?ch:'?';
        }
        row[x-1]=0;
        L("[zh-hud-grid] frame=%u row=%d fg=%u,%u,%u bg=%u,%u,%u raw=%s",
          zh_hud_grid_frames,y,c->fgR,c->fgG,c->fgB,c->bgR,c->bgG,c->bgB,row);
    }
}
