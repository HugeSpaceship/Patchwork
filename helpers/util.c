#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "util.h"

char *Trim(char *str) {
    if (str == NULL) return NULL;

    while (isspace((unsigned char)*str)) str++;

    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';

    return str;
}

// atoi() directly or indirectly involves _Errno and PRX can't use TLS variables because idk
int StrToInt(char *str) {
    int result = 0;
    int sign = 1;

    str = Trim(str);

    if (*str == '-') {
        sign = -1;
        str++;
    }

    // Yeah
    while (*str >= '0' && *str <= '9') {
        result = result  *10 + (*str - '0');
        str++;
    }

    return result  *sign;
}