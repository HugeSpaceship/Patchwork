#include "fs.h"
#include <stdint.h>

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

int CopyFile(const char *src, const char *dst) {
    int src_fd;
    int dst_fd;
    CellFsErrno err;

    err = cellFsOpen(src, CELL_FS_O_RDONLY, &src_fd, NULL, 0);
    if (err != CELL_FS_SUCCEEDED) {
        return 0;
    }

    err = cellFsOpen(dst, CELL_FS_O_WRONLY|CELL_FS_O_CREAT|CELL_FS_O_TRUNC, &dst_fd, NULL, 0);
    if (err != CELL_FS_SUCCEEDED) {
        cellFsClose(src_fd);
        return 0;
    }

    char buffer[2048];

    while (1) {
        uint64_t read = 0;
        err = cellFsRead(src_fd, buffer, sizeof(buffer), &read);

        if (err != CELL_FS_SUCCEEDED) {
            cellFsClose(src_fd);
            cellFsClose(dst_fd);
            return -1;
        }

        if (read == 0) {
            break;
        }

        uint64_t offset = 0;

        while (offset < read) {
            uint64_t written = 0;

            err = cellFsWrite(dst_fd, buffer + offset, read - offset, &written);

            if (err != CELL_FS_SUCCEEDED || written == 0) {
                cellFsClose(src_fd);
                cellFsClose(dst_fd);
                return -1;
            }

            offset += written;
        }
    }

    cellFsClose(src_fd);
    cellFsClose(dst_fd);

    return 0;
}