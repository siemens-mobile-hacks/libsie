#include <swilib.h>
#include <swilib/nucleus.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/sie/subproc.h"
#include "include/sie/resources.h"

MUTEX mtx_res_img;
unsigned int CLIENTS;

int RES_IMG_COUNT;
IMGHDR *IMG_WALLPAPER;

#ifdef NEWSGOLD
    #define GetIMGHDRWallpaper() GetIMGHDRFromCanvasCache(0)
#else
    #define GetIMGHDRWallpaper() GetCanvasBufferPicPtr(0)
#endif

/**********************************************************************************************************************/

void Sie_Resources_Init() {
    MutexCreate(&mtx_res_img);
    IMG_WALLPAPER = GetIMGHDRWallpaper();
    CLIENTS++;
}

void Sie_Resources_Destroy() {
    if (CLIENTS) {
        CLIENTS--;
    }
    if (!CLIENTS) {
        IMG_WALLPAPER = NULL;
        MutexDestroy(&mtx_res_img);
    }
}

/**********************************************************************************************************************/

void SetWallpaper_Proc(void (*proc)()) {
    for (int i = 0; i < 5; i++) {
        IMGHDR *img = GetIMGHDRWallpaper();
        if (img) {
            IMG_WALLPAPER = img;
            break;
        }
        NU_Sleep(50);
    }
    proc();
}

void Sie_Resources_SetWallpaper(WSHDR *ws, void (*proc)()) {
    int hmi_key_id = Registry_GetHMIKeyID("Wallpaper");
    IMG_WALLPAPER = NULL;
    MMI_CanvasBuffer_FlushV(0);
    Registry_SetResourcePath(hmi_key_id, 3, ws);
    if (proc) {
        Sie_SubProc_Run(SetWallpaper_Proc, proc);
    }
}

IMGHDR *Sie_Resources_GetWallpaperIMGHDR() {
    return IMG_WALLPAPER;
}

/**********************************************************************************************************************/

HObj Sie_Resources_CreateHObjFromImgFile(const char *path) {
    HObj hobj = 0;
    unsigned int err = 0;

    size_t len = strlen(path);
    WSHDR *ws = AllocWS((int)len);
    str_2ws(ws, path, len);
    int uid = GetExtUidByFileName_ws(ws);
    if (!uid) {
        FreeWS(ws);
        return hobj;
    }
    hobj = Obs_CreateObject(uid, 0x2D, 0x02, 0x80A8, 1, 1, &err);
    if (err) {
        FreeWS(ws);
        return hobj;
    }
    err = Obs_SetInput_File(hobj, 0, ws);
    FreeWS(ws);
    if (err) {
        Obs_DestroyObject(hobj);
        return 0;
    }
    return hobj;
}

void Sie_Resources_DestroyHObj(HObj hobj) {
    if (hobj) {
        Obs_DestroyObject(hobj);
    }
}


IMGHDR *Sie_Resources_HObj2IMGHDR(HObj hobj, int width, int height) {
    IMGHDR *img = NULL;
    unsigned int err = 0;
    if (width && height) {
        err = Obs_SetOutputImageSize(hobj, (short)width, (short)height);
        if (err) {
            return img;
        }
        err = Obs_SetScaling(hobj, 5);
        if (err) {
            return img;
        }
    }
    err = Obs_Start(hobj);
    if (!err) {
        IMGHDR *tmp = NULL;
        err = Obs_Output_GetPictstruct(hobj, &tmp);
        if (!err) {
            size_t size = CalcBitmapSize((short) tmp->w, (short) tmp->h, tmp->bpnum);
            img = malloc(sizeof(IMGHDR));
            memcpy(img, tmp, sizeof(IMGHDR));
            img->bitmap = malloc(size);
            memcpy(img->bitmap, tmp->bitmap, size);
        }
    }
    return img;
}
