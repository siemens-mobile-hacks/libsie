#ifndef __SIE_GUI_MSG_BOX_H__
#define __SIE_GUI_MSG_BOX_H__

#define SIE_GUI_MSG_BOX_CALLBACK_NO  0x00
#define SIE_GUI_MSG_BOX_CALLBACK_YES 0x01

typedef struct {
    void (*proc)(int, void*);
    void *data;
} SIE_GUI_MSG_BOX_CALLBACK;

void Sie_GUI_MsgBox(const char *msg, const char *left, const char *right, SIE_GUI_MSG_BOX_CALLBACK *callback);
void Sie_GUI_MsgBoxYesNo(const char *msg, SIE_GUI_MSG_BOX_CALLBACK *callback);

#endif
