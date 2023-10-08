#ifndef __SIE_GUI_H__
#define __SIE_GUI_H__

#include <swilib.h>
#include <wchar.h>

#define SIE_GUI_SURFACE_TYPE_DEFAULT 0x00

typedef struct {
    void (*OnAfterDrawIconBar)();
    int (*OnKey)(void *data, GUI_MSG *msg);
} SIE_GUI_SURFACE_HANDLERS;

typedef struct {
    int type;
    SIE_GUI_SURFACE_HANDLERS handlers;
    WSHDR *hdr_ws;
    IMGHDR scrot;
} SIE_GUI_SURFACE;

SIE_GUI_SURFACE *Sie_GUI_Surface_Init(int type, const SIE_GUI_SURFACE_HANDLERS *handlers);
void Sie_GUI_Surface_Destroy(SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_DoScrot(SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_Draw(const SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_OnFocus(SIE_GUI_SURFACE *surface);
void Sie_GUI_Surface_OnUnfocus(SIE_GUI_SURFACE *surface);
int Sie_GUI_Surface_OnKey(SIE_GUI_SURFACE *surface, void *data, GUI_MSG *msg);

#define SCREEN_X2 (ScreenW() - 1)
#define SCREEN_Y2 (ScreenH() - 1)

#define PADDING_ICONBAR 5
#define PADDING_HEADER 5

void Sie_GUI_InitCanvas(RECT *canvas);
void Sie_GUI_DrawIMGHDR(IMGHDR *img, int x, int y, int w, int h);
void Sie_GUI_DrawBleedIMGHDR(IMGHDR  *img, int x, int y, int x2, int y2, int bleed_x, int bleed_y);
void Sie_GUI_DrawIconBar();
void Sie_GUI_DrawHeader(WSHDR *ws);


#define SIE_GUI_MSG_BOX_CALLBACK_NO  0x00
#define SIE_GUI_MSG_BOX_CALLBACK_YES 0x01

void Sie_GUI_MsgBoxYesNo(WSHDR *ws, void (*CallBackProc)(int));

#endif
