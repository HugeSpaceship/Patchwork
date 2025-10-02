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
        char strVal[128];
        int intVal;
        int boolVal;
    } value;
} TomlEntry;

typedef struct {
    const char *key;
    void *target;
    ValueType type;
    size_t maxLen; // Only needed for strings
} KeyMap;

enum TomlParseErrno {
    TOML_FAIL, // Wonderful error handling
    TOML_SKIPPED,
    TOML_SUCCESS
};

int ParseAsTomlEntry(const char *line, char *section, TomlEntry *entry);
void ApplyEntryToKeyMap(const KeyMap *map, const TomlEntry *entry, size_t mapSize);
