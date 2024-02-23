#include <swilib.h>
#include <stdlib.h>
#include "../include/sie/resources.h"
#include "../include/sie/gui/gui.h"

#define TMR_MS_FOCUS (216 / 8)
#define COLOR_HEADER_BG {0x00, 0x00, 0x00, 0x35}

extern GBSTMR TMR_REDRAW_ICONBAR;

extern int CFG_FONT_SIZE_ICONBAR, CFG_FONT_SIZE_HEADER;

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
    char color_text_warning[] = SIE_COLOR_TEXT_WARNING;
    char color_text_error[] = SIE_COLOR_TEXT_ERROR;

    if (TMR_REDRAW_ICONBAR.param6) {
        surface = (SIE_GUI_SURFACE*)TMR_REDRAW_ICONBAR.param6;
    }
    if (IsTimerProc(&TMR_REDRAW_ICONBAR)) {
        GBS_StopTimer(&TMR_REDRAW_ICONBAR);
    }

    Sie_GUI_DrawIMGHDR(Sie_Resources_GetWallpaperIMGHDR(), 0, 0, ScreenW(), YDISP);

    int x = 0, y = 0;
    unsigned int w = 0, h = 0;
    WSHDR *ws = AllocWS(64);
    char *color = NULL;

    // battery
    int cap = *RamCap();
    if (cap < 10) {
        color = color_text_error;
    } else if (cap < 20) {
        color = color_text_warning;
    }
    wsprintf(ws, "%d%%", cap);
    Sie_FT_GetStringSize(ws, CFG_FONT_SIZE_ICONBAR, &w, &h);
    x = SCREEN_X2 - PADDING_ICONBAR - w;
    y = 0 + (YDISP - (int)h) / 2;
    Sie_FT_DrawString(ws, x, y, CFG_FONT_SIZE_ICONBAR, color);

    // clock
    TTime time;
    GetDateTime(NULL, &time);
    wsprintf(ws, "%02d:%02d", time.hour, time.min);
    Sie_FT_GetStringSize(ws, CFG_FONT_SIZE_ICONBAR, &w, &h);
    x = x - PADDING_ICONBAR - (int)w;
    y = 0 + (YDISP - (int)h) / 2;
    Sie_FT_DrawString(ws,  x, y, CFG_FONT_SIZE_ICONBAR, NULL);

    FreeWS(ws);
    if (surface) {
        if (surface->handlers.OnAfterDrawIconBar) {
            surface->handlers.OnAfterDrawIconBar();
        }
    }
    GBS_StartTimerProc(&TMR_REDRAW_ICONBAR, 216, Sie_GUI_DrawIconBar);
}

void Sie_GUI_DrawHeader(WSHDR *ws) {
    const int header_x = 0;
    const int header_y = YDISP;
    const int header_x2 = SCREEN_X2;
    const int header_y2 = header_y + HeaderH();
    const char color_bg[] = COLOR_HEADER_BG;
    Sie_GUI_DrawBleedIMGHDR(Sie_Resources_GetWallpaperIMGHDR(),
                            header_x, header_y, header_x2, header_y2,
                            0, YDISP);
    DrawRectangle(header_x, header_y, header_x2, header_y2 - 1, 0, color_bg, color_bg);

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
    const int y = YDISP;
    const int y2 = YDISP + HeaderH();
    if (wstrlen(ws_left)) {
        Sie_FT_DrawBoundingString(ws_left, 0 + PADDING_HEADER, y, ScreenW() / 2 - PADDING_HEADER, y2,
                                  CFG_FONT_SIZE_HEADER,
                                  SIE_FT_TEXT_ALIGN_LEFT | SIE_FT_TEXT_VALIGN_MIDDLE, NULL);
    }
    if (wstrlen(ws_right)) {
        Sie_FT_DrawBoundingString(ws_right,
                                  ScreenW() / 2 + PADDING_HEADER, y,ScreenW() - PADDING_HEADER, y2,
                                  CFG_FONT_SIZE_HEADER,
                                  SIE_FT_TEXT_ALIGN_RIGHT | SIE_FT_TEXT_VALIGN_MIDDLE, NULL);
    }
    FreeWS(ws_left);
    FreeWS(ws_right);
}
