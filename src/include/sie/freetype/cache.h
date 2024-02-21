#ifndef __CACHE_H__
#define __CACHE_H__

#include <swilib.h>
#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    FT_ULong charcode;
    IMGHDR *img;
    int y_offset;
    int h_advance;
    int v_advance;
    int h_bearing_x;
} SIE_FT_GLYPH_CACHE;

typedef struct {
    FT_Face *face;
    int font_size;
    SIE_FT_GLYPH_CACHE *cache;
    unsigned int size;
} SIE_FT_CACHE;

SIE_FT_CACHE *FT_GetOrCreateCache(const FT_Face *face, int font_size);
SIE_FT_GLYPH_CACHE *FT_GlyphGetOrAddCache(FT_Face *face, SIE_FT_CACHE *cache, FT_ULong charcode);

#endif