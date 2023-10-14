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
            mfree(p->type);
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

SIE_RESOURCES_EXT *Sie_Resources_GetImage(const char *type, const char *name, unsigned int size) {
    SIE_RESOURCES_EXT *p = SIE_RES_EXT;
    while (p) {
        if (strcmp(p->type, type) == 0 && p->size == size && strcmp(p->name, name) == 0) {
            return p;
        }
        p = p->prev;
    }
    return NULL;
}

SIE_RESOURCES_EXT *Sie_Resources_LoadImage(const char *type, const char *name, unsigned int size) {
    const char *dir = "0:\\zbin\\img";
    SIE_RESOURCES_EXT *res_ext = Sie_Resources_GetImage(type, name, size);
    if (!res_ext) {
        size_t len_type = strlen(type);
        size_t len_name = strlen(name);

        char *path = malloc(strlen(dir) + len_type + 16 + len_name + 3 + 1);
        sprintf(path, "%s\\%s\\%d\\%s.png", dir, type, size, name);
        IMGHDR *img = CreateIMGHDRFromPngFile(path, 0);
        mfree(path);
        if (img) {
            res_ext = malloc(sizeof(SIE_RESOURCES_EXT));
            zeromem(res_ext, sizeof(SIE_RESOURCES_EXT));
            res_ext->type = malloc(len_type);
            res_ext->name = malloc(len_name);
            strcpy(res_ext->type, type);
            strcpy(res_ext->name, name);
            res_ext->size = size;
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
