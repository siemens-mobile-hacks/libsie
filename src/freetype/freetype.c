#include <ft2build.h>
#include FT_FREETYPE_H
#include "../include/sie/gui/gui.h"
#include "../include/sie/freetype/freetype_cache.h"

#define TMR_MS_SS_START (216 / 1.5)
#define TMR_MS_SS       (216 / 8)

FT_Library *FT_LIBRARY = NULL;
FT_Face *FT_FACE_REGULAR = NULL;
unsigned int FT_CLIENTS_COUNT = 0;

void Sie_FT_Init() {
    if (!FT_LIBRARY) {
        FT_LIBRARY = malloc(sizeof(FT_Library));
        FT_Init_FreeType(FT_LIBRARY);
        if (!FT_FACE_REGULAR) {
            FT_FACE_REGULAR = malloc(sizeof(FT_Face));
            FT_New_Face(*FT_LIBRARY, "0:\\ZBin\\fonts\\Roboto-Regular.ttf",
                        0, FT_FACE_REGULAR);
        }
    }
    FT_CLIENTS_COUNT++;
}

void Sie_FT_Destroy() {
    if (FT_CLIENTS_COUNT) {
        FT_CLIENTS_COUNT--;
    }
    if (!FT_CLIENTS_COUNT) {
        if (FT_LIBRARY) {
            if (FT_FACE_REGULAR) {
                FT_Done_Face(*FT_FACE_REGULAR);
                mfree(FT_FACE_REGULAR);
                FT_FACE_REGULAR = NULL;
            }
            FT_Done_FreeType(*FT_LIBRARY);
            mfree(FT_LIBRARY);
            FT_LIBRARY = NULL;
            Sie_FT_Cache_Destroy();
        }
    }
}

int Sie_FT_GetMaxHeight(int font_size) {
    FT_Face *face = FT_FACE_REGULAR;
    FT_Set_Pixel_Sizes(*face, 0, font_size);
    const int max_ascender = ((*face)->size->metrics.ascender >> 6);
    const int max_descender = abs(((*face)->size->metrics.descender >> 6));
    return max_ascender + max_descender;
}

void Sie_FT_GetStringSize(WSHDR *ws, int font_size, unsigned int *w, unsigned int *h) {
    FT_Face *face = FT_FACE_REGULAR;
    SIE_FT_CACHE *ft_cache = Sie_FT_Cache_GetOrCreate(face, font_size);
    SIE_FT_CC_CACHE *ft_cc_cache = NULL;

    unsigned int width = 0, height = Sie_FT_GetMaxHeight(font_size);
    FT_ULong charcode;
    for (unsigned int i = 0; i < wstrlen(ws); i++) {
        charcode = ws->wsbody[1 + i];
        ft_cc_cache = Sie_FT_Cache_CCGetOrAdd(face, ft_cache, charcode);
        width += ft_cc_cache->h_advance;
    }
    *w = width;
    *h = height;
}

void Sie_FT_DrawString(WSHDR *ws, int x, int y, int font_size, const char *rgb) {
    char _rgb[3] = COLOR_TEXT_PRIMARY;
    FT_Face *face = FT_FACE_REGULAR;
    SIE_FT_CACHE *ft_cache = Sie_FT_Cache_GetOrCreate(face, font_size);
    SIE_FT_CC_CACHE *ft_cc_cache = NULL;

    FT_Set_Pixel_Sizes(*face, 0, font_size);
    if (rgb) {
        memcpy(_rgb, rgb, 3);
    }

    IMGHDR img;
    size_t size;
    FT_ULong charcode;
    for (int i = 0; i < wstrlen(ws); i++) {
        charcode = ws->wsbody[1 + i];
        ft_cc_cache = Sie_FT_Cache_CCGetOrAdd(face, ft_cache, charcode);

        size = ft_cc_cache->img->w * ft_cc_cache->img->h * 4;
        memcpy(&img, ft_cc_cache->img, sizeof(IMGHDR));
        img.bitmap = malloc(size);
        memcpy(img.bitmap, ft_cc_cache->img->bitmap, size);
        for (int j = 0; j < size; j += 4) {
            img.bitmap[j + 0] = _rgb[2]; // b
            img.bitmap[j + 1] = _rgb[1]; // g
            img.bitmap[j + 2] = _rgb[0]; // r
        }
        Sie_GUI_DrawIMGHDR(&img, x, y + ft_cc_cache->y_offset, img.w, img.h);
        mfree(img.bitmap);
        x += ft_cc_cache->h_advance;
    }
}

void Sie_FT_DrawBoundingString(WSHDR *ws, int x, int y, int x2, int y2, int font_size, int attr, const char *rgb) {
    int x_text = x, y_text = y;
    int len = wstrlen(ws);
    unsigned int w = 0, h = 0;
    WSHDR *copy_ws = AllocWS(len);
    wstrcpy(copy_ws, ws);
    if (attr & SIE_FT_TEXT_ALIGN_CENTER) {
        Sie_FT_GetStringSize(copy_ws, font_size, &w, &h);
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
                Sie_FT_GetStringSize(copy_ws, font_size, &w, &h);
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
                Sie_FT_GetStringSize(copy_ws, font_size, &w, &h);
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
    Sie_FT_DrawString(copy_ws, x_text, y_text, font_size, rgb);
    FreeWS(copy_ws);
}

static void DrawBoundingScrollString(GBSTMR *tmr) {
    SIE_FT_SCROLL_STRING *ss = (SIE_FT_SCROLL_STRING*)tmr->param6;
    if (!IsGuiOnTop((int)(ss->gui_id))) {
        return;
    }

    WSHDR *ws = ss->ws_copy;
    unsigned int w, h;
    int tmr_ms = TMR_MS_SS;
    Sie_FT_GetStringSize(ws, ss->font_size, &w, &h);
    if (ss->offset != 0) {
        if (w > ss->x2 - ss->x) {
            wsRemoveChars(ws, 1, 1);
            ss->offset--;
        } else {
            if (ss->offset < 0) {
                ss->offset = 0;
                wstrcpy(ss->ws_copy, ss->ws);
                GBS_StartTimerProc(tmr, TMR_MS_SS_START, DrawBoundingScrollString);
                return;
            }
        }
    } else {
        ss->offset = -1;
        tmr_ms = TMR_MS_SS_START;
    }
    if (ss->OnBeforeDraw) {
        ss->OnBeforeDraw(ss->x, ss->y, ss->x2, ss->y + (int)h);
    }
    Sie_FT_DrawBoundingString(ws, ss->x, ss->y, ss->x2, ss->y2, ss->font_size, ss->attr, ss->rgb);
    GBS_StartTimerProc(tmr, tmr_ms, DrawBoundingScrollString);
}

void Sie_FT_DrawBoundingScrollString(SIE_FT_SCROLL_STRING *ss, GBSTMR *tmr) {
    Sie_FT_DrawBoundingString(ss->ws, ss->x, ss->y, ss->x2, ss->y2, ss->font_size, ss->attr, ss->rgb);
    wstrcpy(ss->ws_copy, ss->ws);

    unsigned int w, h;
    Sie_FT_GetStringSize(ss->ws, ss->font_size, &w, &h);
    if (w > ss->x2 - ss->x) {
        ss->offset = -1;
        tmr->param6 = (unsigned int)ss;
        GBS_StartTimerProc(tmr, TMR_MS_SS_START, DrawBoundingScrollString);
    }
}
