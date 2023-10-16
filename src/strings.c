#include <ctype.h>
#include <swilib.h>
#include <stdlib.h>

void Sie_Strings_ToLower(char *str) {
    for (unsigned int i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}
