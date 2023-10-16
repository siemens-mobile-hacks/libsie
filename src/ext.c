#include <swilib.h>

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
