//
// Created by henry on 5/22/2025.
//

#include "config.h"
#include <stdbool.h>
#include <string.h>



const char* getValue(const char* key, const char* cfg) {
    int len = strlen(cfg);
    int keyLen = strlen(key);
    int valLen = 0;

    int lineOffset = 0;
    bool match = false;
    for (int i = 0; i < len; i++) {
        if (lineOffset < keyLen) {
            if (cfg[i] == ' ' || cfg[i] == '\t') {
                continue;
            }
            if (key[lineOffset] == cfg[i]) {
                lineOffset++;
                match = true;
                continue;
            }
            match = false;
        }
        if (cfg[i] == '=' && match) {
            for (int c = i; c < len; c++) { // Keep going until we find the end of the line
                if (cfg[c] == '\n') {
                    break;
                }
                valLen++;
            }
            const char* val = __builtin_alloca(valLen);
            memcpy((void*)val, (void*)i+1, valLen);
            return val;
        }
        if (cfg[i] == '\n') {
            lineOffset = 0;
        }
    }
    return NULL;
}
