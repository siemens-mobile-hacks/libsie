#include <swilib.h>
#include <stdlib.h>
#include "../../include/sie/gui/gui.h"
#include "../../include/sie/resources.h"

extern IMGHDR *SIE_RES_IMG_WALLPAPER;

#define FONT_SIZE 18
#define FONT_SIZE_EMPTY 22
#define COLOR_SELECT_BG {0x00, 0x00, 0x00, 0x35}
#define COLOR_INDICATOR_BG {0x00, 0x00, 0x00, 0x35}
#define COLOR_INDICATOR {0xFF, 0xFF, 0x00, 0x64}
#define COLOR_EMPTY_TEXT {0xFF, 0xFF, 0xFF}

static void DrawSSBG(int x, int y, int x2, int y2);

SIE_MENU_LIST *Sie_Menu_List_Init(unsigned int gui_id) {
    SIE_MENU_LIST *menu = malloc(sizeof(SIE_MENU_LIST));
    zeromem(menu, sizeof(SIE_MENU_LIST));
    SIE_FT_SCROLL_STRING *ss = malloc(sizeof(SIE_FT_SCROLL_STRING));
    zeromem(ss, sizeof(SIE_FT_SCROLL_STRING));
    ss->font_size = FONT_SIZE;
    ss->attr = SIE_FT_TEXT_ALIGN_LEFT;
    ss->color = NULL;
    ss->gui_id = gui_id;
    ss->OnBeforeDraw = DrawSSBG;
    menu->ss = ss;
    return menu;
}

void LoadIcon2(SIE_MENU_LIST_ITEM *item) {
    char name[64];
    zeromem(name, 1);
    switch (item->type) {
        case SIE_MENU_LIST_ITEM_TYPE_CHECKBOX:
            if (!item->flag) {
                strcpy(name, "checkbox");
            } else {
                strcpy(name, "checkbox-checked");
            }
    }
    if (name[0]) {
        SIE_RESOURCES_IMG *res_img = Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_ACTIONS, 24, name);
        if (res_img) {
            item->icon2 = res_img->icon;
        }
    }
}

void Sie_Menu_List_AddItem(SIE_MENU_LIST *menu, SIE_MENU_LIST_ITEM *item, const char *name) {
    SIE_MENU_LIST_ITEM *menu_item;
    menu->items = realloc(menu->items, sizeof(SIE_MENU_LIST_ITEM) * (menu->n_items + 1));
    menu_item = &(menu->items[menu->n_items]);
    memcpy(menu_item, item, sizeof(SIE_MENU_LIST_ITEM));

    // icon2
    LoadIcon2(menu_item);
    // ws
    size_t len = strlen(name);
    menu_item->ws = AllocWS(len);
    if (name[0] >= 65) { // cp1251
        wsprintf(menu_item->ws, "%t", name);
    } else { // file_name
        str_2ws(menu_item->ws, name, len);
    }

    zeromem(item, sizeof(SIE_MENU_LIST_ITEM));
    menu->n_items++;
}

void Sie_Menu_List_SetItemType(SIE_MENU_LIST_ITEM *item, unsigned int type, unsigned int flag) {
    switch (type) {
        case SIE_MENU_LIST_ITEM_TYPE_CHECKBOX:
            item->type = type;
            item->flag = flag;
            LoadIcon2(item);
            break;
        default:
            break;
    }
}

void Sie_Menu_List_Destroy(SIE_MENU_LIST *menu) {
    if (menu) {
        if (menu->items) {
            for (unsigned int i = 0; i < menu->n_items; i++) {
                SIE_MENU_LIST_ITEM *item = &(menu->items[i]);
                FreeWS(item->ws);
            }
            mfree(menu->items);
        }
        SIE_FT_SCROLL_STRING *ss = menu->ss;
        GBS_DelTimer(&(ss->tmr));
        mfree(ss);
        mfree(menu);
    }
}

/**********************************************************************************************************************/

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

static void DrawSSBG(int x, int y, int x2, int y2) {
    const char color_bg[] = COLOR_SELECT_BG;
    Sie_GUI_DrawBleedIMGHDR(SIE_RES_IMG_WALLPAPER, x, y, x2, y2, x, y);
    DrawRectangle(x, y, x2, y2, 0, color_bg, color_bg);
}

IMGHDR *GetIcon(const SIE_MENU_LIST_ITEM *item) {
    switch (item->type) {
        case SIE_MENU_LIST_ITEM_TYPE_CHECKBOX:
            return item->icon2;
        default:
            return item->icon;
    }
}

