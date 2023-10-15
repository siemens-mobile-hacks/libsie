#include <swilib.h>
#include <stdlib.h>
#include "../../include/sie/gui/gui.h"
#include "../../include/sie/freetype/freetype.h"

extern IMGHDR *SIE_RES_IMG_WALLPAPER;

#define FONT_SIZE 18
#define FONT_SIZE_EMPTY 22
#define COLOR_SELECT_BG {0x00, 0x00, 0x00, 0x35}
#define COLOR_SELECT_TEXT {0xFF, 0xFF,  0xFF}
#define COLOR_INDICATOR_BG {0x00, 0x00, 0x00, 0x35}
#define COLOR_INDICATOR {0xFF, 0xFF, 0x00, 0x64}
#define COLOR_EMPTY_TEXT {0xFF, 0xFF, 0xFF}

SIE_MENU_LIST *Sie_Menu_List_Init(SIE_MENU_LIST_ITEM *items, unsigned int n_items) {
    SIE_MENU_LIST *menu = malloc(sizeof(SIE_MENU_LIST));
    zeromem(menu, sizeof(SIE_MENU_LIST));
    menu->items = items;
    menu->n_items = n_items;
    return menu;
}

void Sie_Menu_List_Destroy(SIE_MENU_LIST *menu) {
    if (menu) {
        if (menu->items) {
            SIE_MENU_LIST_ITEM *item;
            for (unsigned int i = 0; i < menu->n_items; i++) {
                item = &(menu->items[i]);
                FreeWS(item->ws);
            }
            mfree(menu->items);
        }
        mfree(menu);
    }
}

void Sie_Menu_List_DrawIndicator(SIE_MENU_LIST *menu) {
    const char color[] = COLOR_INDICATOR;
    const char color_bg[] = COLOR_INDICATOR_BG;

    const int bg_h_offset = 2;
    const int bg_v_offset = 2;
    const int bg_w = 2;
    const int bg_h = ScreenH() - HeaderH() - YDISP - bg_v_offset * 2;

    const int bg_x = SCREEN_X2 - bg_h_offset - bg_w;
    const int bg_y = YDISP + HeaderH() + bg_v_offset;
    const int bg_x2 = SCREEN_X2 - bg_h_offset;
    const int bg_y2 = bg_y + bg_h;
    DrawRectangle(bg_x, bg_y, ScreenW() - bg_h_offset, bg_y2, 1, color_bg, color_bg);

    const int indicator_x = bg_x - 1;
    const int indicator_x2 = bg_x2 + 1;
    const float indicator_h = (float)bg_h / (float)menu->n_items;
    const float indicator_y = (float)bg_y + indicator_h * (float)menu->row;
    const float indicator_y2 = indicator_y + indicator_h;
    DrawRectangle(indicator_x, (int)(indicator_y), indicator_x2, (int)(indicator_y2), 0, color, color);
}

void Sie_Menu_List_DrawMenu(SIE_MENU_LIST *menu) {
#define show_indicator (menu->n_items > SIE_MENU_LIST_MAX_ITEMS)
    const char color_select_bg[] = COLOR_SELECT_BG;
    const char color_select_text[] = COLOR_SELECT_TEXT;

    const int h_offset = 3;
    const int v_offset = 2;
    const int header_h = HeaderH();
    const int width = ScreenW();
    const int height = ScreenH() - 1 - YDISP - header_h - v_offset;
    const float item_h = (float)height / SIE_MENU_LIST_MAX_ITEMS;
    const int item_w = (show_indicator) ? width - 6 - 1 - h_offset : width - 1 - h_offset;

    Sie_GUI_DrawBleedIMGHDR(SIE_RES_IMG_WALLPAPER, 0, 0 + YDISP + HeaderH(), SCREEN_X2, SCREEN_Y2, 0, YDISP + HeaderH());

    int c = min(menu->n_items, SIE_MENU_LIST_MAX_ITEMS);
    for (unsigned int i = 0; i < c; i++) {
        const SIE_MENU_LIST_ITEM *item = &(menu->items[menu->offset + i]);
        IMGHDR *img = item->icon;
        WSHDR *ws = item->ws;

        const int x = 3;
        const int x2 = item_w;
        const float y = (float)(YDISP + header_h + 1 + (float)v_offset / 2) + (float)i * item_h;
        const float y2 = y + item_h - (float)v_offset / 2;
        const float text_y = y + (item_h - (float)Sie_FT_GetMaxHeight(FONT_SIZE)) / 2;

        char *color_text = NULL;
        int text_x = x + 8;
        int text_x2 = x2 - 8;
        int icon_x = 0;
        float icon_y = 0;
        if (i == (menu->row - menu->offset)) {
            DrawRoundedFrame(x, (int)y, x2, (int)y2,
                             6, 6, 0,
                             color_select_bg, color_select_bg);
            color_text = (char*)color_select_text;
        }
        if (img) {
            icon_x = text_x;
            text_x = icon_x + img->w + 6;
            icon_y = y + (item_h - (float)img->h) / 2;
            Sie_GUI_DrawIMGHDR(img, icon_x, (int)icon_y, img->w, img->h);
        }
        Sie_FT_DrawBoundingString(ws, text_x, (int)text_y, text_x2, 0, FONT_SIZE, SIE_FT_TEXT_ALIGN_LEFT, color_text);
    }
    if (show_indicator) {
        Sie_Menu_List_DrawIndicator(menu);
    }
}

