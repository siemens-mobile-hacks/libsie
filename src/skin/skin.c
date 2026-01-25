#include <swilib.h>
#include "../include/sie/cfg/cfg.h"
#include "../include/sie/skin/skin.h"

SIE_SKIN SKIN = {
    {CFG_LEVEL, "Fonts", 1, 0},
    {CFG_STR_UTF8, "Font regular path", 3, 0},
    "?:\\zbin\\fonts\\RobotoCondensed-Regular.ttf",
    {CFG_LEVEL, "", 0, 0},
};

void Sie_Skin_Load(const char *path) {
    extern SIE_CONFIG CFG;

    SKIN.font_regular_path[0] = BCFG_GetDefaultDisk();
    if (BCFG_LoadConfig(CFG.skin_path, &SKIN, sizeof(SIE_SKIN)) == -1) {
        BCFG_SaveConfig(CFG.skin_path, &SKIN, sizeof(SIE_SKIN));
    }
}
