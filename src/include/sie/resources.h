#ifndef __SIE_RESOURCES_H__
#define __SIE_RESOURCES_H__

#include <swilib.h>

#define SIE_RESOURCES_TYPE_EXT    0x00
#define SIE_RESOURCES_TYPE_PLACES 0x01

typedef struct {
    void *prev;
    void *next;
    unsigned int type;
    unsigned int size;
    char *name;
    IMGHDR *icon;
} SIE_RESOURCES_EXT;

void Sie_Resources_Init();
void Sie_Resources_Destroy();

void Sie_Resources_SetWallpaper(WSHDR *ws);

SIE_RESOURCES_EXT *Sie_Resources_LoadImage(unsigned int type, unsigned int size, const char *name);
HObj Sie_Resources_CreateHObjFromImgFile(const char *path);
void Sie_Resources_DestroyHObj(HObj hobj);
IMGHDR *Sie_Resources_HObj2IMGHDR(HObj hobj, int width, int height);

#endif
