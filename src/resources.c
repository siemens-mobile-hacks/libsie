#include <swilib.h>
#include <stdlib.h>
#include "include/sie/settings.h"
#include "include/sie/resources.h"

IMGHDR *SIE_RES_IMG_WALLPAPER;
SIE_RESOURCES_EXT *SIE_RES_EXT;
unsigned int SIE_RES_EXT_COUNT;
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
        for (unsigned int i = 0; i < SIE_RES_EXT_COUNT; i++) {
            SIE_RESOURCES_EXT *res_ext = &(SIE_RES_EXT[i]);
            mfree(res_ext->icon->bitmap);
            mfree(res_ext->icon);
        }
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

SIE_RESOURCES_EXT *Sie_Resources_ExtGet(const char *ext) {
    SIE_RESOURCES_EXT *res_ext = NULL;
    for (unsigned int i = 0; i < SIE_RES_EXT_COUNT; i++) {
        if (strcmp(SIE_RES_EXT[i].ext, ext) == 0) {
            res_ext = &(SIE_RES_EXT)[i];
        }
    }
    return res_ext;
}

SIE_RESOURCES_EXT *Sie_Resources_ExtLoad(const char *file_name, const char *ext) {
    const char *dir = "0:\\zbin\\img\\libsie\\ext\\";
    SIE_RESOURCES_EXT *res_ext = Sie_Resources_ExtGet(ext);
    if (!res_ext) {
        char *path = malloc(strlen(dir) + strlen(file_name) + 1);
        sprintf(path, "%s%s", dir, file_name);
        IMGHDR *img = CreateIMGHDRFromPngFile(path, 0);
        mfree(path);
        if (img) {
            SIE_RES_EXT = realloc(SIE_RES_EXT, sizeof(SIE_RESOURCES_EXT) * (SIE_RES_EXT_COUNT + 1));
            res_ext = &(SIE_RES_EXT[SIE_RES_EXT_COUNT]);
            strcpy(res_ext->ext, ext);
            res_ext->icon = img;
            SIE_RES_EXT_COUNT++;
        }
    }
    return res_ext;
}