void Sie_Menu_List_DrawMenu(SIE_MENU_LIST *menu) {
#define show_indicator (menu->n_items > SIE_MENU_LIST_MAX_ITEMS)
#define is_select (i == (menu->row - menu->offset))

    const char color_select_bg[] = COLOR_SELECT_BG;

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
        IMGHDR *img = GetIcon(item);
        WSHDR *ws = item->ws;

        const int x = 3;
        const int x2 = item_w;
        const float y = (float)(YDISP + header_h + 1 + (float)v_offset / 2) + (float)i * item_h;
        const float y2 = y + item_h - (float)v_offset / 2;
        const float text_y = y + (item_h - (float)Sie_FT_GetMaxHeight(FONT_SIZE)) / 2;

        int text_x = x + 8;
        int text_x2 = x2 - 8;
        int icon_x = 0;
        float icon_y = 0;

        if (is_select) {
            DrawRoundedFrame(x, (int)y, x2, (int)y2,
                             6, 6, 0,
                             color_select_bg, color_select_bg);
        }
        if (img) {
            icon_x = text_x;
            text_x = icon_x + img->w + 6;
            icon_y = y + (item_h - (float)img->h) / 2;
            Sie_GUI_DrawIMGHDR(img, icon_x, (int)icon_y, img->w, img->h);
        }
        if (is_select) {
            SIE_FT_SCROLL_STRING *ss = menu->ss;
            ss->ws = ws;
            ss->x = text_x;
            ss->y = (int)text_y;
            ss->x2 = text_x2;
            ss->y2 = 0;
            ss->color = item->color;
            Sie_FT_DrawBoundingScrollString(menu->ss, &(ss->tmr));
        } else {
            Sie_FT_DrawBoundingString(ws, text_x, (int)text_y, text_x2, 0,
                                      FONT_SIZE, SIE_FT_TEXT_ALIGN_LEFT, item->color);
        }
    }
    if (show_indicator) {
        Sie_Menu_List_DrawIndicator(menu);
    }
}

void Sie_Menu_List_DrawEmpty() {
    Sie_GUI_DrawBleedIMGHDR(SIE_RES_IMG_WALLPAPER, 0, YDISP, SCREEN_X2, SCREEN_Y2, 0, YDISP);

    WSHDR *ws = AllocWS(16);
    wsprintf(ws, "%t", "<Empty>");
    unsigned int w = 0, h = 0;
    Sie_FT_GetStringSize(ws, FONT_SIZE_EMPTY, &w, &h);
    const int x = ScreenW() / 2 - (int)w / 2;
    const int y = YDISP + HeaderH() + (ScreenH() - YDISP - HeaderH()) / 2 - (int)w / 2;
    const char color[] = COLOR_EMPTY_TEXT;
    Sie_FT_DrawString(ws, x, y, FONT_SIZE_EMPTY, color);
    FreeWS(ws);
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
            SIE_MENU_LIST_ITEM *menu_item = &(menu->items[menu->row]);
            void (*proc)(void *menu_item, unsigned int row) = menu_item->proc;

            switch (msg->gbsmsg->submess) {
                case SIE_MENU_LIST_KEY_PREV:
                    GBS_DelTimer(&(menu->ss->tmr));
                    if (menu->row != 0) {
                        menu->row--;
                    } else {
                        menu->row = menu->n_items - 1;
                    }
                    Sie_Menu_List_Refresh(menu);
                    Sie_Menu_List_Draw(menu);
                    break;
                case SIE_MENU_LIST_KEY_NEXT:
                    GBS_DelTimer(&(menu->ss->tmr));
                    menu->row++;
                    if (menu->row >= menu->n_items) {
                        menu->row = 0;
                    }
                    Sie_Menu_List_Refresh(menu);
                    Sie_Menu_List_Draw(menu);
                    break;
                case SIE_MENU_LIST_KEY_ENTER:
                    if (proc) {
                        proc(menu_item, menu->row);
                    }
                    break;
            }
        }
    }
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

unsigned int Sie_Menu_List_GetIdByName_ws(SIE_MENU_LIST *menu, WSHDR *ws, unsigned int *err) {
    unsigned int i = 0;
    while (i < menu->n_items) {
        if (wstrcmp(menu->items[i].ws, ws) == 0) {
            return i;
        }
        i++;
    }
    *err = 1;
    return 0;
}

unsigned int Sie_Menu_List_SetRow(SIE_MENU_LIST *menu, unsigned int row) {
    if (row < menu->n_items) {
        menu->row = row;
    } else {
        menu->row = menu->n_items - 1;
    }
    return menu->row;
}
