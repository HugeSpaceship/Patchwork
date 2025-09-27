#include <string.h>

#include "helpers/message.h"
#include "helpers/util.h"
#include "core/fs.h"
#include "core/memory.h"

// This function is a bit too specific to use ReplaceNextInstanceOfChar() very easily
void WMPopup(char *message, int icon, int snd) {
    size_t oldLen = strlen(message);
    size_t newLen = oldLen;

    // Extra loop over string but safely saves memory over always estimating worse case (all spaces | oldLen  *3)
    for (size_t i = 0; i < oldLen; i++) {
        if (message[i] == '\n') {
            newLen += 2;
        }
    }
    newLen++;

    char fullPath[strlen(WM_POPUP_REQUEST) + newLen + WM_POPUP_CONFIG_MAX_LEN];

    memset(fullPath, 0, sizeof(fullPath));

    strcpy(fullPath, WM_POPUP_REQUEST); // Copy popup command

    // Replace all newlines with '0A' char for popup request parser
    char *p = fullPath + strlen(fullPath);
    for (size_t i = 0; i < oldLen; i++) {
        if (message[i] == '\n') {
            memcpy(p, "%0A", 3);
            p += 3;
        }
        else if (message[i] == '\r') { // Not sure why this would happen willingly
            memcpy(p, "%0A", 3);
            p += 3;
            if (message[i + 1] == '\n') {
                p++;
                i++;
            }
        }
        else {
            *p = message[i];
            p++;
        }
    }

    // Append icon and sound bits without snprintf()
    char iconStr[2];
    IntToStr(iconStr, icon, 1);
    char sndStr[2];
    IntToStr(sndStr, snd, 1);

    memcpy(p, WM_POPUP_CONFIG_ICON, strlen(WM_POPUP_CONFIG_ICON));
    p += strlen(WM_POPUP_CONFIG_ICON);
    memcpy(p, iconStr, 1);
    p++;
    memcpy(p, WM_POPUP_CONFIG_SND, strlen(WM_POPUP_CONFIG_SND));
    p += strlen(WM_POPUP_CONFIG_SND);
    memcpy(p, sndStr, 1);
    p++;

    *p = '\0'; // Write null terminator just in case

    WriteFile(WM_REQUEST_PATH, fullPath, strlen(fullPath));
}
