#ifndef __SIE_FREETYPE_CACHE_H__
#define __SIE_FREETYPE_CACHE_H__

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

SIE_FT_CACHE *Sie_FT_Cache_GetOrCreate(const FT_Face *face, int font_size);
void Sie_FT_Cache_Destroy();

SIE_FT_GLYPH_CACHE *Sie_FT_Cache_GlyphGet(SIE_FT_CACHE *ft_cache, FT_ULong charcode);
SIE_FT_GLYPH_CACHE *Sie_FT_Cache_GlyphAdd(FT_Face *face, SIE_FT_CACHE *ft_cache, FT_ULong charcode);
SIE_FT_GLYPH_CACHE *Sie_FT_Cache_GlyphGetOrAdd(FT_Face *face, SIE_FT_CACHE *cache, FT_ULong charcode);

#endif