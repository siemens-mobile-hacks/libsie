#ifndef __SIE_GUI_MSG_BOX_H__
#define __SIE_GUI_MSG_BOX_H__

#include "../include/sie/gui/surface.h"

#define SIE_GUI_MSG_BOX_CALLBACK_NO  0x00
#define SIE_GUI_MSG_BOX_CALLBACK_YES 0x01

typedef struct {
    void (*proc)(int, void*);
    void *data;
} SIE_GUI_MSG_BOX_CALLBACK;

typedef struct {
    GUI gui;
    SIE_GUI_SURFACE *surface;
    WSHDR *msg_ws;
    WSHDR *left_ws;
    WSHDR *right_ws;
    SIE_GUI_MSG_BOX_CALLBACK callback;
} SIE_MSG_BOX_GUI;

SIE_MSG_BOX_GUI *Sie_GUI_MsgBox(const char *msg, const char *left, const char *right,
                                SIE_GUI_MSG_BOX_CALLBACK *callback);
SIE_MSG_BOX_GUI *Sie_GUI_MsgBoxDefault(const char *msg);
SIE_MSG_BOX_GUI *Sie_GUI_MsgBoxYesNo(const char *msg, SIE_GUI_MSG_BOX_CALLBACK *callback);

#endif
