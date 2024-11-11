#include <swilib.h>
#include <swilib/nucleus.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/sie/subproc.h"
#include "include/sie/resources.h"

extern int CFG_ICONS_CACHE_SIZE;

MUTEX mtx_res_img;
unsigned int CLIENTS;

int RES_IMG_COUNT;
IMGHDR *IMG_WALLPAPER;
SIE_RESOURCES_IMG *RES_IMG_FIRST, *RES_IMG_LAST;

void DestroyElement(SIE_RESOURCES_IMG *res_img) {
    mfree(res_img->name);
    mfree(res_img->icon->bitmap);
    mfree(res_img->icon);
    mfree(res_img);
}
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
        SIE_RESOURCES_IMG *p = RES_IMG_LAST;
        while (p) {
            SIE_RESOURCES_IMG *prev = p->prev;
            DestroyElement(p);
            p = prev;
        }
        RES_IMG_FIRST = RES_IMG_LAST = NULL;
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

SIE_RESOURCES_IMG *Sie_Resources_GetImage(unsigned int type, unsigned int size, const char *name) {
    SIE_RESOURCES_IMG *p = RES_IMG_LAST;
    while (p) {
        if (p->type == type && p->size == size && strcmp(p->name, name) == 0) {
            return p;
        }
        p = p->prev;
    }
    return NULL;
}

SIE_RESOURCES_IMG *Sie_Resources_LoadImage(unsigned int type, unsigned int size, const char *name) {
    char _type[32];
    const char *dir = "0:\\zbin\\img";
    switch (type) {
        case SIE_RESOURCES_TYPE_EXT:
            strcpy(_type, "ext");
            break;
        case SIE_RESOURCES_TYPE_PLACES:
            strcpy(_type, "places");
            break;
        case SIE_RESOURCES_TYPE_STATUS:
            strcpy(_type, "status");
            break;
        case SIE_RESOURCES_TYPE_ACTIONS:
            strcpy(_type, "actions");
            break;
        case SIE_RESOURCES_TYPE_DEVICES:
            strcpy(_type, "devices");
            break;
        case SIE_RESOURCES_TYPE_APPS: default:
            strcpy(_type, "apps");
    }

    MutexLock(&mtx_res_img);
    SIE_RESOURCES_IMG *res_img = Sie_Resources_GetImage(type, size, name);
    if (!res_img) {
        size_t len_type = strlen(_type);
        size_t len_name = strlen(name);

        char *path = malloc(strlen(dir) + len_type + 16 + len_name + 4 + 1);
        sprintf(path, "%s\\%s\\%d\\%s.png", dir, _type, size, name);
        IMGHDR *img = CreateIMGHDRFromPngFile(path, 0);
        mfree(path);
        if (img) {
            res_img = malloc(sizeof(SIE_RESOURCES_IMG));
            zeromem(res_img, sizeof(SIE_RESOURCES_IMG));
            res_img->type = type;
            res_img->size = size;
            res_img->name = malloc(len_name + 1);
            strcpy(res_img->name, name);
            res_img->icon = img;
            if (!RES_IMG_LAST) {
                RES_IMG_FIRST = RES_IMG_LAST = res_img;
            } else {
                int diff = RES_IMG_COUNT - CFG_ICONS_CACHE_SIZE;
                if (diff > 0) {
                    SIE_RESOURCES_IMG *p = RES_IMG_FIRST;
                    for (int i = 0; i < diff; i++) {
                        SIE_RESOURCES_IMG *next = p->next;
                        DestroyElement(p);
                        next->prev = NULL;
                        p = next;
                        RES_IMG_COUNT--;
                        RES_IMG_FIRST = p;
                    }
                }
                RES_IMG_LAST->next = res_img;
                res_img->prev = RES_IMG_LAST;
                RES_IMG_LAST = res_img;
            }
            RES_IMG_COUNT++;
        }
    }
    MutexUnlock(&mtx_res_img);
    return res_img;
}

IMGHDR *Sie_Resources_LoadIMGHDR(unsigned int type, unsigned int size, const char *name) {
    SIE_RESOURCES_IMG *res_img = Sie_Resources_LoadImage(type, size, name);
    return (res_img) ? res_img->icon : NULL;
}

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
