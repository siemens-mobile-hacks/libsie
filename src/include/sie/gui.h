#ifndef __SIE_GUI_H__
#define __SIE_GUI_H__

#include <swilib.h>
#include <wchar.h>

#define SIE_GUI_SURFACE_TYPE_DEFAULT 0x00

typedef struct {
    int type;
    int (*OnKey)(void *data, GUI_MSG *msg);
    WSHDR *ws_hdr;
} SIE_GUI_SURFACE;

SIE_GUI_SURFACE *Sie_GUI_Surface_Init(int type, int (*OnKey)(void *data, GUI_MSG *msg));
void Sie_GUI_Surface_Destroy(SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_Draw(const SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_OnFocus();
void Sie_GUI_Surface_OnUnFocus();
int Sie_GUI_Surface_OnKey(SIE_GUI_SURFACE *surface, void *data, GUI_MSG *msg);

#define SCREEN_X2 (ScreenW() - 1)
#define SCREEN_Y2 (ScreenH() - 1)

#define PADDING_ICONBAR 5
#define PADDING_HEADER 5

void Sie_GUI_DrawIMGHDR(IMGHDR *img, int x, int y, int w, int h);
void Sie_GUI_DrawBleedIMGHDR(IMGHDR  *img, int x, int y, int x2, int y2, int bleed_x, int bleed_y);
void Sie_GUI_DrawIconBar();
void Sie_GUI_DrawHeader(WSHDR *ws);

#endif
