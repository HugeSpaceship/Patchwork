#ifndef INI_H
#define INI_H

#define INI_VERSION "0.1.1"

typedef struct ini_t ini_t;

ini_t*      ini_load(const char *filename);
void        ini_free(ini_t *ini);
const char* ini_get(ini_t *ini, const char *section, const char *key);
int         ini_sget(ini_t *ini, const char *section, const char *key, const char *scanfmt, void *dst);

#endif