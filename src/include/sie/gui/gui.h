#ifndef __SIE_GUI_H__
#define __SIE_GUI_H__

#include <swilib.h>
#include <wchar.h>
#include "box.h"
#include "stack.h"
#include "colors.h"
#include "surface.h"
#include "menu/menu.h"
#include "menu/menu_list.h"

SIE_GUI_SURFACE *Sie_GUI_Surface_Init(int type, const SIE_GUI_SURFACE_HANDLERS *handlers);
void Sie_GUI_Surface_Destroy(SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_DoScrot(SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_Draw(const SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_OnFocus(SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_OnUnFocus(SIE_GUI_SURFACE *surface);
int Sie_GUI_Surface_OnKey(SIE_GUI_SURFACE *surface, void *data, GUI_MSG *msg);

#define SCREEN_X2 (ScreenW() - 1)
#define SCREEN_Y2 (ScreenH() - 1)

#define PADDING_ICONBAR 5
#define PADDING_HEADER 5

#define Sie_GUI_CloseGUI(id) GeneralFunc_flag1((int)id, 0);

void Sie_GUI_InitCanvas(RECT *canvas);
void Sie_GUI_SetRGB(char *rgb, char r, char g, char b);

void Sie_GUI_DrawIMGHDR(IMGHDR *img, int x, int y, int w, int h);
void Sie_GUI_DrawBleedIMGHDR(IMGHDR  *img, int x, int y, int x2, int y2, int bleed_x, int bleed_y);
void Sie_GUI_DrawIconBar();
void Sie_GUI_DrawHeader(WSHDR *ws);

#endif
