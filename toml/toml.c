#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "toml/toml.h"
#include "helpers/util.h"

int ParseAsTomlEntry(const char *line, char *section, TomlEntry *entry, int *error) {
    const char *p = line;

    while (isspace(*p)) p++;
    if (*p == '#' || *p == '\0') return 0; // Skip comments

    // Parse section header
    if (*p == '[') {
        p++;
        char *end = strchr(p, ']');
        if (!end) {
            *error = 1; // No ending bracket
            return 0;
        }
        size_t len = end - p;
        strncpy(section, p, len);
        section[len] = '\0';
        return 0;
    }

    // Parse key-value pair
    char *eq = strchr(p, '=');
    if (!eq) {
        *error = 1; // No key-value separator (equal sign)
        return 0;
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
            *error = 1; // No end quote
            return 0;
        }
        *end = '\0'; // Replace end quote with null terminator
        entry->type = TYPE_STRING;
        strncpy(entry->value.str_val, start, sizeof(entry->value.str_val));
    }
    else if (strcmp(val, "true") == 0 || strcmp(val, "false") == 0) {
        entry->type = TYPE_BOOL;
        entry->value.bool_val = (strcmp(val, "true") == 0);
    }
    else {
        entry->type = TYPE_INT;
        entry->value.int_val = StrToInt(val);
    }

    return 1;
}
