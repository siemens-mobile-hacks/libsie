#ifndef __SIE_MENU_LIST_H__
#define __SIE_MENU_LIST_H__

#include <swilib.h>

#define SIE_MENU_LIST_MAX_ITEMS 7
#define SIE_MENU_LIST_KEY_PREV UP_BUTTON
#define SIE_MENU_LIST_KEY_NEXT DOWN_BUTTON
#define SIE_MENU_LIST_KEY_ENTER ENTER_BUTTON

typedef struct {
    IMGHDR *icon;
    WSHDR *ws;
    void (*proc)(void);
} SIE_MENU_LIST_ITEM;

typedef struct {
    SIE_MENU_LIST_ITEM *items;
    unsigned int n_items;

    unsigned int row;
    unsigned int offset;
} SIE_MENU_LIST;

SIE_MENU_LIST *Sie_Menu_List_Init(SIE_MENU_LIST_ITEM *items, unsigned int n_items);
void Sie_Menu_List_Destroy(SIE_MENU_LIST *menu);

void Sie_Menu_List_DrawMenu(SIE_MENU_LIST *menu);
void Sie_Menu_List_DrawEmpty();
void Sie_Menu_List_Refresh(SIE_MENU_LIST *menu);
void Sie_Menu_List_Draw(SIE_MENU_LIST *menu);
void Sie_Menu_List_OnKey(SIE_MENU_LIST *menu, GUI_MSG *msg);

#endif