#include <swilib.h>
#include <stdlib.h>
#include "include/sie/settings.h"
#include "include/sie/resources.h"

IMGHDR *SIE_RES_IMG_WALLPAPER;
SIE_RESOURCES_EXT *SIE_RES_EXT;
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
        SIE_RES_IMG_WALLPAPER = CreateIMGHDRFromPngFile(path, 0);
        mfree(path);
    }
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
        SIE_RESOURCES_EXT *p = SIE_RES_EXT;
        while (p) {
            SIE_RESOURCES_EXT *prev = p->prev;
            mfree(p->name);
            mfree(p->icon->bitmap);
            mfree(p->icon);
            mfree(p);
            p = prev;
        }
        SIE_RES_EXT = NULL;
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
    SIE_RES_IMG_WALLPAPER = CreateIMGHDRFromPngFile(path, 0);
    mfree(path);
}

/**********************************************************************************************************************/

SIE_RESOURCES_EXT *Sie_Resources_GetImage(unsigned int type, unsigned int size, const char *name) {
    SIE_RESOURCES_EXT *p = SIE_RES_EXT;
    while (p) {
        if (p->type == type && p->size == size && strcmp(p->name, name) == 0) {
            return p;
        }
        p = p->prev;
    }
    return NULL;
}

SIE_RESOURCES_EXT *Sie_Resources_LoadImage(unsigned int type, unsigned int size, const char *name) {
    char _type[32];
    const char *dir = "0:\\zbin\\img";
    switch (type) {
        case SIE_RESOURCES_TYPE_EXT:
            strcpy(_type, "ext");
            break;
        case SIE_RESOURCES_TYPE_PLACES:
            strcpy(_type, "places");
            break;
        default:
            strcpy(_type, "apps");
    }

    SIE_RESOURCES_EXT *res_ext = Sie_Resources_GetImage(type, size, name);
    if (!res_ext) {
        size_t len_type = strlen(_type);
        size_t len_name = strlen(name);

        char *path = malloc(strlen(dir) + len_type + 16 + len_name + 3 + 1);
        sprintf(path, "%s\\%s\\%d\\%s.png", dir, _type, size, name);
        IMGHDR *img = CreateIMGHDRFromPngFile(path, 0);
        mfree(path);
        if (img) {
            res_ext = malloc(sizeof(SIE_RESOURCES_EXT));
            zeromem(res_ext, sizeof(SIE_RESOURCES_EXT));
            res_ext->type = type;
            res_ext->size = size;
            res_ext->name = malloc(len_name);
            strcpy(res_ext->name, name);
            res_ext->icon = img;
            if (!SIE_RES_EXT) {
                SIE_RES_EXT = res_ext;
            } else {
                SIE_RES_EXT->next = res_ext;
                res_ext->prev = SIE_RES_EXT;
                SIE_RES_EXT = res_ext;
            }
        }
    }
    return res_ext;
}
