/* Translate only dictionary-resolved names in the native spaced tooltip frame. */
static void PlanZhNameTooltips(void)
{
    int i;
    for (i = 0; i < g_nrun; ++i) {
        Run *r = &g_runs[i];
        char name[MAX_TEXT], text[MAX_TEXT];
        int n = r->len;
        if (r->used || n < 6 || n >= MAX_TEXT ||
            r->t[0] != '[' || r->t[1] != ' ' ||
            r->t[n-2] != ' ' || r->t[n-1] != ']') continue;
        if (!CapXlat(r->t + 2, n - 4, name, sizeof name)) continue;
        if (strlen(name) + 5 > sizeof text) continue;
        snprintf(text, sizeof text, "[ %s ]", name);
        if (cm_cells(text) > n * r->f) continue;
        AddOpF(r, r->x0, n, text, r->fr, r->fg, r->fb);
        r->used = 1;
    }
}
