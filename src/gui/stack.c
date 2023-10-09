#include <swilib.h>
#include <stdlib.h>
#include "../include/sie/gui/gui.h"

SIE_GUI_STACK *Sie_GUI_Stack_Add(SIE_GUI_STACK *top, GUI *gui, unsigned int gui_id) {
    SIE_GUI_STACK *stack = malloc(sizeof(SIE_GUI_STACK));
    zeromem(stack, sizeof(SIE_GUI_STACK));
    stack->prev = top;
    stack->gui = gui;
    stack->gui_id = gui_id;
    if (top) {
        top->next = stack;
    }
    return stack;
}

void Sie_GUI_Stack_Destroy(SIE_GUI_STACK *stack) {
    SIE_GUI_STACK *p = stack;
    while (p) {
        SIE_GUI_STACK *prev = p->prev;
        mfree(p);
        p = prev;
    }
}

SIE_GUI_STACK *Sie_GUI_Stack_FindByGuiID(const SIE_GUI_STACK *top, unsigned int gui_id) {
    SIE_GUI_STACK *p = (SIE_GUI_STACK*)top;
    while (p) {
        if (p->gui_id == gui_id) {
            return p;
        }
        p = p->prev;
    }
    return NULL;
}

SIE_GUI_STACK *Sie_GUI_Stack_Pop(SIE_GUI_STACK *top, unsigned int gui_id) {
    if (top->gui_id == gui_id) {
        SIE_GUI_STACK *prev = top->prev;
        prev->next = NULL;
        mfree(top);
        return prev;
    } else return top;
}

SIE_GUI_STACK *Sie_GUI_Stack_CloseChildren(SIE_GUI_STACK *top, unsigned int gui_id) {
    SIE_GUI_STACK *stack = Sie_GUI_Stack_FindByGuiID(top, gui_id);
    if (stack) {
        SIE_GUI_STACK *p = stack->next;
        while (p) {
            SIE_GUI_STACK *next = p->next;
            GeneralFunc_flag1((int)p->gui_id, 0);
            mfree(p);
            p = next;
        }
        stack->next = NULL;
        return stack;
    } else return top;
}
