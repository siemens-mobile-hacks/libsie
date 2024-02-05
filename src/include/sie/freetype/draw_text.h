#ifndef __SIE_FREETYPE_DRAW_TEXT_H__
#define __SIE_FREETYPE_DRAW_TEXT_H__

typedef struct {
    int width;
    int break_point;
} SIE_FT_RENDER_LINE;

typedef struct {
    SIE_FT_RENDER_LINE *lines;
    int height;
} SIE_FT_RENDER;

void Sie_FT_DrawText(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color);

#endif
