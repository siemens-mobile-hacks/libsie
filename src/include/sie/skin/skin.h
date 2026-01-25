#pragma once

#include <cfg_items.h>

#pragma pack(push, 1)
typedef struct {
    const CFG_HDR cfghdr_fonts_b;
    const CFG_HDR cfghdr_fonts_0;
    char font_regular_path[128];
    const CFG_HDR cfghdr_fonts_e;
} SIE_SKIN;
#pragma pack(pop)

void Sie_Skin_Load(const char *path);
