#ifndef __SIE_RESOURCES_H__
#define __SIE_RESOURCES_H__

#include <swilib.h>

void Sie_Resources_SetWallpaper(WSHDR *ws, void (*proc)());
IMGHDR *Sie_Resources_GetWallpaperIMGHDR();

HObj Sie_Resources_CreateHObjFromImgFile(const char *path);
void Sie_Resources_DestroyHObj(HObj hobj);
IMGHDR *Sie_Resources_HObj2IMGHDR(HObj hobj, int width, int height);

#endif
