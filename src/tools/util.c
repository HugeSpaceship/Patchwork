#include "util.h"

static int LogFP; // Static global file pointer

char *TrimEnd(char *str) {
    if (str == NULL) return NULL;
    if (*str == '\0') return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

// TODO: Create some kind of vprint function to not repeat printf, buf alloc, and format string len code
// We still sadly have to repeat all the stdarg code for every function because of the genius who designed `...` syntax

void Print(const char *fmt, ...) {
    char print_buf[MAX_PRINTF_BUF];
    va_list args;
    va_start(args, fmt);

    size_t print_len = Vsnprintf(print_buf, MAX_PRINTF_BUF, fmt, args);

    if (print_len < 0) {
        return; // TODO: Log failed log (the string was too big)
    }

    sys_tty_write(SYS_TTYP_PPU_STDOUT, print_buf, print_len, NULL);
}

void PrintLn(const char *fmt, ...) {
    char print_buf[MAX_PRINTF_BUF];
    va_list args;
    va_start(args, fmt);

    size_t print_len = Vsnprintf(print_buf, MAX_PRINTF_BUF - 1, fmt, args);

    if (print_len < 0) {
        return;
    }

    print_buf[print_len++] = '\n';
    print_buf[print_len] = '\0';

    sys_tty_write(SYS_TTYP_PPU_STDOUT, print_buf, print_len, NULL);
}

// TODO: Implement some type of log level enumerator and automatically print string versions of the variants

void InitLogger() {
    cellFsOpen(DEFAULT_LOG_PATH, CELL_FS_O_WRONLY | CELL_FS_O_CREAT | CELL_FS_O_TRUNC, &LogFP, NULL, 0);
}

void DestroyLogger() {
    cellFsClose(LogFP);
}

void Log(const char *fmt, ...) {
    char print_buf[MAX_PRINTF_BUF];
    va_list args;
    va_start(args, fmt);

    size_t print_len = Vsnprintf(print_buf, MAX_PRINTF_BUF, fmt, args);

    if (print_len < 0) {
        return;
    }

    cellFsWrite(LogFP, print_buf, print_len, NULL);
    sys_tty_write(SYS_TTYP_PPU_STDOUT, print_buf, print_len, NULL);
}

void LogLn(const char *fmt, ...) {
    char print_buf[MAX_PRINTF_BUF];
    va_list args;
    va_start(args, fmt);

    size_t print_len = Vsnprintf(print_buf, MAX_PRINTF_BUF - 1, fmt, args);

    if (print_len < 0) {
        return;
    }

    print_buf[print_len++] = '\n';
    print_buf[print_len] = '\0';

    cellFsWrite(LogFP, print_buf, print_len, NULL);
    sys_tty_write(SYS_TTYP_PPU_STDOUT, print_buf, print_len, NULL);
}

void ReplaceNext(char *str, char target, char c) {
    size_t i = 0;
    while (str[i] != target) {
        if (str[i] == '\0') {
            return;
        }
        
        i++;
    }

    str[i] = c;
}
