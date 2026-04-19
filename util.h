#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/tty.h>

inline char* trimEnd(char* str) {
    if (str == NULL) return NULL;
    if (*str == '\0') return str;

    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

inline void println(const char* str) {
    sys_tty_write(SYS_TTYP_PPU_STDOUT, str, strlen(str), NULL);
}

#endif // UTIL_H
