#include <ctype.h>
#include <string.h>

void Sie_Strings_ToLower(char *str) {
    for (unsigned int i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
}
