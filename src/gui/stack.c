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
        LockSched();
        SIE_GUI_STACK *prev = top->prev;
        prev->next = NULL;
        mfree(top);
        UnlockSched();
        return prev;
    } else return top;
}

SIE_GUI_STACK *Sie_GUI_Stack_CloseChildren(SIE_GUI_STACK *top, unsigned int gui_id) {
    SIE_GUI_STACK *stack = Sie_GUI_Stack_FindByGuiID(top, gui_id);
    if (stack) {
        while (top != stack) {
            LockSched();
            Sie_GUI_CloseGUI(top->gui_id);
            UnlockSched();
            top = top->prev;
        }
        return stack;
    } else return top;
}
