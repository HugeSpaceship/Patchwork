#include <string.h>
#include <stdarg.h>

#include "helpers/string.h"

void ReplaceNextInstanceOfChar(const char *input, char *buf, size_t bufSize, const char *replace, char target) {
    size_t replaceLen = strlen(replace);
    size_t i = 0;
    int replaced = 0;

    for (; *input && i < bufSize - 1; input++) {
        if (*input == target && !replaced) {
            if (i + replaceLen >= bufSize - 1) {
                break;
            }

            memcpy(&buf[i], replace, replaceLen);
            i += replaceLen;
            replaced = 1;
        } else {
            buf[i++] = *input;
        }
    }

    buf[i] = '\0';
}
