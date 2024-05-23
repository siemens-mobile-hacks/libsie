#include <swilib.h>
#include <stdlib.h>
#include <string.h>

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
    WSHDR *ws = AllocWS((ssize_t)len);
    str_2ws(ws, file_name, len);
    int uid = GetExtUidByFileName_ws(ws);
    FreeWS(ws);
    return uid;
}

int Sie_Ext_MimeTypeCompare(const char *file_name, const char *type) {
    WSHDR *mime_type = AllocWS(32);
    if (GetMimeType(Sie_Ext_GetExtUidByFileName(file_name), mime_type, 0)) {
        size_t len = strlen(type);
        WSHDR *ws = AllocWS((ssize_t)len);
        wsprintf(ws, type);
        if (wstrncmp(mime_type, ws, len) == 0) {
            return 1;
        }
        FreeWS(ws);
    }
    FreeWS(mime_type);
    return 0;
}
