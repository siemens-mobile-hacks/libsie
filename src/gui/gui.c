#include <swilib.h>
#include <stdlib.h>
#include "../include/sie/resources.h"
#include "../include/sie/gui/gui.h"

#define TMR_MS_FOCUS (216 / 8)
#define FONT_SIZE_ICONBAR 14
#define FONT_SIZE_HEADER 20
#define COLOR_HEADER_BG {0x00, 0x00, 0x00, 0x35}

extern IMGHDR *SIE_RES_IMG_WALLPAPER;
extern GBSTMR TMR_REDRAW_ICONBAR;

void Focus_GUI(GBSTMR *tmr) {
    SIE_GUI_FOCUS_DATA *data = (SIE_GUI_FOCUS_DATA*)tmr->param6;
    unsigned int gui_id = data->gui_id;
    FocusGUI((int)gui_id);
    if (IsGuiOnTop((int)gui_id)) {
        GBS_DelTimer(tmr);
        if (data->proc) {
            data->proc(data->data);
        }
    } else {
        GBS_StartTimerProc(tmr, TMR_MS_FOCUS, Focus_GUI);
    }
}

void Sie_GUI_FocusGUI(GBSTMR *tmr, SIE_GUI_FOCUS_DATA *data) {
    tmr->param6 = (int)data;
    Focus_GUI(tmr);
}

void Sie_GUI_InitCanvas(RECT *canvas) {
    canvas->x = 0;
    canvas->y = 0;
    canvas->x2 = SCREEN_X2;
    canvas->y2 = SCREEN_Y2;
}

