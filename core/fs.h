#include <cell/cell_fs.h>

int StatFile(const char *path, CellFsStat *sb);
int ReadFile(const char *path, char *buf, int buf_size);
int ReadLine(const char *input, size_t inputSize, char *buf, size_t bufSize, size_t *offset);
void WriteFile(const char *path, void *buf, const size_t size);