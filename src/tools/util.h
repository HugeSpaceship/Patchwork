#ifndef UTIL_H
#define UTIL_H

#include <cstdio>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <sys/tty.h>
#include <sysutil/sysutil_msgdialog.h>
#include <wchar.h>

#include "printf.h"

#define ERROR_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_ERROR | CELL_MSGDIALOG_TYPE_SE_MUTE_OFF | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL);
#define INFO_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL)
#define OPTION_DIALOG(text, callback) cellMsgDialogOpen2(CELL_MSGDIALOG_BUTTON_TYPE_YESNO, text, callback, NULL, NULL)

#define MAX_PRINTF_BUF 128

static inline char *TrimEnd(char *str) {
    if (str == NULL) return NULL;
    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

static inline void print(const char *fmt, ...) {
    char print_buf[MAX_PRINTF_BUF];
    va_list args;
    va_start(args, fmt);

    vsnprintf(print_buf, MAX_PRINTF_BUF, fmt, args);
    sys_tty_write(SYS_TTYP_PPU_STDOUT, print_buf, strlen(print_buf), NULL);
}

static inline void println(const char *fmt, ...) {
    char print_buf[MAX_PRINTF_BUF];
    va_list args;
    va_start(args, fmt);

    vsnprintf(print_buf, MAX_PRINTF_BUF, fmt, args);
    sys_tty_write(SYS_TTYP_PPU_STDOUT, "\n", 1, NULL);
}

static inline void ReplaceNext(char *str, char target, char c) {
    size_t i = 0;
    while (str[i] != target) {
        if (str[i] == '\0') {
            return;
        }
        
        i++;
    }

    str[i] = c;
}

#endif // UTIL_H
