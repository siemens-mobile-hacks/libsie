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

SIE_GUI_STACK *Sie_GUI_Stack_Delete(SIE_GUI_STACK *top, unsigned int gui_id) {
    SIE_GUI_STACK *gui = Sie_GUI_Stack_FindByGuiID(top, gui_id);
    if (gui) {
        SIE_GUI_STACK *prev = gui->prev;
        SIE_GUI_STACK *next = gui->next;
        if (prev) {
            prev->next = next;
        }
        if (next) {
            next->prev = prev;
        }
        mfree(gui);
        if (gui == top) {
            return prev;
        }
    }
    return NULL;
}

SIE_GUI_STACK *Sie_GUI_Stack_CloseChildren(SIE_GUI_STACK *top, unsigned int gui_id) {
    SIE_GUI_STACK *stack = Sie_GUI_Stack_FindByGuiID(top, gui_id);
    int *ids = NULL;
    unsigned int i = 0;
    while (top != stack) {
        ids = realloc(ids, sizeof(int) * (i + 1));
        ids[i++] = top->gui_id;
        top = top->prev;
    }
    for (unsigned int j = 0; j < i; j++) {
        Sie_GUI_CloseGUI_GBS(ids[j]);
    }
    if (ids) {
        mfree(ids);
    }
    return (stack) ? stack : top;
}
