#include <swilib.h>
#include <stdlib.h>
#include "include/sie/ext.h"
#include "include/sie/settings.h"
#include "include/sie/resources.h"

IMGHDR *SIE_RES_IMG_WALLPAPER;
SIE_RESOURCES_IMG *SIE_RES_IMG;
unsigned int SIE_RES_CLIENTS;


void Sie_Resources_Init() {
    if (!SIE_RES_IMG_WALLPAPER) {
        size_t len;
        char *path;
        WSHDR *ws = AllocWS(1024);
        Settings_ReadWS(ws, NULL, "wallpaper");
        len = wstrlen(ws);
        path = malloc(len + 1);
        ws_2str(ws, path, len);
        FreeWS(ws);

        HObj hobj = Sie_Resources_CreateHObjFromImgFile(path);
        if (hobj) {
            SIE_RES_IMG_WALLPAPER = Sie_Resources_HObj2IMGHDR(hobj, ScreenW(), ScreenH());
            Obs_DestroyObject(hobj);
        }
        mfree(path);
    }
    Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_STATUS, 24, "battery-00");
    Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_STATUS, 24, "battery-20");
    Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_STATUS, 24, "battery-40");
    Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_STATUS, 24, "battery-60");
    Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_STATUS, 24, "battery-80");
    Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_STATUS, 24, "battery-100");
    SIE_RES_CLIENTS++;
}

void Sie_Resources_Destroy() {
    if (SIE_RES_CLIENTS) {
        SIE_RES_CLIENTS--;
    }
    if (!SIE_RES_CLIENTS) {
        if (SIE_RES_IMG_WALLPAPER) {
            mfree(SIE_RES_IMG_WALLPAPER->bitmap);
            mfree(SIE_RES_IMG_WALLPAPER);
        }
        SIE_RES_IMG_WALLPAPER = NULL;
        SIE_RESOURCES_IMG *p = SIE_RES_IMG;
        while (p) {
            SIE_RESOURCES_IMG *prev = p->prev;
            mfree(p->name);
            mfree(p->icon->bitmap);
            mfree(p->icon);
            mfree(p);
            p = prev;
        }
        SIE_RES_IMG = NULL;
    }
}

/**********************************************************************************************************************/

void Sie_Resources_SetWallpaper(WSHDR *ws) {
    if (SIE_RES_IMG_WALLPAPER) {
        mfree(SIE_RES_IMG_WALLPAPER->bitmap);
        mfree(SIE_RES_IMG_WALLPAPER);
    }
    Settings_UpdateWS(ws, NULL, "wallpaper");

    size_t len = wstrlen(ws);
    char *path = malloc(len + 1);
    ws_2str(ws, path, len);
    HObj hobj = Sie_Resources_CreateHObjFromImgFile(path);
    if (hobj) {
        SIE_RES_IMG_WALLPAPER = Sie_Resources_HObj2IMGHDR(hobj, ScreenW(), ScreenH());
        Obs_DestroyObject(hobj);
    }
    mfree(path);
}

/**********************************************************************************************************************/

SIE_RESOURCES_IMG *Sie_Resources_GetImage(unsigned int type, unsigned int size, const char *name) {
    SIE_RESOURCES_IMG *p = SIE_RES_IMG;
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
        default:
            strcpy(_type, "apps");
    }

    SIE_RESOURCES_IMG *res_ext = Sie_Resources_GetImage(type, size, name);
    if (!res_ext) {
        size_t len_type = strlen(_type);
        size_t len_name = strlen(name);

        char *path = malloc(strlen(dir) + len_type + 16 + len_name + 4 + 1);
        sprintf(path, "%s\\%s\\%d\\%s.png", dir, _type, size, name);
        IMGHDR *img = CreateIMGHDRFromPngFile(path, 0);
        mfree(path);
        if (img) {
            res_ext = malloc(sizeof(SIE_RESOURCES_IMG));
            zeromem(res_ext, sizeof(SIE_RESOURCES_IMG));
            res_ext->type = type;
            res_ext->size = size;
            res_ext->name = malloc(len_name + 1);
            strcpy(res_ext->name, name);
            res_ext->icon = img;
            if (!SIE_RES_IMG) {
                SIE_RES_IMG = res_ext;
            } else {
                SIE_RES_IMG->next = res_ext;
                res_ext->prev = SIE_RES_IMG;
                SIE_RES_IMG = res_ext;
            }
        }
    }
    return res_ext;
}

HObj Sie_Resources_CreateHObjFromImgFile(const char *path) {
    HObj hobj = 0;
    unsigned int err = 0;

    size_t len = strlen(path);
    WSHDR *ws = AllocWS(len);
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
