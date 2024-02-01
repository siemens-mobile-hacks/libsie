#ifndef __SIE_GUI_BOX_H__
#define __SIE_GUI_BOX_H__

#include "surface.h"

#define SIE_GUI_BOX_TYPE_STANDARD 0x00
#define SIE_GUI_BOX_TYPE_QUESTION 0x01
#define SIE_GUI_BOX_TYPE_WAIT     0x02

#define SIE_GUI_BOX_CALLBACK_NO  0x00
#define SIE_GUI_BOX_CALLBACK_YES 0x01

typedef struct {
    const char *msg;
    const char *left_sk;
    const char *right_sk;
} SIE_GUI_BOX_TEXT;

typedef struct {
    void (*proc)(int, void*);
    void *data;
} SIE_GUI_BOX_CALLBACK;

typedef struct {
    GUI gui;
    SIE_GUI_SURFACE *surface;
    unsigned int type;
    WSHDR *msg_ws;
    WSHDR *left_sk_ws;
    WSHDR *right_sk_ws;
    SIE_GUI_BOX_CALLBACK callback;
} SIE_GUI_BOX_GUI;

SIE_GUI_BOX_GUI *Sie_GUI_Box(unsigned int type, SIE_GUI_BOX_TEXT *text, SIE_GUI_BOX_CALLBACK *callback);
SIE_GUI_BOX_GUI *Sie_GUI_MsgBox(const char *msg);
SIE_GUI_BOX_GUI *Sie_GUI_MsgBoxYesNo(const char *msg, SIE_GUI_BOX_CALLBACK *callback);

#endif
