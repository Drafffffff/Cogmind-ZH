/* Included after Run/AddOpF definitions by the project-owned build adapter. */
#include "zh_manual.h"
static void PlanZhManual(void)
{
    int i;
    for (i = 0; i < g_nrun; ++i) {
        Run *run = &g_runs[i];
        const char *text;
        int span, tail;
        if (run->used || run->len <= 0) continue;
        text = ZhManualDecode(run->t, (size_t)run->len);
        span = run->len;
        /* Native subheadings uppercase the label and append an underline.
           Decode its complete provider prefix, leaving native decoration drawn. */
        if (!text && run->len >= 9) {
            span = 9;
            while (span < run->len && run->t[span] == 'X') ++span;
            text = ZhManualDecode(run->t, (size_t)span);
            for (tail = span; text && tail < run->len; ++tail)
                if (run->t[tail] != '-' && run->t[tail] != ' ' && run->t[tail] != '=') text = NULL;
        }
        if (!text || cm_cells(text) > span) continue;
        AddOpF(run, run->x0, span, text, run->fr, run->fg, run->fb);
        run->used = 1;
    }
}
