#ifndef FS_H
#define FS_H

#include <cell/cell_fs.h>
#include "util.h"

int ReadFile(const char *path, char *buf, int buf_size);
void WriteFile(const char *path, const void *buf, const uint64_t size);

#endif //FS_H