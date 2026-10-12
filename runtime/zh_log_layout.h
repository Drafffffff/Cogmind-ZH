/* Narrow guard for reflowing one native HUD log paragraph.  idx contains
   row-major grid offsets of the first non-space cell on each candidate row.
   The caller must already have established that these rows matched the same
   complete source key (including key0); this helper only validates geometry
   and native presentation evidence. */
#ifndef ZH_LOG_LAYOUT_H
#define ZH_LOG_LAYOUT_H

static int ZhLogBlockCanReflow(const int *idx, int nb)
{
    int i, x0, y0;
    if (!idx || nb < 2 || nb > 8 || g_grid.w != 160 || g_grid.h != 45)
        return 0;

    /* Identify the left log itself. The adjacent panel may be COMBAT,
       item/robot DATA, or another overlay; its heading is not this log's
       identity. Full paragraph matching and continuation geometry below
       are still mandatory before any reflow. */
    {
        int x, log = 0;
        for (x = 0; x + 6 < 49; ++x)
            if (ChAt(x,0)=='/' && ChAt(x+2,0)=='L' && ChAt(x+4,0)=='O' &&
                ChAt(x+6,0)=='G') log = 1;
        if (!log) return 0;
    }

    x0 = idx[0] % g_grid.w;
    y0 = idx[0] / g_grid.w;
    if (y0 < 1 || y0 > 7 || x0 != 1) return 0;

    {
        BYTE cr, cg, cb;
        const XCell *first_cell = &CELL(x0,y0);
        if (ChAt(x0,y0)==' ' || ChAt(x0,y0)==0) return 0;
        cr=first_cell->fgR;cg=first_cell->fgG;cb=first_cell->fgB;
        for (i = 0; i < nb; ++i) {
            int y = y0 + i, x, first = -1, last = -1;
            if (y > 8 || idx[i] / g_grid.w != y) return 0;
            for (x = 1; x < 49; ++x) {
                const XCell *c = &CELL(x,y);
                int glyph = ChAt(x,y);
                if (glyph != ' ' && glyph != 0) {
                    if (c->fgR != cr || c->fgG != cg || c->fgB != cb) return 0;
                    if (first < 0) first = x;
                    last = x;
                }
            }
            if (first != (i == 0 ? 1 : 3) || idx[i] != y*g_grid.w+first || last < first)
                return 0;
            /* A fresh left-aligned message inside the block is not a continuation. */
            if (i > 0 && ChAt(1,y) != ' ' && ChAt(1,y) != 0) return 0;
        }
    }
    return 1;
}

/* The planner should pass this guard only for an already matched complete
   paragraph candidate. f/vs are the common native run dimensions; same_key
   means the merged candidate key and key0 are byte-identical. */
static int ZhLogBlockCanReflowCandidate(const int *idx, int nb,
                                        int f, int vs, int same_key)
{
    return f == 1 && vs == 1 && same_key && ZhLogBlockCanReflow(idx, nb);
}

#endif
