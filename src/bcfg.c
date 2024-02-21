#include "include/sie/config_loader.h"

char BCFG_PATH[64] = "0:\\zbin\\etc\\SieLibrary.bcfg";

void Sie_BCFG_Load() {
    __CONFIG_EXTERN(0, cfghdr_fonts_b);
    if (Sie_Config_Load(BCFG_PATH, __CONFIG(0, cfghdr_fonts_b))) {
        return;
    }
    BCFG_PATH[0] = '4';
    if (Sie_Config_Load(BCFG_PATH, __CONFIG(0, cfghdr_fonts_b))) {
        return;
    }
    BCFG_PATH[0] = '\0';
}

const char *Sie_BCFG_GetPath() {
    return BCFG_PATH;
}
