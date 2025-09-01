#include <ctype.h>

typedef enum {
    TYPE_STRING,
    TYPE_INT,
    TYPE_BOOL
} ValueType;

typedef struct {
    char section[64];
    char key[64];
    ValueType type;
    union {
        char str_val[128];
        int int_val;
        int bool_val;
    } value;
} TomlEntry;

int ParseAsTomlEntry(const char *line, char *section, TomlEntry *entry, int *error);
