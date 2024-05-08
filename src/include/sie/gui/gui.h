#pragma once

#include <swilib.h>
#include <wchar.h>
#include "stack.h"
#include "colors.h"

typedef struct {
    unsigned int gui_id;
    void (*proc)(void *data);
    void *data;
} SIE_GUI_FOCUS_DATA;

#define Sie_GUI_CloseGUI(id) GeneralFunc_flag1((int)id, 1);
void Sie_GUI_FocusGUI_Async(GBSTMR *tmr, SIE_GUI_FOCUS_DATA *data);
void Sie_GUI_FocusGUI_Sync(unsigned int gui_id);

void Sie_GUI_InitCanvas(RECT *canvas);
void Sie_GUI_InitHeader(HEADER_DESC *header_desc);
void Sie_GUI_InitHeaderSmall(HEADER_DESC *header_desc);
void Sie_GUI_InitInput(INPUTDIA_DESC *input_desc);
void Sie_GUI_SetRGB(char *rgb, char r, char g, char b);
IMGHDR *Sie_GUI_TakeScrot();

void Sie_GUI_DrawIMGHDR(IMGHDR *img, int x, int y, int w, int h);
void Sie_GUI_DrawBleedIMGHDR(IMGHDR  *img, int x, int y, int x2, int y2, int bleed_x, int bleed_y);
