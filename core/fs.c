#include <stdlib.h>
#include <cell/cell_fs.h>

#include "helpers/message.h"
#include "core/memory.h"
#include "core/fs.h"

int StatFile(const char *path, CellFsStat *sb) {
    CellFsErrno err = cellFsStat(path, sb);
    if (err != CELL_FS_SUCCEEDED) {
        return 0;
    }
    
    return 1;
}

int ReadFile(const char *path, char *buf, int bufSize) {
    int fd;

    CellFsErrno err = cellFsOpen(path, CELL_FS_O_RDONLY, &fd, NULL, 0);
    if (err != CELL_FS_SUCCEEDED) {
        return 0;
    }

    err = cellFsRead(fd, buf, bufSize, NULL);
    if (err != CELL_FS_SUCCEEDED) {
        ERROR_DIALOG("Failed to read file");
        cellFsClose(fd);
        return 0;
    }

    cellFsClose(fd);
    return 1;
}

// Technically doesn't even touch fs for performance but I'm putting it here anyways
int ReadLine(const char *input, size_t inputSize, char *buf, size_t bufSize, size_t *offset) {
    if (*offset >= inputSize) {
        return 0;
    }

    setmem(buf, 0, bufSize);

    size_t i = 0;
    while (*offset < inputSize) {
        char c = input[*offset];
        (*offset)++;

        if (c == '\n') {
            break;
        }
        // In case Windows tries to ruin everything
        else if (c == '\r') {
            if (*offset < inputSize && input[*offset] == '\n') {
                (*offset)++;
            }
            break;
        }

        if (i < bufSize -1) {
            buf[i++] = c;
        } 
        else {
            break;
        }
    }

    return 1;
};

void WriteFile(const char *path, void *buf, const uint64_t size) {
    int fd;

    CellFsErrno err = cellFsOpen(path, CELL_FS_O_WRONLY|CELL_FS_O_CREAT|CELL_FS_O_TRUNC, &fd, NULL, 0);
    if (err != CELL_FS_SUCCEEDED) {
        goto fail;
    }

    cellFsWrite(fd, buf, size, NULL);

    fail:
    cellFsClose(fd);
}
