#include "../include/sie/config_loader.h"

void Sie_Skin_Load(const char *path) {
    __CONFIG_EXTERN(1, skinhdr_fonts_b);
    Sie_Config_Load(path, __CONFIG(1, skinhdr_fonts_b));
}
