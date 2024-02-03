#include <swilib.h>
#include <stdlib.h>
#include <sys/param.h>
#include "../include/sie/gui/gui.h"
#include "../include/sie/resources.h"

#define FONT_SIZE_MSG 20
#define FONT_SIZE_SOFT_KEYS 18
#define COLOR_BG {0x00, 0x00, 0x00, 0x64}
#define COLOR_BORDER {0x18, 0x18, 0x18, 0x64}
#define COLOR_SURFACE_BG {0x00, 0x00, 0x00, 0x20}

RECT canvas;
IMGHDR *IMG_YES;

static void OnRedraw(SIE_GUI_BOX *data) {
    const char color_bg[] = COLOR_BG;
    const char color_border[] = COLOR_BORDER;
    const char color_surface_bg[] = COLOR_SURFACE_BG;

    IMGHDR *scrot = &(data->surface->scrot);

    Sie_GUI_DrawIMGHDR(scrot, 0, 0, scrot->w, scrot->h);
    Sie_GUI_DrawIconBar();
    DrawRectangle(0, YDISP, SCREEN_X2, SCREEN_Y2, 0,
                  GetPaletteAdrByColorIndex(23), color_surface_bg);

    const int w_window = ScreenW() - 8;
    const int h_window = 170;
    const int x = (ScreenW() - w_window) / 2;
    const int x2 = x + w_window;
    const int y = (YDISP + (ScreenH() - YDISP) / 2) - h_window / 2;
    const int y2 = y + h_window;
    DrawRectangle(x, y, x2, y2, 0,color_border, color_bg);

    const int y2_sk = y2 - 5;
    const int y2_msg = y2 - 5;
    Sie_FT_DrawBoundingString(data->msg_ws, x, y, x2, y2_msg,FONT_SIZE_MSG,
                              SIE_FT_TEXT_ALIGN_CENTER + SIE_FT_TEXT_VALIGN_MIDDLE, NULL);

    if (data->type <= SIE_GUI_BOX_TYPE_QUESTION) {
        int x_img = 0;
        int x2_img = 0;
        if (IMG_YES) {
            int y_img = 0;
            unsigned int w, h = 0, h1 = 0, h2 = 0;
            if (data->left_sk_ws) {
                Sie_FT_GetStringSize(data->left_sk_ws, FONT_SIZE_SOFT_KEYS, &w, &h1);
            }
            if (data->right_sk_ws) {
                Sie_FT_GetStringSize(data->right_sk_ws, FONT_SIZE_SOFT_KEYS, &w, &h2);
            }
            h = MAX(h1, h2);
            if (h) {
                x_img = 4 + (w_window / 2) - IMG_YES->w / 2;
                y_img = y2_sk - (int) h / 2 - IMG_YES->h / 2;
                x2_img = x_img + IMG_YES->w;
                Sie_GUI_DrawIMGHDR(IMG_YES, x_img, y_img, IMG_YES->w, IMG_YES->h);
            }
        }
        if (data->left_sk_ws) {
            int x_left_sk = x + 10;
            int x2_left_sk = x + (x2 - x) / 2 - 5;
            if (x_img) {
                x2_left_sk -= IMG_YES->w / 2;
            }
            Sie_FT_DrawBoundingString(data->left_sk_ws, x_left_sk, y, x2_left_sk, y2_sk,
                                      FONT_SIZE_SOFT_KEYS,
                                      SIE_FT_TEXT_ALIGN_LEFT + SIE_FT_TEXT_VALIGN_BOTTOM, NULL);
        }
        if (data->right_sk_ws) {
            int x_right_sk = x + (x2 - x) / 2 + 5;
            int x2_right_sk = x2 - 10;
            if (x2_img) {
                x_right_sk += IMG_YES->w / 2;
            }
            Sie_FT_DrawBoundingString(data->right_sk_ws, x_right_sk, y, x2_right_sk, y2_sk,
                                      FONT_SIZE_SOFT_KEYS,
                                      SIE_FT_TEXT_ALIGN_RIGHT + SIE_FT_TEXT_VALIGN_BOTTOM, NULL);
        }
    }
}

static void OnAfterDrawIconBar() {
    const char color_surface_bg[] = COLOR_SURFACE_BG;
    DrawRectangle(0, 0, SCREEN_X2, YDISP, 0,
                  GetPaletteAdrByColorIndex(23), color_surface_bg);
}

static void OnCreate(SIE_GUI_BOX *data, void *(*malloc_adr)(int)) {
    data->gui.state = 1;
}

static void OnClose(SIE_GUI_BOX *data, void (*mfree_adr)(void *)) {
    data->gui.state = 0;
    FreeWS(data->msg_ws);
    if (data->left_sk_ws) {
        FreeWS(data->left_sk_ws);
    }
    if (data->right_sk_ws) {
        FreeWS(data->right_sk_ws);
    }
    Sie_GUI_Surface_Destroy(data->surface);
}

static void OnFocus(SIE_GUI_BOX *data, void *(*malloc_adr)(int), void (*mfree_adr)(void *)) {
    data->gui.state = 2;
    Sie_GUI_Surface_OnFocus(data->surface);
}

