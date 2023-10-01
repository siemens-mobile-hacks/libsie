#include <ctype.h>
#include <swilib.h>
#include <stdlib.h>

void Sie_Strings_ToLower(char *str) {
    for (unsigned int i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}

char *Sie_Strings_GetExtByFileName(const char *file_name) {
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
