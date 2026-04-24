#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/tty.h>
#include <sysutil/sysutil_msgdialog.h>

#define ERROR_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_ERROR | CELL_MSGDIALOG_TYPE_SE_MUTE_OFF | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL);

static inline char *TrimEnd(char *str) {
    if (str == NULL) return NULL;
    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

static inline void println(const char *str) {
    sys_tty_write(SYS_TTYP_PPU_STDOUT, str, strlen(str), NULL);
}

#endif // UTIL_H
