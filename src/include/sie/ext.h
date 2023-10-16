#ifndef __SIE_EXT_H__
#define __SIE_EXT_H__

#define SIE_EXT_UID_JPG 0x26
#define SIE_EXT_UID_PNG 0x27

char *Sie_Ext_GetExtByFileName(const char *file_name);
int Sie_Ext_GetExtUidByFileName(const char *file_name);

#endif
