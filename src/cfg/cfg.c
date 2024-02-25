#include "../include/sie/config_loader.h"
#include "../include/sie/fs/fs.h"
#include "../include/sie/skin/skin.h"

extern char CFG_SKIN_PATH[];

char CFG_PATH[64] = "0:\\zbin\\etc\\SieLibrary.bcfg";

void Sie_CFG_Load() {
    __CONFIG_EXTERN(0, cfghdr_0);
    if (!Sie_Config_Load(CFG_PATH, __CONFIG(0, cfghdr_0))) {
        CFG_PATH[0] = '4';
        if (!Sie_Config_Load(CFG_PATH, __CONFIG(0, cfghdr_0))) {
            CFG_PATH[0] = '\0';
            return;
        }
    }

    unsigned int err;
    Sie_FS_CreateDirs("0:\\Skins\\", &err);
    Sie_Skin_Load(CFG_SKIN_PATH);
}

const char *Sie_CFG_GetPath() {
    return CFG_PATH;
}
