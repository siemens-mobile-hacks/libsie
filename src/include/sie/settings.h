#ifndef __SIE_SETTINGS_H__
#define __SIE_SETTINGS_H__

#include <swilib.h>

int Settings_UpdateWS(WSHDR *ws, const char *entry, const char *keyword);
int Settings_ReadWS(WSHDR *ws, const char *entry, const char *keyword);

#endif
