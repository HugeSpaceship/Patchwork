#ifndef UTIL_H
#define UTIL_H

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <sys/tty.h>
#include <sysutil/sysutil_msgdialog.h>
#include <cell/cell_fs.h>

#include "printf.h"

#define ERROR_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_ERROR | CELL_MSGDIALOG_TYPE_SE_MUTE_OFF | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL);
#define INFO_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL)
#define OPTION_DIALOG(text, callback) cellMsgDialogOpen2(CELL_MSGDIALOG_BUTTON_TYPE_YESNO, text, callback, NULL, NULL)

#define MAX_PRINTF_BUF 128

#define DEFAULT_LOG_PATH "/dev_hdd0/plugins/patchwork/patchwork.log"

char *TrimEnd(char *str);

void ReplaceNext(char *str, char target, char c);

void print(const char *fmt, ...);
void println(const char *fmt, ...);

void InitLogger();
void DestroyLogger();
void Log(const char *fmt, ...);
void LogLn(const char *fmt, ...);

#endif // UTIL_H
