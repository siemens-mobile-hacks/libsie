#ifndef __SIE_GUI_STACK_H__
#define __SIE_GUI_STACK_H__

#include <swilib.h>

typedef struct {
    void *next;
    void *prev;
    GUI *gui;
    unsigned int gui_id;
} SIE_GUI_STACK;


SIE_GUI_STACK *Sie_GUI_Stack_Add(SIE_GUI_STACK *top, GUI *gui, unsigned int gui_id);
void Sie_GUI_Stack_Destroy(SIE_GUI_STACK *stack);

SIE_GUI_STACK *Sie_GUI_Stack_FindByGuiID(const SIE_GUI_STACK *top, unsigned int gui_id);
SIE_GUI_STACK *Sie_GUI_Stack_Delete(SIE_GUI_STACK *top, unsigned int gui_id);
SIE_GUI_STACK *Sie_GUI_Stack_CloseChildren(SIE_GUI_STACK *top, unsigned int gui_id);

#endif
