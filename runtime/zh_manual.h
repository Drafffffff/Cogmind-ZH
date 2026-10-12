/* Dedicated decoder for the generated native manual provider. */
#ifndef ZH_MANUAL_H
#define ZH_MANUAL_H
#include <stddef.h>
#include <string.h>
#include "zh_manual_data.h"

/* Decode only an exact generated row. Never reinterpret arbitrary game text. */
static const char *ZhManualDecode(const char *source, size_t length)
{
    size_t i, id = 0;
    if (length < 9 || source[0] != 'Z' || source[1] != 'H' ||
        (source[2] != 'm' && source[2] != 'M')) return NULL;
    for (i = 3; i < 9; ++i) {
        unsigned char c = (unsigned char)source[i];
        unsigned digit;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else return NULL;
        id = (id << 4) | digit;
    }
    if (!id || id > ZH_MANUAL_COUNT) return NULL;
    --id;
    if (strlen(zh_manual_rows[id].token) != length ||
        memcmp(source + 3, zh_manual_rows[id].token + 3, length - 3)) return NULL;
    return zh_manual_rows[id].text;
}
#endif
