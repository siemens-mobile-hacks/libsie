#include "include/sie/cfg/cfg.h"

__attribute__((constructor))
void InitLibrary() {
    extern void Sie_FT_Init();
    extern void Sie_SubProc_Init();
    extern void Sie_Resources_Init();
    Sie_CFG_Load();
    Sie_FT_Init();
    Sie_SubProc_Init();
    Sie_Resources_Init();
}

__attribute__((destructor))
void DestroyLibrary() {
    extern void Sie_FT_Destroy();
    extern void Sie_SubProc_Destroy();
    extern void Sie_Resources_Destroy();
    Sie_FT_Destroy();
    Sie_SubProc_Destroy();
    Sie_Resources_Destroy();
}
