#include <swilib.h>

int Settings_UpdateWS(WSHDR *ws, const char *entry, const char *keyword) {
    return SettingsAE_Update_ws(ws, 9, (char *)entry, (char *)keyword);
}

int Settings_ReadWS(WSHDR *ws, const char *entry, const char *keyword) {
    return SettingsAE_Read_ws(ws, 9, (char *)entry, (char *)keyword);
}
