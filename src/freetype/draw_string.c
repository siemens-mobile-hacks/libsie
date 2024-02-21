#include <swilib.h>
#include "../include/sie/gui/gui.h"
#include "../include/sie/freetype/freetype_cache.h"

#define SS_TMR_MS_START (216 / 1.5)
#define SS_TMR_MS       (16)
#define SS_STEP         (4)

extern FT_Face *FT_FACE_REGULAR;
extern IMGHDR *BrushGlyphIMGHDR(IMGHDR *img, const char *color);

static void DrawStr(WSHDR *ws, int x, int y, int x2, int y2, int x_offset, int font_size, const char *color) {
    char rgb[3] = SIE_COLOR_TEXT_PRIMARY;
    FT_Face *face = FT_FACE_REGULAR;
    SIE_FT_CACHE *cache = Sie_FT_Cache_GetOrCreate(face, font_size);
    SIE_FT_GLYPH_CACHE *glyph_cache = NULL;

    FT_Set_Pixel_Sizes(*face, 0, font_size);
    if (color) {
        memcpy(rgb, color, 3);
    }

    int bleed_x = 0;
    int _x_offset = x_offset;
    unsigned int width = 0;
    for (int i = 0; i < wstrlen(ws); i++) {
        unsigned short charcode = ws->wsbody[1 + i];
        glyph_cache = Sie_FT_Cache_GlyphGetOrAdd(face, cache, charcode);
        width += glyph_cache->h_advance;

        if (x_offset) {
            if (x_offset * -1 > width) {
                _x_offset += glyph_cache->h_advance;
                continue;
            } else if (!bleed_x) {
                bleed_x = _x_offset;
            }
        }

        IMGHDR *img = BrushGlyphIMGHDR(glyph_cache->img, rgb);
        const int x_img = x + glyph_cache->h_bearing_x;
        const int y_img = y + glyph_cache->y_offset;
        const int x2_img = x_img + glyph_cache->h_advance;
        int w_img = img->w;
        int h_img = img->h;
        int last = 0;
        if (x2 && x2_img > x2) { // last
            w_img = x2 - (x_img + glyph_cache->h_bearing_x);
            last = 1;
        }
        if (bleed_x < 0) {
            Sie_GUI_DrawBleedIMGHDR(img, x_img, y_img, x_img + img->w, y_img + img->h,
                                    bleed_x * -1, 0);
            x += bleed_x;
            bleed_x = 1; // stop
        } else {
            Sie_GUI_DrawIMGHDR(img, x_img, y_img, w_img, h_img);
        }
        mfree(img->bitmap);
        mfree(img);
        if (last) {
            break;
        }
        x += glyph_cache->h_advance;
    }
}

static void DrawBoundingString(WSHDR *ws, int x, int y, int x2, int y2,
                               int font_size, int attr, const char *color) {
    int x_text = x, y_text = y;
    int len = wstrlen(ws);
    unsigned int w = 0, h = 0;
    WSHDR *copy_ws = AllocWS(len);
    wstrcpy(copy_ws, ws);
    Sie_FT_GetStringSize(copy_ws, font_size, &w, &h);
    if (attr & SIE_FT_TEXT_ALIGN_CENTER) {
        if (w <= x2 - x) {
            x_text = x + (x2 - x - (int)w) / 2;
        } else {
            goto ALIGN_LEFT;
        }
    }
    else if (attr & SIE_FT_TEXT_ALIGN_RIGHT) {
        while (1) {
            if (len <= 1) {
                break;
            } else {
                x_text = x2 - (int)w;
                if (w > x2 -x) {
                    wsRemoveChars(copy_ws, 1, 2);
                    len -= 1;
                } else break;
            }
        }
    }
    else if (attr & SIE_FT_TEXT_ALIGN_LEFT) {
        ALIGN_LEFT:
        x_text = x;
        while (1) {
            if (len <= 1) {
                break;
            } else {
                if (w > x2 - x) {
                    wsRemoveChars(copy_ws, len - 1, len);
                    len -= 1;
                } else break;
            }
        }
    }
    if (attr & SIE_FT_TEXT_VALIGN_MIDDLE) {
        y_text = y + (y2 - y) / 2 - (int)h / 2;
    }
    else if (attr & SIE_FT_TEXT_VALIGN_BOTTOM) {
        y_text = y2 - (int)h;
    }
    DrawStr(copy_ws, x_text, y_text, x2, y2, 0, font_size, color);
    FreeWS(copy_ws);
}

static void DrawBoundingScrollString(GBSTMR *tmr) {
    SIE_FT_SCROLL_STRING *ss = (SIE_FT_SCROLL_STRING*)tmr->param6;
    if (!IsGuiOnTop((int)(ss->gui_id))) {
        return;
    }

    unsigned int w, h;
    Sie_FT_GetStringSize(ss->ws, ss->font_size, &w, &h);
    if (ss->OnBeforeDraw) {
        ss->OnBeforeDraw(ss->x, ss->y, ss->x2, ss->y + (int)h);
    }
    DrawStr(ss->ws, ss->x, ss->y, ss->x2, ss->y2, ss->offset, ss->font_size, ss->color);

    int tmr_ms = SS_TMR_MS_START;
    if (!ss->offset) {
        ss->offset = -SS_STEP;
    } else {
        if (w + ss->offset > ss->x2 - ss->x) {
            ss->offset -= SS_STEP;
            tmr_ms = SS_TMR_MS;
        } else {
            ss->offset = 0;
        }
    }
    GBS_StartTimerProc(tmr, tmr_ms, DrawBoundingScrollString);
}

void Sie_FT_DrawString(WSHDR *ws, int x, int y, int font_size, const char *color) {
    DrawStr(ws, x, y, 0, 0, 0, font_size, color);
}

void Sie_FT_DrawBoundingString(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *color) {
    DrawBoundingString(ws, x, y, x2, y2, font_size, attr, color);
}

void Sie_FT_DrawBoundingScrollString(SIE_FT_SCROLL_STRING *ss, GBSTMR *tmr) {
    unsigned int w, h;
    Sie_FT_GetStringSize(ss->ws, ss->font_size, &w, &h);
    if (w > ss->x2 - ss->x) {
        ss->offset = 0;
        tmr->param6 = (unsigned int)ss;
        DrawBoundingScrollString(tmr);
    } else {
        Sie_FT_DrawBoundingString(ss->ws, ss->x, ss->y, ss->x2, ss->y2, ss->font_size, ss->attr, ss->color);
    }
}
