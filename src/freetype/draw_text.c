#include <swilib.h>
#include <sys/param.h>
#include "../include/sie/gui/gui.h"
#include "../include/sie/freetype/freetype.h"

extern FT_Face *FT_FACE_REGULAR;
extern IMGHDR *BrushGlyphIMGHDR(IMGHDR *img, const char *color);

SIE_FT_RENDER *Sie_FT_Render(WSHDR *ws, int x, int x2, int font_size) {
    FT_Face *face = FT_FACE_REGULAR;
    SIE_FT_CACHE *cache = FT_GetOrCreateCache(face, font_size);
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

    for (uint16_t i = 0; i < wstrlen(ws); i++) {
        const uint16_t charcode = ws->wsbody[1 + i];
        if (charcode == '\r') {
            continue;
        }

        const SIE_FT_GLYPH_CACHE *glyph_cache = FT_GlyphGetOrAddCache(face, cache, charcode);
        max_v_advance = MAX(max_v_advance, glyph_cache->v_advance);
        word_width += glyph_cache->h_advance;
        line_width += glyph_cache->h_advance;
        if (line_width > x2 - x) {
            BREAK_LINE:
                if (line_id > max_lines / 2) {
                    max_lines *= 2;
                    render->lines = realloc(render->lines, sizeof(SIE_FT_RENDER_LINE) * max_lines); // NOLINT
                }

                SIE_FT_RENDER_LINE *line = &(render->lines[line_id++]);
                if (last_space) {
                    line->break_point = last_space + 1;
                    i = last_space;
                } else {
                    line->break_point = i;
                    line_width -= glyph_cache->h_advance;
                    i--;
                }
                // line_width -= word_width;
                line->width = line_width;
                line->v_advance = max_v_advance;
                render->lines_count++;
                render->height += max_v_advance;
                line_width = word_width = last_space = max_v_advance = 0;
        }
        else if (charcode == '\n') {
            last_space = i;
            goto BREAK_LINE;
        }
        else if (charcode == ' ') {
            last_space = i;
        }
    }
    render->lines[line_id].width = word_width;
    render->lines[line_id].v_advance = max_v_advance;
    render->lines[line_id].break_point = -1;
    render->height += max_v_advance;
    return render;
}

SIE_FT_RENDER *Sie_FT_DrawRenderedText(SIE_FT_RENDER *render, WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color) {
    char rgb[] = SIE_COLOR_TEXT_PRIMARY;
    FT_Face *face = FT_FACE_REGULAR;
    SIE_FT_CACHE *ft_cache = FT_GetOrCreateCache(face, font_size);
    FT_Set_Pixel_Sizes(*face, 0, font_size);
    if (color) {
        memcpy(rgb, color, 3);
    }

    if (!render) {
        render = Sie_FT_Render(ws, x, x2, font_size);
    }

    int _x = x;
    int _y = y;
    if (attr & SIE_FT_TEXT_VALIGN_MIDDLE) {
        _y = y + ((y2 - y) - render->height) / 2;
    }
    int line_id = 0;
    int x_offset = 0;
    for (uint16_t i = 0; i < wstrlen(ws); i++) {
        SIE_FT_RENDER_LINE *line = (render->lines) ? &(render->lines[line_id]) : NULL;
        if (!line || _y > y2) {
            break;
        } else if (_y <= 0 - line->v_advance) {
            i = line->break_point;
            goto BREAK_POINT;
        }
        if (line->break_point == i) {
            BREAK_POINT:
                _x = x;
                _y += line->v_advance;
                line_id++;
                i--;
                continue;
        }
        if (attr & SIE_FT_TEXT_ALIGN_CENTER) {
            x_offset = ((x2 - x) - line->width) / 2;
        }

        const uint16_t charcode = ws->wsbody[1 + i];
        if (charcode != '\n') {
            SIE_FT_GLYPH_CACHE *glyph_cache = FT_GlyphGetOrAddCache(face, ft_cache, charcode);
            const int y_img = _y + glyph_cache->y_offset;
            const int x_img = _x + glyph_cache->h_bearing_x + x_offset;
            IMGHDR *img = BrushGlyphIMGHDR(glyph_cache->img, rgb);
            DrawIMGHDR(x_img, y_img, img);
            mfree(img->bitmap);
            mfree(img);
            _x += glyph_cache->h_advance;
        }
    }
    return render;
}

void Sie_FT_DestroyRender(SIE_FT_RENDER *render) {
    mfree(render->lines);
    mfree(render);
}

void Sie_FT_DrawText(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color) {
    SIE_FT_RENDER *render = Sie_FT_DrawRenderedText(NULL, ws, x, y, x2, y2, font_size, attr, color);
    Sie_FT_DestroyRender(render);
}
