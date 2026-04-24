#include "fs.h"

int ReadFile(const char *path, char *buf, int buf_size) {
    int fp;

    CellFsErrno err = cellFsOpen(path, CELL_FS_O_RDONLY, &fp, NULL, 0);
    if (err != CELL_FS_SUCCEEDED) {
        return 0;
    }

    err = cellFsRead(fp, buf, buf_size, NULL);
    if (err != CELL_FS_SUCCEEDED) {
        ERROR_DIALOG("Failed to read file");
        cellFsClose(fp);
        return 0;
    }

    cellFsClose(fp);
    return 1;
}

void WriteFile(const char *path, const void *buf, const uint64_t size) {
    int fp;

    CellFsErrno err = cellFsOpen(path, CELL_FS_O_WRONLY|CELL_FS_O_CREAT|CELL_FS_O_TRUNC, &fp, NULL, 0);
    if (err != CELL_FS_SUCCEEDED) {
        goto fail;
    }

    cellFsWrite(fp, buf, size, NULL);

    fail:
    cellFsClose(fp);
}