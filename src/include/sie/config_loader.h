#ifndef __SIE_CONFIG_LOADER_H__
#define __SIE_CONFIG_LOADER_H__

#include <cfg_items.h>

int Sie_Config_Save(const char *path, CFG_HDR *cfghdr0, const int *config_begin, const int *config_end);
int Sie_Config_Load(const char *path, CFG_HDR *cfghdr0, const int *config_begin, const int *config_end);

#endif
