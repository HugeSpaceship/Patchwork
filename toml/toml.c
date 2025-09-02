#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "toml/toml.h"
#include "helpers/util.h"

// Parse a single line of TOML as a TomlEntry struct
int ParseAsTomlEntry(const char *line, char *section, TomlEntry *entry) {
    const char *p = line;

    while (isspace(*p)) p++;
    if (*p == '#' || *p == '\0') {
        return TOML_SKIPPED; // Skip comments
    }

    // Parse section header
    if (*p == '[') {
        p++;
        char *end = strchr(p, ']');
        if (!end) {
            return TOML_FAIL;
        }
        size_t len = end - p;
        strncpy(section, p, len);
        section[len] = '\0';
        return TOML_SUCCESS;
    }

    // Parse key-value pair
    char *eq = strchr(p, '=');
    if (!eq) {
        return TOML_FAIL;
    }
    
    size_t key_len = eq - p;
    size_t val_len = strlen(eq + 1);

    // alloca to use Trim() properly
    char *key = __builtin_alloca(key_len + 1);
    char *val = __builtin_alloca(val_len + 1);

    memcpy(key, p, key_len);
    key[key_len] = '\0';
    memcpy(val, eq + 1, val_len + 1);

    key = Trim(key);
    val = Trim(val);

    // Section should be tracked externally and passed in
    strncpy(entry->section, section, sizeof(entry->section));
    strncpy(entry->key, key, sizeof(entry->key));

    // Parse types
    if (val[0] == '"') {
        char *start = val + 1; // Skip quote
        char *end = strchr(start, '"'); // Find end quote
        if (!end) {
            return TOML_FAIL;
        }
        *end = '\0'; // Replace end quote with null terminator
        entry->type = TYPE_STRING;
        strncpy(entry->value.strVal, start, sizeof(entry->value.strVal));
    }
    else if (strcmp(val, "true") == 0 || strcmp(val, "false") == 0) {
        entry->type = TYPE_BOOL;
        entry->value.boolVal = (strcmp(val, "true") == 0);
    }
    else {
        entry->type = TYPE_INT;
        entry->value.intVal = StrToInt(val);
    }

    return TOML_SUCCESS;
}

// Parse all lines of TOML in buffer and try to populate values in a KeyMap
void ApplyEntryToKeyMap(const KeyMap *map, const TomlEntry *entry, size_t mapSize) {
    for (size_t i = 0; i < mapSize; i++) {
        if (strcmp(entry->key, map[i].key) != 0) {
            continue;
        }

        switch (map[i].type) {
            case TYPE_STRING:
                strncpy((char *)map[i].target, entry->value.strVal, map[i].maxLen);
                break; // Could be terminated here, but ParseAsTomlEntry terminates all strings

            case TYPE_BOOL:
                *(int *)map[i].target = entry->value.boolVal;
                break;

            case TYPE_INT:
                *(int *)map[i].target = entry->value.intVal;
                break;
        }

        break;
    }
}
