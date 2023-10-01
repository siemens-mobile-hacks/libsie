#ifndef __SIE_RESOURCES_H__
#define __SIE_RESOURCES_H__

#include <swilib.h>

typedef struct {
    char ext[16];
    IMGHDR *icon;
} SIE_RESOURCES_EXT;

void Sie_Resources_Init();
void Sie_Resources_Destroy();

void Sie_Resources_SetWallpaper(WSHDR *ws);

SIE_RESOURCES_EXT *Sie_Resources_ExtLoad(const char *file_name, const char *ext);

#endif
