#ifndef __SIE_FREETYPE_H__
#define __SIE_FREETYPE_H__

#include <wchar.h>
#include <swilib.h>

#define SIE_FT_TEXT_ALIGN_LEFT   TEXT_ALIGNLEFT
#define SIE_FT_TEXT_ALIGN_CENTER TEXT_ALIGNMIDDLE
#define SIE_FT_TEXT_ALIGN_RIGHT  TEXT_ALIGNRIGHT
#define SIE_FT_TEXT_VALIGN_TOP    8
#define SIE_FT_TEXT_VALIGN_MIDDLE 16
#define SIE_FT_TEXT_VALIGN_BOTTOM 32

typedef struct {
    WSHDR *ws;
    WSHDR *ws_copy;
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

extern void Sie_FT_Init();
extern void Sie_FT_Destroy();

int Sie_FT_GetMaxHeight(int font_size);
void Sie_FT_GetStringSize(WSHDR *ws, int font_size, unsigned int *w, unsigned int *h);
void Sie_FT_DrawString(WSHDR *ws, int x, int y, int font_size, const char *color);
void Sie_FT_DrawBoundingString(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color);
void Sie_FT_DrawBoundingScrollString(SIE_FT_SCROLL_STRING *ss, GBSTMR *tmr);

#endif
