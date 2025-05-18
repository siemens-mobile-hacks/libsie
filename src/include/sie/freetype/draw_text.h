#pragma once

typedef struct {
    int width;
    int v_advance;
    uint16_t break_point;
} SIE_FT_RENDER_LINE;

typedef struct {
    SIE_FT_RENDER_LINE *lines;
    int lines_count;
    int height;
} SIE_FT_RENDER;

SIE_FT_RENDER *Sie_FT_Render(WSHDR *ws, int x, int x2, int font_size);
void Sie_FT_DestroyRender(SIE_FT_RENDER *render);

SIE_FT_RENDER *Sie_FT_DrawRenderedText(SIE_FT_RENDER *render, WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color);
void Sie_FT_DrawText(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color);
