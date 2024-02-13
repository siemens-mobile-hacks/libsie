#include <swilib.h>
#include <sys/param.h>
#include "../include/sie/gui/gui.h"
#include "../include/sie/freetype/freetype_cache.h"

extern FT_Face *FT_FACE_REGULAR;
extern IMGHDR *BrushGlyphIMGHDR(IMGHDR *img, const char *color);

static SIE_FT_RENDER *Render(WSHDR *ws, int x, int x2, int font_size) {
    FT_Face *face = FT_FACE_REGULAR;
    SIE_FT_CACHE *cache = Sie_FT_Cache_GetOrCreate(face, font_size);
    FT_Set_Pixel_Sizes(*face, 0, font_size);

    int line_id = 0;
    int max_lines = 8;
    int line_width = 0;
    int word_width = 0;
    int last_space = 0;
    int max_v_advance = 0;

    SIE_FT_RENDER *render = malloc(sizeof(SIE_FT_RENDER));
    zeromem(render, sizeof(SIE_FT_RENDER));
    render->lines = malloc(sizeof(SIE_FT_RENDER_LINE) * max_lines);
    for (int i = 0; i < wstrlen(ws); i++) {
        unsigned short charcode = ws->wsbody[1 + i];
        SIE_FT_GLYPH_CACHE *glyph_cache = Sie_FT_Cache_GlyphGetOrAdd(face, cache, charcode);
        word_width += glyph_cache->h_advance;
        if (word_width > x2 - x) { // break line
            if (line_id > max_lines / 2) {
                max_lines *= 2;
                render->lines = realloc(render->lines, sizeof(SIE_FT_RENDER_LINE) * max_lines);
            }
            SIE_FT_RENDER_LINE *line = &(render->lines[line_id++]);
            if (last_space) {
                line->break_point = last_space + 1;
                i = last_space;
                last_space = 0;
            } else {
                line->break_point = i;
                line_width = word_width - glyph_cache->h_advance;
                i--;
            }
            line->width = line_width;
            render->height += max_v_advance;
            line_width = word_width = max_v_advance = 0;
        } else if (charcode == ' ') {
            last_space = i;
            line_width = word_width;
        } else {
            max_v_advance = MAX(max_v_advance, glyph_cache->v_advance);
        }
    }
    render->lines[line_id].width = word_width;
    render->lines[line_id].break_point = -1;
    render->height += max_v_advance;
    return render;
}

SIE_FT_RENDER *DrawText(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color) {
    char rgb[] = SIE_COLOR_TEXT_PRIMARY;
    FT_Face *face = FT_FACE_REGULAR;
    SIE_FT_CACHE *ft_cache = Sie_FT_Cache_GetOrCreate(face, font_size);
    FT_Set_Pixel_Sizes(*face, 0, font_size);
    if (color) {
        memcpy(rgb, color, 3);
    }

    SIE_FT_RENDER *render = Render(ws, x, x2, font_size);

    int _x = x;
    int _y = y;
    if (attr & SIE_FT_TEXT_VALIGN_MIDDLE) {
        _y = y + ((y2 - y) - render->height) / 2;
    }
    int line_id = 0;
    int x_offset = 0;
    int max_v_advance = 0;
    for (int i = 0; i < wstrlen(ws); i++) {
        unsigned short charcode = ws->wsbody[1 + i];
        SIE_FT_GLYPH_CACHE *glyph_cache = Sie_FT_Cache_GlyphGetOrAdd(face, ft_cache, charcode);
        max_v_advance = MAX(max_v_advance, glyph_cache->v_advance);

        SIE_FT_RENDER_LINE *line = (render->lines) ? &(render->lines[line_id]) : NULL;
        if (line) {
            if (attr & SIE_FT_TEXT_ALIGN_CENTER) {
                x_offset = ((x2 - x) - line->width) / 2;
            }
            if (line->break_point == i) {
                _x = x;
                _y += max_v_advance;
                max_v_advance = 0;
                line_id++;
                i--;
                continue;
            }
        }
        const int x_img = _x + glyph_cache->h_bearing_x + x_offset;
        const int y_img = _y + glyph_cache->y_offset;
        IMGHDR *img = BrushGlyphIMGHDR(glyph_cache->img, rgb);
        Sie_GUI_DrawIMGHDR(img, x_img, y_img, img->w, img->h);
        mfree(img->bitmap);
        mfree(img);
        _x += glyph_cache->h_advance;
    }
    return render;
}

void Sie_FT_DestroyRender(SIE_FT_RENDER *render) {
    mfree(render->lines);
    mfree(render);
}

void Sie_FT_DrawText(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color) {
    SIE_FT_RENDER *render = DrawText(ws, x, y, x2, y2, font_size, attr, color);
    Sie_FT_DestroyRender(render);
}