static void OnUnFocus(SIE_GUI_BOX *data, void (*mfree_adr)(void *)) {
    if (data->gui.state != 2) return;
    data->gui.state = 1;
    Sie_GUI_Surface_OnUnFocus(data->surface);
}

static int _OnKey(SIE_GUI_BOX *data, GUI_MSG *msg) {
    if (msg->gbsmsg->msg == KEY_DOWN || msg->gbsmsg->msg == LONG_PRESS) {
        if (data->type <= SIE_GUI_BOX_TYPE_ERROR) {
            if (msg->gbsmsg->submess == LEFT_SOFT || msg->gbsmsg->submess == ENTER_BUTTON) {
                return 1;
            }
        } else {
            if (data->callback.proc) {
                switch (msg->gbsmsg->submess) {
                    case LEFT_SOFT: case ENTER_BUTTON:
                        data->callback.proc(SIE_GUI_BOX_CALLBACK_YES, data->callback.data);
                        return 1;
                    case RIGHT_SOFT:
                        data->callback.proc(SIE_GUI_BOX_CALLBACK_NO, data->callback.data);
                        return 1;
                }
            }
        }
    }
    return 0;
}

static int OnKey(SIE_GUI_BOX *data, GUI_MSG *msg) {
    return Sie_GUI_Surface_OnKey(data->surface, data, msg);
}

extern void kill_data(void *p, void (*func_p)(void *));

static int method8(void) { return 0; }

static int method9(void) { return 0; }

static const void *const gui_methods[11] = {
        (void *)OnRedraw,
        (void *)OnCreate,
        (void *)OnClose,
        (void *)OnFocus,
        (void *)OnUnFocus,
        (void *)OnKey,
        0,
        (void *)kill_data,
        (void *)method8,
        (void *)method9,
        0
};

SIE_GUI_BOX *Sie_GUI_Box(unsigned int type, SIE_GUI_BOX_TEXT *text, SIE_GUI_BOX_CALLBACK *callback) {
    SIE_GUI_BOX *gui = malloc(sizeof(SIE_GUI_BOX));
    const SIE_GUI_SURFACE_HANDLERS surface_handlers = {
            OnAfterDrawIconBar,
            (int(*)(void *, GUI_MSG *msg))_OnKey,
    };
    zeromem(gui, sizeof(SIE_GUI_BOX));
    gui->type = type;
    gui->msg_ws = AllocWS(128);
    wsprintf(gui->msg_ws, "%t", text->msg);
    if (type <= SIE_GUI_BOX_TYPE_QUESTION) {
        SIE_RESOURCES_IMG *res_img = Sie_Resources_LoadImage(SIE_RESOURCES_TYPE_ACTIONS, 24, "yes");
        if (res_img) {
            IMG_YES = res_img->icon;
        }
        if (text->left_sk) {
            gui->left_sk_ws = AllocWS((int)strlen(text->left_sk));
            wsprintf(gui->left_sk_ws, "%t", text->left_sk);
        }
        if (text->right_sk) {
            gui->right_sk_ws = AllocWS((int)strlen(text->right_sk));
            wsprintf(gui->right_sk_ws, "%t", text->right_sk);
        }
        memcpy(&(gui->callback), callback, sizeof(SIE_GUI_BOX_CALLBACK));
    }
    LockSched();
    Sie_GUI_InitCanvas(&canvas);
    gui->gui.canvas = (RECT*)(&canvas);
    gui->gui.methods = (void*)gui_methods;
    gui->gui.item_ll.data_mfree = (void (*)(void *))mfree_adr();
    gui->surface = Sie_GUI_Surface_Init(SIE_GUI_SURFACE_TYPE_DEFAULT, &surface_handlers,
                                        CreateGUI(gui));
    Sie_GUI_Surface_DoScrot(gui->surface);
    UnlockSched();
    return gui;
}

SIE_GUI_BOX *Sie_GUI_MsgBox(const char *msg) {
    SIE_GUI_BOX_TEXT text = {msg, "Ok", NULL};
    return Sie_GUI_Box(SIE_GUI_BOX_TYPE_STANDARD, &text, NULL);
}

SIE_GUI_BOX *Sie_GUI_MsgBoxOk(const char *msg) {
    SIE_GUI_BOX_TEXT text = {msg,"Ok", NULL};
    return Sie_GUI_Box(SIE_GUI_BOX_TYPE_OK, &text, NULL);
}

SIE_GUI_BOX *Sie_GUI_MsgBoxError(const char *msg) {
    SIE_GUI_BOX_TEXT text = {msg,"Ok", NULL};
    return Sie_GUI_Box(SIE_GUI_BOX_TYPE_OK, &text, NULL);
}

SIE_GUI_BOX *Sie_GUI_MsgBoxYesNo(const char *msg, SIE_GUI_BOX_CALLBACK *callback) {
    SIE_GUI_BOX_TEXT text = {msg, "Yes", "No"};
    return Sie_GUI_Box(SIE_GUI_BOX_TYPE_QUESTION, &text, callback);
}

SIE_GUI_BOX *Sie_GUI_WaitBox(const char *msg) {
    SIE_GUI_BOX_TEXT text = {(msg) ? msg : "Wait...", NULL, NULL};
    return Sie_GUI_Box(SIE_GUI_BOX_TYPE_WAIT, &text, NULL);
}
