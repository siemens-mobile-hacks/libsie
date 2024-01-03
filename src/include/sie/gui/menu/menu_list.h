#ifndef __SIE_MENU_LIST_H__
#define __SIE_MENU_LIST_H__

#include <swilib.h>
#include "../../freetype/freetype.h"

#define SIE_MENU_LIST_MAX_ITEMS 7
#define SIE_MENU_LIST_KEY_PREV UP_BUTTON
#define SIE_MENU_LIST_KEY_NEXT DOWN_BUTTON
#define SIE_MENU_LIST_KEY_ENTER ENTER_BUTTON

typedef struct {
    IMGHDR *icon;
    WSHDR *ws;
    char *color;
    void (*proc)(void *menu_item, unsigned int row);
} SIE_MENU_LIST_ITEM;

typedef struct {
    SIE_MENU_LIST_ITEM *items;
    unsigned int n_items;
    unsigned int row;
    unsigned int offset;
    SIE_FT_SCROLL_STRING *ss;
    GBSTMR tmr_ss;
} SIE_MENU_LIST;

SIE_MENU_LIST *Sie_Menu_List_Init(unsigned int gui_id, SIE_MENU_LIST_ITEM *items, unsigned int n_items);
void Sie_Menu_List_Destroy(SIE_MENU_LIST *menu);

void Sie_Menu_List_DrawMenu(SIE_MENU_LIST *menu);
void Sie_Menu_List_DrawEmpty();
void Sie_Menu_List_Draw(SIE_MENU_LIST *menu);

void Sie_Menu_List_OnKey(SIE_MENU_LIST *menu, GUI_MSG *msg);

void Sie_Menu_List_Refresh(SIE_MENU_LIST *menu);
unsigned int Sie_Menu_List_GetIdByName_ws(SIE_MENU_LIST *menu, WSHDR *ws, unsigned int *err);
void Sie_Menu_List_SetRow(SIE_MENU_LIST *menu, unsigned int row);

#endif
