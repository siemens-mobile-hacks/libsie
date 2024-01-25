#ifndef __SIE_MENU_LIST_H__
#define __SIE_MENU_LIST_H__

#include <swilib.h>
#include "../../freetype/freetype.h"

#define SIE_MENU_LIST_MAX_ITEMS 7
#define SIE_MENU_LIST_KEY_PREV UP_BUTTON
#define SIE_MENU_LIST_KEY_NEXT DOWN_BUTTON
#define SIE_MENU_LIST_KEY_ENTER ENTER_BUTTON

#define SIE_MENU_LIST_ITEM_TYPE_DEFAULT  0x00
#define SIE_MENU_LIST_ITEM_TYPE_CHECKBOX 0x01
#define SIE_MENU_LIST_ITEM_TYPE_RADIO    0x02

typedef struct {
    unsigned int type;
    unsigned int flag;
    IMGHDR *icon;  // just icon
    IMGHDR *icon2; // checkbox or radio
    WSHDR *ws;
    char *color;
    void (*proc)(void *menu_item, unsigned int row);
} SIE_MENU_LIST_ITEM;

typedef struct {
    unsigned int row;
} SIE_MENU_LIST_SEPARATOR;

typedef struct {
    SIE_MENU_LIST_ITEM *items;
    SIE_MENU_LIST_SEPARATOR *separators;
    unsigned int n_items;
    unsigned int n_separators;
    unsigned int row;
    unsigned int offset;
    SIE_FT_SCROLL_STRING *ss;
} SIE_MENU_LIST;

SIE_MENU_LIST *Sie_Menu_List_Init(unsigned int gui_id);
void Sie_Menu_List_AddItem(SIE_MENU_LIST *menu, SIE_MENU_LIST_ITEM *item, const char *name);
void Sie_Menu_List_AddSeparator(SIE_MENU_LIST *menu);
void Sie_Menu_List_Destroy(SIE_MENU_LIST *menu);

void Sie_Menu_List_DrawMenu(SIE_MENU_LIST *menu);
void Sie_Menu_List_DrawEmpty();
void Sie_Menu_List_Draw(SIE_MENU_LIST *menu);
void Sie_Menu_List_OnKey(SIE_MENU_LIST *menu, GUI_MSG *msg);
void Sie_Menu_List_Refresh(SIE_MENU_LIST *menu);


unsigned int Sie_Menu_List_SetRow(SIE_MENU_LIST *menu, unsigned int row);

void Sie_Menu_List_SetItemType(SIE_MENU_LIST_ITEM *item, unsigned int type, unsigned int flag);
SIE_MENU_LIST_ITEM *Sie_Menu_List_GetItem(SIE_MENU_LIST *menu, unsigned int row);
#define Sie_Menu_List_GetCurrentItem(menu) Sie_Menu_List_GetItem(menu, menu->row);
unsigned int Sie_Menu_List_GetRowByName_ws(SIE_MENU_LIST *menu, WSHDR *ws, unsigned int *err);

#endif
