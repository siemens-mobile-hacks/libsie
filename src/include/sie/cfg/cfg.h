#pragma once
#include <cfg_items.h>

#pragma pack(push, 1)
typedef struct {
    const CFG_HDR cfghdr_gui_b;
    const CFG_HDR cfghdr_gui_0;
    char skin_path[128];
    const CFG_HDR cfghdr_gui_e;
} SIE_CONFIG;
#pragma pack(pop)

void Sie_CFG_Load();