void Sie_Menu_List_DrawEmpty() {
    Sie_GUI_DrawBleedIMGHDR(SIE_RES_IMG_WALLPAPER, 0, YDISP, SCREEN_X2, SCREEN_Y2, 0, YDISP);

    WSHDR *ws = AllocWS(16);
    wsprintf(ws, "%t", "<Пусто>");
    unsigned int w = 0, h = 0;
    Sie_FT_GetStringSize(ws, FONT_SIZE_EMPTY, &w, &h);
    const int x = ScreenW() / 2 - (int)w / 2;
    const int y = YDISP + HeaderH() + (ScreenH() - YDISP - HeaderH()) / 2 - (int)w / 2;
    const char color[] = COLOR_EMPTY_TEXT;
    Sie_FT_DrawString(ws, x, y, FONT_SIZE_EMPTY, color);
    FreeWS(ws);
}

void Sie_Menu_List_Refresh(SIE_MENU_LIST *menu) {
    if (menu->n_items > SIE_MENU_LIST_MAX_ITEMS) {
        if (menu->row >= (unsigned int)(SIE_MENU_LIST_MAX_ITEMS / 2)) {
            if (menu->row < menu->n_items - (unsigned int)(SIE_MENU_LIST_MAX_ITEMS / 2)) {
                menu->offset = menu->row - (unsigned int)(SIE_MENU_LIST_MAX_ITEMS) / 2;
            } else {
                menu->offset = menu->n_items - SIE_MENU_LIST_MAX_ITEMS;
            }
        } else {
            menu->offset = 0;
        }
    }
}

void Sie_Menu_List_Draw(SIE_MENU_LIST *menu) {
    if (menu->n_items) {
        Sie_Menu_List_DrawMenu(menu);
    } else {
        Sie_Menu_List_DrawEmpty();
    }
}

void Sie_Menu_List_OnKey(SIE_MENU_LIST *menu, GUI_MSG *msg) {
    if (msg->gbsmsg->msg == KEY_DOWN || msg->gbsmsg->msg == LONG_PRESS) {
        if (menu->n_items) {
            switch (msg->gbsmsg->submess) {
                case SIE_MENU_LIST_KEY_PREV:
                    if (menu->row != 0) {
                        menu->row--;
                    } else {
                        menu->row = menu->n_items - 1;
                    }
                    Sie_Menu_List_Refresh(menu);
                    Sie_Menu_List_Draw(menu);
                    break;
                case SIE_MENU_LIST_KEY_NEXT:
                    menu->row++;
                    if (menu->row >= menu->n_items) {
                        menu->row = 0;
                    }
                    Sie_Menu_List_Refresh(menu);
                    Sie_Menu_List_Draw(menu);
                    break;
                case SIE_MENU_LIST_KEY_ENTER:
                    if (menu->n_items) {
                        void (*proc)();
                        proc = menu->items[menu->row].proc;
                        if (proc) {
                            proc();
                        }
                    }
                    break;
            }
        }
    }
}
