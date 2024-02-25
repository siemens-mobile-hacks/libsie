#include <cfg_items.h>

__CFG_BEGIN(0)

__root CFG_HDR cfghdr_0 = {CFG_UINT, "Icons cache size", 5, 200};
__root int CFG_ICONS_CACHE_SIZE = 50;

__root CFG_HDR cfghdr_gui_b = {CFG_LEVEL, "GUI", 1, 0};

__root CFG_HDR cfghdr_gui_0 = {CFG_STR_UTF8, "Skin path", 3, 127};
__root char CFG_SKIN_PATH[128] = "0:\\Skins\\default.skin";

__root CFG_HDR cfghdr_gui_e = {CFG_LEVEL, "", 0, 0};

__CFG_END(0)
