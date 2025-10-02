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

void IntToStr(char *buf, int value, size_t digits) {
    int isNegative = value < 0; // Store this early
    char temp[digits + isNegative];
    int i = 0;

    while (value > 0 || i < digits) {
        temp[i++] = '0' + (value % 10); // Writes the digits to the buffer backwards
        value /= 10;
    }
    if (isNegative) {
        temp[i++] = '-';
    }

    // Reverse the buffer
    for (int j = 0; j < i; j++) {
        buf[j] = temp[i - j - 1];
    }

    buf[i] = '\0'; // Terminate, might cause issues with appending the buffer elsewhere
}
