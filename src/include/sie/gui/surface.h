#ifndef __SIE_GUI_SURFACE_H__
#define __SIE_GUI_SURFACE_H__

#include <swilib.h>

#define SIE_GUI_SURFACE_TYPE_DEFAULT 0x00

typedef struct {
    void (*OnAfterDrawIconBar)();
    int (*OnKey)(void *data, GUI_MSG *msg);
} SIE_GUI_SURFACE_HANDLERS;

typedef struct {
    int type;
    SIE_GUI_SURFACE_HANDLERS handlers;
    unsigned int gui_id;
    WSHDR *hdr_ws;
    IMGHDR scrot;
} SIE_GUI_SURFACE;

#endif
