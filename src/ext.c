#include <swilib.h>
#include <stdlib.h>

char *Sie_Ext_GetExtByFileName(const char *file_name) {
    char *p = strrchr(file_name, '.');
    if (p) {
        p++;
        size_t len = strlen(file_name) - (p - file_name);
        if (len) {
            char *ext = malloc(len + 1);
            strncpy(ext, p, len);
            ext[len] = '\0';
            return ext;
        }
    }
    return NULL;
}

int Sie_Ext_GetExtUidByFileName(const char *file_name) {
    size_t len = strlen(file_name);
    WSHDR *ws = AllocWS(len);
    str_2ws(ws, file_name, len);
    int uid = GetExtUidByFileName_ws(ws);
    FreeWS(ws);
    return uid;
}
