/* Bounded composition for a captured robot-model + part name.
   Include after DictEnt, g_dict, DICT_SIZE, DictHash and cm_cells are defined.
   This deliberately handles only a known model prefix followed by one exact
   dictionary name. Unknown text is never copied into a successful result. */
#ifndef ZH_CAPTURE_NAMES_H
#define ZH_CAPTURE_NAMES_H

#define ZH_CAPTURE_NAME_MAX 256

static const DictEnt *ZhCaptureDictFind(const char *s, int n)
{
    unsigned h;
    int slot, probes;
    if (!s || n <= 0 || !g_ndict) return 0;
    h = DictHash(s, n);
    slot = (int)(h & (DICT_SIZE - 1));
    for (probes = 0; probes < DICT_SIZE && g_dict[slot].k; ++probes) {
        if (g_dict[slot].klen == n && memcmp(g_dict[slot].k, s, (size_t)n) == 0)
            return &g_dict[slot];
        slot = (slot + 1) & (DICT_SIZE - 1);
    }
    return 0;
}

static int ZhCaptureModelPrefix(const char *s, int n, int *prefixLen)
{
    int i = 0, letters = 0, digits = 0;
    if (!s || !prefixLen) return 0;
    while (i < n && ((s[i] >= 'A' && s[i] <= 'Z') ||
                     (s[i] >= 'a' && s[i] <= 'z'))) { ++i; ++letters; }
    if (letters < 1 || letters > 3 || i >= n || s[i++] != '-') return 0;
    while (i < n && s[i] >= '0' && s[i] <= '9') { ++i; ++digits; }
    if (digits < 2 || digits > 3 || i >= n || s[i] != ' ') return 0;
    *prefixLen = i; /* length before the required separating space */
    return 1;
}

static int ZhCaptureKnownModelPrefix(const char *s, int prefixLen)
{
    int i;
    /* The translation tables can be hot-reloaded, so consult the live
       dictionary rather than retaining a prefix cache across BuildDict. */
    for (i = 0; i < DICT_SIZE; ++i) {
        const DictEnt *e = &g_dict[i];
        if (e->k && e->klen > prefixLen + 1 &&
            memcmp(e->k, s, (size_t)prefixLen) == 0 &&
            e->k[prefixLen] == ' ' && e->k[prefixLen + 1] != ' ') return 1;
    }
    return 0;
}

static int ZhCaptureNameComposite(const char *s, int n, int budget,
                                  char *out, int cap)
{
    int prefixLen, suffixLen, valueLen;
    const DictEnt *suffix;
    if (!s || !out || cap <= 0) return 0;
    out[0] = 0;
    if (n <= 0 || n > ZH_CAPTURE_NAME_MAX || budget < 0 || !g_ndict ||
        !ZhCaptureModelPrefix(s, n, &prefixLen) ||
        !ZhCaptureKnownModelPrefix(s, prefixLen)) return 0;
    suffixLen = n - prefixLen - 1;
    suffix = ZhCaptureDictFind(s + prefixLen + 1, suffixLen);
    if (!suffix || suffix->vlen <= 0 || suffix->vlen >= cap) return 0;

    /* Keep the model identifier verbatim (it is known from a translated robot
       name); translate the complete suffix from the dictionary. */
    valueLen = prefixLen + 1 + suffix->vlen; /* model + separator + suffix */
    if (valueLen >= cap) return 0;
    memcpy(out, s, (size_t)prefixLen);
    out[prefixLen] = ' ';
    memcpy(out + prefixLen + 1, suffix->v, (size_t)suffix->vlen);
    out[valueLen] = 0;
    if (cm_cells(out) > budget) { out[0] = 0; return 0; }
    return 1;
}

#endif
