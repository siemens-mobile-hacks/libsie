#ifndef __SIE_FREETYPE_H__
#define __SIE_FREETYPE_H__

#include <wchar.h>
#include <swilib.h>
#include "draw_text.h"
#include "draw_string.h"

#define SIE_FT_FACE_REGULAR_ID 0x00

#define SIE_FT_TEXT_ALIGN_LEFT   TEXT_ALIGNLEFT
#define SIE_FT_TEXT_ALIGN_CENTER TEXT_ALIGNMIDDLE
#define SIE_FT_TEXT_ALIGN_RIGHT  TEXT_ALIGNRIGHT
#define SIE_FT_TEXT_VALIGN_TOP    8
#define SIE_FT_TEXT_VALIGN_MIDDLE 16
#define SIE_FT_TEXT_VALIGN_BOTTOM 32

typedef struct {
    WSHDR *ws;
    int offset;
    int x, y;
    int x2, y2;
    int font_size;
    int attr;
    char *color;
    //
    GBSTMR tmr;
    unsigned int gui_id;
    void (*OnBeforeDraw)(int x, int y, int x2, int y2);
} SIE_FT_SCROLL_STRING;

void Sie_FT_UnloadFaces();
void Sie_FT_LoadFaces();
void Sie_FT_DestroyCache();

int Sie_FT_GetMaxHeight(int font_size);
void Sie_FT_GetStringSize(WSHDR *ws, int font_size, unsigned int *w, unsigned int *h);

void Sie_FT_DrawBoundingString(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color);
void Sie_FT_DrawBoundingScrollString(SIE_FT_SCROLL_STRING *ss, GBSTMR *tmr);

#endif
