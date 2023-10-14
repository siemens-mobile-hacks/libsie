#ifndef __SIE_RESOURCES_H__
#define __SIE_RESOURCES_H__

#include <swilib.h>

typedef struct {
    void *prev;
    void *next;
    char *type;
    char *name;
    unsigned int size;
    IMGHDR *icon;
} SIE_RESOURCES_EXT;

void Sie_Resources_Init();
void Sie_Resources_Destroy();

void Sie_Resources_SetWallpaper(WSHDR *ws);

SIE_RESOURCES_EXT *Sie_Resources_LoadImage(const char *type, const char *name, unsigned int size);

#endif
