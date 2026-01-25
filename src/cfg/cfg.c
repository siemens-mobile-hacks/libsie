#include <swilib.h>
#include <stdio.h>
#include "../include/sie/fs/fs.h"
#include "../include/sie/cfg/cfg.h"
#include "../include/sie/skin/skin.h"

SIE_CONFIG SIE_CFG = {
    {CFG_LEVEL, "GUI", 1, 0},
    {CFG_STR_UTF8, "Skin path", 3, 127},
    "0:\\Skins\\default.skin",
    {CFG_LEVEL, "", 0, 0}
};

void Sie_CFG_Load() {
    char path[128];
    sprintf(path, "%c:\\zbin\\etc\\SieLibrary.bcfg", BCFG_GetDefaultDisk());
    if (BCFG_LoadConfig(path, &SIE_CFG, sizeof(SIE_CONFIG)  == -1)) {
        BCFG_SaveConfig(path, &SIE_CFG, sizeof(SIE_CONFIG));
    }
    unsigned int err;
    Sie_FS_CreateDirs("0:\\Skins\\", &err);
    Sie_Skin_Load(SIE_CFG.skin_path);
}
