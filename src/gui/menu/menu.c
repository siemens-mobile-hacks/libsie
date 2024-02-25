#include <swilib.h>
#include <stdlib.h>
#include <sys/param.h>
#include "../../include/sie/resources.h"
#include "../../include/sie/gui/gui.h"

SIE_MENU *Sie_Menu_Init(SIE_MENU_ITEM *items, int n_items, int col, int row) {
    SIE_MENU *menu = malloc(sizeof(SIE_MENU));
    menu->items = items;
    menu->n_items = n_items;
    menu->col = col;
    menu->row = row;
    menu->max_cols = MIN(n_items, SIE_MENU_MAX_COLS);
    menu->max_rows = (n_items + SIE_MENU_MAX_COLS - 1) / SIE_MENU_MAX_COLS;
    return menu;
}

void Sie_Menu_Destroy(SIE_MENU *menu) {
    if (menu) {
        if (menu->items) {
            SIE_MENU_ITEM *item;
            for (int i = 0; i < menu->n_items; i++) {
                item = &(menu->items[i]);
                FreeWS(item->ws);
                mfree(item->icon->bitmap);
                mfree(item->icon);
            }
        }
        mfree(menu);
    }
}

void Sie_Menu_Draw(SIE_MENU *menu) {
    const int width = ScreenW();
    const int height = ScreenH() - YDISP - HeaderH();
    const int icon_area_width = width / SIE_MENU_MAX_COLS;
    const int icon_area_height = height / SIE_MENU_MAX_ROWS;

    Sie_GUI_DrawBleedIMGHDR(Sie_Resources_GetWallpaperIMGHDR(),
                            0, YDISP + HeaderH(), ScreenW(), ScreenH(),
                            0, YDISP + HeaderH());

    int x = 0;
    int y = 0;
    SIE_MENU_ITEM *item = NULL;
    for (int i = 0, column = 0, row = 0; i < menu->n_items; i++) {
        if (row + 1 >= SIE_MENU_MAX_ROWS + 1) {
            break;
        }

        item = (SIE_MENU_ITEM *)&menu->items[i];
        x = 0 + column * icon_area_width;
        y = YDISP + HeaderH() + row * icon_area_height;
        x += (icon_area_width - item->icon->w) / 2;
        y += (icon_area_height - item->icon->h) / 2;

        Sie_GUI_DrawIMGHDR(item->icon, x, y, item->icon->w, item->icon->h);

        if ((column + 1) % SIE_MENU_MAX_COLS == 0) {
            row += 1;
            column = 0;
            continue;
        }
        column += 1;
    }
}

void Sie_Menu_Redraw(SIE_MENU *menu) {
    const int id_idle_menu = Sie_Menu_Index(menu->col, menu->row);
    Sie_GUI_DrawHeader(menu->items[id_idle_menu].ws);
    Sie_Menu_Draw(menu);
}

void Sie_Menu_OnKey(SIE_MENU *menu, GUI_MSG *msg) {
    if (msg->gbsmsg->msg == KEY_DOWN || msg->gbsmsg->msg == LONG_PRESS) {
        switch (msg->gbsmsg->submess) {
            case UP_BUTTON:
                menu->row--;
                if (menu->row < 0) {
                    menu->row = menu->max_rows - 1;
                }
                Sie_Menu_Redraw(menu);
            case RIGHT_BUTTON:
                menu->col++;
                if (menu->col >= menu->max_cols) {
                    menu->col = 0;
                }
                Sie_Menu_Redraw(menu);
                break;
            case DOWN_BUTTON:
                menu->row++;
                if (menu->row >= menu->max_rows) {
                    menu->row = 0;
                }
                Sie_Menu_Redraw(menu);
                break;
            case LEFT_BUTTON:
                menu->col--;
                if (menu->col < 0) {
                    menu->col = menu->max_cols - 1;
                }
                Sie_Menu_Redraw(menu);
                break;
            case ENTER_BUTTON:
                menu->items[menu->row].proc();
                break;
        }
    }
}
