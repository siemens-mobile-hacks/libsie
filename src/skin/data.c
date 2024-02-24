#include <cfg_items.h>

__CFG_BEGIN(1)

__root CFG_HDR skinhdr_fonts_b = {CFG_LEVEL, "Fonts", 1, 0};

__root CFG_HDR skinhdr_fonts_0 = {CFG_STR_UTF8, "Font regular path", 3, 127};
__root char SKIN_FONT_REGULAR_PATH[128] = "0:\\zbin\\fonts\\RobotoCondensed-Regular.ttf";

__root CFG_HDR skinghdr_fonts_size_b = {CFG_LEVEL, "Fonts size", 1, 0};

__root CFG_HDR skinhdr_fonts_size_0 = {CFG_UINT, "Iconbar", 13, 30};
__root int SKIN_FONT_SIZE_ICONBAR = 15;

__root CFG_HDR skinhdr_fonts_size_1 = {CFG_UINT, "Header", 13, 30};
__root int SKIN_FONT_SIZE_HEADER = 22;

__root CFG_HDR skinhdr_fonts_size_2 = {CFG_UINT, "Menu list item", 13, 30};
__root int SKIN_FONT_SIZE_MENU_LIST_ITEM = 20;

__root CFG_HDR skinhdr_font_size_10 = {CFG_UINT, "Msg box", 13, 30};
__root int SKIN_FONT_SIZE_MSG_BOX = 20;

__root CFG_HDR skinhdr_font_size_11 = {CFG_UINT, "Msg box softkeys", 13, 30};
__root int SKIN_FONT_SIZE_MSG_BOX_SOFT_KEYS = 20;

__root CFG_HDR skinhdr_fonts_size_e = {CFG_LEVEL, "", 0, 0};

__root CFG_HDR skinhdr_fonts_e = {CFG_LEVEL, "", 0, 0};

__CFG_END(1)
