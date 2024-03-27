#include <swilib.h>
#include <stdlib.h>
#include <string.h>
#include "../include/sie/gui/gui.h"

GBSTMR TMR_REDRAW_ICONBAR;

SIE_GUI_SURFACE *Sie_GUI_Surface_Init(int type, const SIE_GUI_SURFACE_HANDLERS *handlers, unsigned int gui_id) {
    SIE_GUI_SURFACE *surface = malloc(sizeof(SIE_GUI_SURFACE));
    zeromem(surface, sizeof(SIE_GUI_SURFACE));
    surface->type = type;
    if (handlers) {
        memcpy(&(surface->handlers), handlers, sizeof(SIE_GUI_SURFACE_HANDLERS));
    }
    surface->gui_id = gui_id;
    surface->hdr_ws = AllocWS(256);
    return surface;
};

void Sie_GUI_Surface_Destroy(SIE_GUI_SURFACE *surface) {
    FreeWS(surface->hdr_ws);
    Sie_GUI_Surface_DestroyScrot(surface);
    mfree(surface);
}

void Sie_GUI_Surface_SetHeader(SIE_GUI_SURFACE *surface, const char *header) {
    wsprintf(surface->hdr_ws, "%t", header);
}

void Sie_GUI_Surface_TakeScrot(SIE_GUI_SURFACE *surface) {
    Sie_GUI_Surface_DestroyScrot(surface);
    surface->scrot = Sie_GUI_TakeScrot();
}

void Sie_GUI_Surface_DestroyScrot(SIE_GUI_SURFACE *surface) {
    LockSched();
    if (surface->scrot) {
        mfree(surface->scrot->bitmap);
        mfree(surface->scrot);
        surface->scrot = NULL;
    }
    UnlockSched();
}

void Sie_GUI_Surface_Draw(const SIE_GUI_SURFACE *surface) {
    Sie_GUI_DrawIconBar();
    if (wstrlen(surface->hdr_ws)) {
        Sie_GUI_DrawHeader(surface->hdr_ws);
    }
}

void Sie_GUI_Surface_OnFocus(SIE_GUI_SURFACE *surface) {
    extern IMGHDR *IMG_WALLPAPER;
#ifdef ELKA
    DisableIconBar(1);
#endif
    IMG_WALLPAPER = GetIMGHDRFromCanvasCache(0);
    TMR_REDRAW_ICONBAR.param6 = (int)surface;
    GBS_StartTimerProc(&TMR_REDRAW_ICONBAR, 216, Sie_GUI_DrawIconBar);
}

void Sie_GUI_Surface_OnUnFocus(SIE_GUI_SURFACE *surface) {
#ifdef ELKA
    DisableIconBar(0);
#endif
    TMR_REDRAW_ICONBAR.param6 = 0;
    GBS_StopTimer(&TMR_REDRAW_ICONBAR);
}

int Sie_GUI_Surface_OnKey(SIE_GUI_SURFACE *surface, void *data, GUI_MSG *msg) {
    if (msg->gbsmsg->msg == KEY_DOWN && msg->gbsmsg->submess == 0xDEAD + surface->gui_id) {
        return 1;
    }
    else if (surface->handlers.OnKey) {
        return surface->handlers.OnKey(data, msg);
    }
    return 0;
}
