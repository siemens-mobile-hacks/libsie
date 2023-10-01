#ifndef __SIE_MENU_H__
#define __SIE_MENU_H__

#include <wchar.h>
#include <swilib.h>

#define SIE_MENU_MAX_COLS 3
#define SIE_MENU_MAX_ROWS 4

typedef struct {
    WSHDR *ws;
    IMGHDR *icon;
    void (*proc)();
} SIE_MENU_ITEM;

typedef struct {
    SIE_MENU_ITEM *items;
    unsigned int n_items;
    int col, row;
    int max_cols, max_rows;
} SIE_MENU;

#define Sie_Menu_Index(col, row) (row * SIE_MENU_MAX_COLS + col)

SIE_MENU *Sie_Menu_Init(SIE_MENU_ITEM *items, int n_items, int col, int row);
void Sie_Menu_Destroy(SIE_MENU *menu);

void Sie_Menu_Draw(SIE_MENU *menu);
void Sie_Menu_Redraw(SIE_MENU *menu);
void Sie_Menu_OnKey(SIE_MENU *menu, GUI_MSG *msg);

#endif
