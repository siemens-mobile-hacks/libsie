#include "../include/sie/freetype/freetype_cache.h"

SIE_FT_CACHE *FT_CACHE;
unsigned int FT_CACHE_SIZE;

SIE_FT_CACHE *Sie_FT_Cache_GetOrCreate(const FT_Face *face, int font_size) {
    SIE_FT_CACHE *ft_cache = NULL;
    for (unsigned int i = 0; i < FT_CACHE_SIZE; i++) {
        if (FT_CACHE[i].face == face && FT_CACHE[i].font_size == font_size) {
            ft_cache = &(FT_CACHE[i]);
        }
    }
    if (ft_cache) {
        return ft_cache;
    } else {
        FT_CACHE = realloc(FT_CACHE, sizeof(SIE_FT_CACHE) * (FT_CACHE_SIZE + 1));
        ft_cache = &(FT_CACHE[FT_CACHE_SIZE]);
        ft_cache->face = (FT_Face *)face;
        ft_cache->font_size = font_size;
        ft_cache->size = 0;
        ft_cache->cache = NULL;
        FT_CACHE_SIZE++;
        return ft_cache;
    }
}

void Sie_FT_Cache_Destroy() {
    for (int i = 0; i < FT_CACHE_SIZE; i++) {
        SIE_FT_CACHE ft_cache = FT_CACHE[i];
        for (unsigned int j = 0; j < ft_cache.size; j++) {
            SIE_FT_CC_CACHE ft_wc_cache = ft_cache.cache[j];
            mfree(ft_wc_cache.img->bitmap);
            mfree(ft_wc_cache.img);
        }
        mfree(ft_cache.cache);
    }
    mfree(FT_CACHE);
    FT_CACHE = NULL;
    FT_CACHE_SIZE = 0;
}

SIE_FT_CC_CACHE *Sie_FT_Cache_CCGet(SIE_FT_CACHE *ft_cache, FT_ULong charcode) {
    SIE_FT_CC_CACHE *ft_cc_cache = NULL;
    for (unsigned int i = 0; i < ft_cache->size; i++) {
        ft_cc_cache = &(ft_cache->cache[i]);
        if (ft_cc_cache->charcode == charcode) {
            return ft_cc_cache;
        }
    }
    return NULL;
}

SIE_FT_CC_CACHE *Sie_FT_Cache_CCAdd(FT_Face *face, SIE_FT_CACHE *ft_cache, FT_ULong charcode) {
    FT_UInt glyph_index;
    FT_Bitmap *bitmap;

    glyph_index = FT_Get_Char_Index(*face, charcode);
    FT_Load_Glyph((*face), glyph_index, FT_LOAD_DEFAULT);
    FT_Render_Glyph((*face)->glyph, FT_RENDER_MODE_NORMAL);
    bitmap = &((*face)->glyph->bitmap);

    IMGHDR *img = malloc(sizeof(IMGHDR));
    img->w = bitmap->width;
    img->h = bitmap->rows;
    img->bpnum = 0x0A;
    img->bitmap = malloc(img->w * img->h * 4);

    uint8_t im[img->h][img->w];
    for (int _y = 0, j = 0; _y < img->h; _y++) {
        for (int _x = 0; _x < img->w; _x++, j++) {
            im[_y][_x] = bitmap->buffer[j];
        }
    }
    for (int j = 0, _y = 0; _y < img->h; _y++) {
        for (int _x = 0; _x < img->w; _x++, j++) {
            img->bitmap[4 * j + 3] = im[_y][_x];
        }
    }

    int h_advance = 0;
    int h_bearing_x = 0;
    int y_offset = 0;
    const int max_ascender = ((*face)->size->metrics.ascender >> 6);
    h_advance = ((*face)->glyph->metrics.horiAdvance >> 6);
    h_bearing_x = ((*face)->glyph->metrics.horiBearingX >> 6);
    y_offset = -((*face)->glyph->bitmap_top) + max_ascender;

    ft_cache->cache = realloc(ft_cache->cache, sizeof(SIE_FT_CC_CACHE) * (ft_cache->size + 1));
    SIE_FT_CC_CACHE *cc_cache = &(ft_cache->cache[ft_cache->size]);
    zeromem(cc_cache, sizeof(SIE_FT_CC_CACHE));
    cc_cache->charcode = charcode;
    cc_cache->img = img;
    cc_cache->y_offset = y_offset;
    cc_cache->h_advance = h_advance;
    cc_cache->h_bearing_x = h_bearing_x;
    ft_cache->size++;
    return cc_cache;
}

SIE_FT_CC_CACHE *Sie_FT_Cache_CCGetOrAdd(FT_Face *face, SIE_FT_CACHE *ft_cache, FT_ULong charcode) {
    SIE_FT_CC_CACHE *cc_cache = NULL;
    cc_cache = Sie_FT_Cache_CCGet(ft_cache, charcode);
    if (!cc_cache) {
        cc_cache = Sie_FT_Cache_CCAdd(face, ft_cache, charcode);
    }
    return cc_cache;
}
