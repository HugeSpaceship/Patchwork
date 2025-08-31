#include <string.h>

#include <sys/tty.h>

#include "message.h"
#include "sys/fs.h"
#include "sys/memory.h"
#include "printf/printf.h"

// Could be abstracted into it's own "Replace all instances of x character in string with..." function, but eh
void WMPopup(char *message, int icon, int snd) {
    size_t oldLen = strlen(message);
    size_t newLen = oldLen;

    // Extra loop over string but safely saves memory over always estimating worse case (all spaces | oldLen  *3)
    for (size_t i = 0; i < oldLen; i++) {
        if (message[i] == ' ' || message[i] == '\n') {
            newLen += 2;
        }
    }
    newLen++;

    char *newMessage = (char*)__builtin_alloca(newLen);
    setmem(newMessage, 0, newLen);

    char *p = newMessage;
    for (size_t i = 0; i < oldLen; i++) {
        if (message[i] == ' ') {
            memcpy(p, "\%20", 3);
            p += 3;
        } 
        else if (message[i] == '\n') {
            memcpy(p, "\%0A", 3);
            p += 3;
        }
        else {
            *p = message[i];
            p++;
        }
    }
    *p = '\0';

    char fullPath[strlen(WM_POPUP_REQUEST) + newLen + WM_POPUP_CONFIG_MAX_LEN];
    snprintf(fullPath, sizeof(fullPath), "%s%s&icon=%d&snd=%d", WM_POPUP_REQUEST, newMessage, icon, snd);

    WriteFile(WM_REQUEST_PATH, fullPath, strlen(fullPath));
}

void PrintTTY(const char *msg) {
    unsigned int written;
    sys_tty_write(0, msg, strlen(msg), &written);
}