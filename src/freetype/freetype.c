#include <swilib.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../include/sie/gui/gui.h"
#include "../include/sie/freetype/freetype_cache.h"

extern char CFG_FONT_REGULAR_PATH[];

FT_Library *FT_LIBRARY;
FT_Face *FT_FACE_REGULAR;
unsigned int FT_CLIENTS_COUNT;

void Sie_FT_Init() {
    if (!FT_LIBRARY) {
        FT_LIBRARY = malloc(sizeof(FT_Library));
        FT_Init_FreeType(FT_LIBRARY);
        Sie_FT_LoadFaces();
    }
    FT_CLIENTS_COUNT++;
}

void Sie_FT_Destroy() {
    if (FT_CLIENTS_COUNT) {
        FT_CLIENTS_COUNT--;
    }
    if (!FT_CLIENTS_COUNT) {
        if (FT_LIBRARY) {
            Sie_FT_UnloadFaces();
            FT_Done_FreeType(*FT_LIBRARY);
            mfree(FT_LIBRARY);
            FT_LIBRARY = NULL;
            Sie_FT_Cache_Destroy();
        }
    }
}

void Sie_FT_UnloadFaces() {
    if (FT_FACE_REGULAR) {
        FT_Done_Face(*FT_FACE_REGULAR);
        mfree(FT_FACE_REGULAR);
        FT_FACE_REGULAR = NULL;
    }
}

void Sie_FT_LoadFaces() {
    FT_FACE_REGULAR = malloc(sizeof(FT_Face));
    FT_New_Face(*FT_LIBRARY, CFG_FONT_REGULAR_PATH,
                SIE_FT_FACE_REGULAR_ID, FT_FACE_REGULAR);
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
    SIE_FT_CACHE *cache = Sie_FT_Cache_GetOrCreate(face, font_size);
    SIE_FT_GLYPH_CACHE *glyph_cache = NULL;

    unsigned int width = 0, height = Sie_FT_GetMaxHeight(font_size);
    FT_ULong charcode;
    for (unsigned int i = 0; i < wstrlen(ws); i++) {
        charcode = ws->wsbody[1 + i];
        glyph_cache = Sie_FT_Cache_GlyphGetOrAdd(face, cache, charcode);
        width += glyph_cache->h_advance;
    }
    *w = width;
    *h = height;
}

IMGHDR *BrushGlyphIMGHDR(IMGHDR *img, const char *color) {
    IMGHDR *res = malloc(sizeof(IMGHDR));
    memcpy(res, img, sizeof(IMGHDR));
    size_t size = CalcBitmapSize((short)res->w, (short)res->h, res->bpnum);
    res->bitmap = malloc(size);
    memcpy(res->bitmap, img->bitmap, size);
    for (int i = 0; i < size; i += 4) {
        res->bitmap[i + 0] = color[2];
        res->bitmap[i + 1] = color[1];
        res->bitmap[i + 2] = color[0];
    }
    return res;
}