void Sie_GUI_SetRGB(char *rgb, char r, char g, char b) {
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

IMGHDR *Sie_GUI_TakeScrot() {
    LockSched();
    IMGHDR *scrot = malloc(sizeof(IMGHDR));
    size_t size = CalcBitmapSize((short) ScreenW(), (short) ScreenH(), IMGHDR_TYPE_RGB565);
    scrot->w = ScreenW();
    scrot->h = ScreenH();
    scrot->bpnum = IMGHDR_TYPE_RGB565;
    scrot->bitmap = malloc(size);
    memcpy(scrot->bitmap, RamScreenBuffer(), size);
    UnlockSched();
    return scrot;
}

void Sie_GUI_DrawIMGHDR(IMGHDR *img, int x, int y, int w, int h) {
    RECT rc;
    DRWOBJ drwobj;
    StoreXYWHtoRECT(&rc, x, y, w, h);
    SetPropTo_Obj5(&drwobj, &rc, 0, img);
    SetColor(&drwobj, NULL, NULL);
    DrawObject(&drwobj);
}

void Sie_GUI_DrawBleedIMGHDR(IMGHDR *img, int x, int y, int x2, int y2, int bleed_x, int bleed_y) {
    RECT rc;
    DRWOBJ drwobj;
    StoreXYXYtoRECT(&rc, x, y, x2, y2);
    SetPropTo_Obj5(&drwobj, &rc, 0, img);
    SetProp2ImageOrCanvas(&drwobj, &rc, 0, img, bleed_x, bleed_y);
    SetColor(&drwobj, NULL, NULL);
    DrawObject(&drwobj);
}

void Sie_GUI_DrawIconBar() {
    const SIE_GUI_SURFACE *surface = NULL;
    if (TMR_REDRAW_ICONBAR.param6) {
        surface = (SIE_GUI_SURFACE*)TMR_REDRAW_ICONBAR.param6;
    }
    if (IsTimerProc(&TMR_REDRAW_ICONBAR)) {
        GBS_StopTimer(&TMR_REDRAW_ICONBAR);
    }
//    if (surface) {
//        TMR_REDRAW_ICONBAR.param6 = (unsigned int)surface;
//    }

    Sie_GUI_DrawIMGHDR(SIE_RES_IMG_WALLPAPER, 0, 0, ScreenW(), YDISP);

    int x = 0, y = 0;
    char img_name[64];
    unsigned int w = 0, h = 0;
    WSHDR *ws = AllocWS(64);

//    // free ram
//    wsprintf(ws, "%d %t", GetFreeRamAvail(), "Б");
//    Sie_FT_DrawString(ws,
//                      0 + PADDING_ICONBAR,
//                      0 + (YDISP - Sie_FT_GetMaxHeight(FONT_SIZE_ICONBAR)) / 2,
//                      FONT_SIZE_ICONBAR, NULL);
    // battery cap
    int percent = 0;
    const int cap = *RamCap();
    const unsigned short ls = *RamLS();
    if (cap <= 5) {
        percent = 0;
    } else if (cap <= 20) {
        percent = 20;
    } else if (cap <= 40) {
        percent = 40;
    } else if (cap <= 60) {
        percent = 60;
    } else if (cap <= 90) {
        percent = 80;
    } else if (cap <= 100) {
        percent = 100;
    }
    sprintf(img_name, "%s-%02d", "battery", percent);
    if (ls) {
        strcat(img_name, "-charging");
    }
    IMGHDR *img = Sie_Resources_LoadIMGHDR(SIE_RESOURCES_TYPE_STATUS, 24, img_name);
    if (img) {
        x = SCREEN_X2 - PADDING_ICONBAR - img->w;
        y = 0 + (YDISP - img->h) / 2;
        Sie_GUI_DrawIMGHDR(img, x, y, img->w, img->h);
    } else {
        wsprintf(ws, "%d%%", *RamCap());
        Sie_FT_GetStringSize(ws, FONT_SIZE_ICONBAR, &w, &h);
        x = SCREEN_X2 - PADDING_ICONBAR - w;
        y = 0 + (YDISP - (int)h) / 2;
        Sie_FT_DrawString(ws, x, y, FONT_SIZE_ICONBAR, NULL);
    }
    // clock
    TTime time;
    GetDateTime(NULL, &time);
    wsprintf(ws, "%02d:%02d", time.hour, time.min);
    Sie_FT_GetStringSize(ws, FONT_SIZE_ICONBAR, &w, &h);
    x = x - PADDING_ICONBAR - (int)w;
    y = 0 + (YDISP - (int)h) / 2;
    Sie_FT_DrawString(ws,  x, y, FONT_SIZE_ICONBAR, NULL);
    
    FreeWS(ws);
    if (surface) {
        if (surface->handlers.OnAfterDrawIconBar) {
            surface->handlers.OnAfterDrawIconBar();
        }
    }
    GBS_StartTimerProc(&TMR_REDRAW_ICONBAR, 216, Sie_GUI_DrawIconBar);
}

void Sie_GUI_DrawHeader(WSHDR *ws) {
    const char color_bg[] = COLOR_HEADER_BG;
    const int header_x = 0;
    const int header_y = YDISP;
    const int header_x2 = SCREEN_X2;
    const int header_y2 = header_y + HeaderH();
    Sie_GUI_DrawBleedIMGHDR(SIE_RES_IMG_WALLPAPER,
                            header_x, header_y, header_x2, header_y2,
                            0, YDISP);
    DrawRectangle(header_x, header_y, header_x2, header_y2 - 1, 0, color_bg, color_bg);

    int text_x = 0;
    int text_y = 0;

    char str[64];
    WSHDR *ws_left = AllocWS(64);
    WSHDR *ws_right = AllocWS(64);
    ws_2str(ws, str, wstrlen(ws));
    char *p = strchr(str, '\t');
    if (p) {
        const int len_left = p - str;
        if (len_left > 0) {
            str_2ws(ws_left, str, len_left);
        }
        str_2ws(ws_right, p + 1, strlen(str) - (p - str - 1));
    } else {
        wstrcpy(ws_left, ws);
    }
    text_x = 0 + PADDING_HEADER;
    text_y = YDISP + (HeaderH() - Sie_FT_GetMaxHeight(FONT_SIZE_HEADER)) / 2;
    if (wstrlen(ws_left)) {
        Sie_FT_DrawString(ws_left, text_x, text_y, FONT_SIZE_HEADER, NULL);
    }
    if (wstrlen(ws_right)) {
        unsigned int w = 0, h = 0;
        Sie_FT_GetStringSize(ws_right,  FONT_SIZE_HEADER, &w, &h);
        text_x = SCREEN_X2 - 1 - PADDING_HEADER - w;
        Sie_FT_DrawString(ws_right, text_x, text_y, FONT_SIZE_HEADER, NULL);
    }
    FreeWS(ws_left);
    FreeWS(ws_right);
}
