#include "http.h"
#include <stdint.h>

HttpContext HttpContext_ctor(void *pool, size_t pool_size) {
    HttpContext result;
    result.err = cellHttpInit(pool, pool_size);
    if (result.err < 0) {
        return result;
    }
    result.pool = pool;
    result.pool_size = pool_size;

    result.err = cellHttpCreateClient(&result.client);

    return result;
}

void HttpContext_dtor(HttpContext *ctx) {
    cellHttpEnd();
    cellHttpDestroyClient(ctx->client);
}

HttpTransaction HttpTransaction_ctor(HttpContext *ctx, char *uri_string, const char *method) {
    HttpTransaction result;
    result.err = cellHttpUtilParseUri(&result.uri, uri_string, 
        result.pool, TRANSACTION_POOL_SIZE, NULL // Maybe i dont wanna put a null here
    );

    if (result.err < 0) {
        return result; // Error out
    }

    result.err = cellHttpCreateTransaction(&result.trans_id, ctx->client, method, &result.uri);
    if (result.err < 0) {
    }

    return result;
}

void HttpTransaction_dtor(HttpTransaction *trans) {
    cellHttpDestroyTransaction(trans->trans_id); // Better naming perchance
}

// Function may not need to be in here but its fine for now
int HttpDownloadFile(HttpContext *ctx, HttpTransaction *trans, const char *path) {
    int fd;
    CellFsErrno err = cellFsOpen(path, 
        CELL_FS_O_WRONLY | CELL_FS_O_CREAT | CELL_FS_O_TRUNC, 
        &fd, NULL, 0
    );

    if (err != CELL_FS_SUCCEEDED) {
        return 0;
    }

    trans->err = cellHttpSendRequest(trans->trans_id, NULL, 0, NULL);
    if (trans->err < 0) {
        return 0;
    }

    trans->err = cellHttpResponseGetContentLength(trans->trans_id, &trans->total);
    if (trans-> err < 0) {
        return 0;
    }
    
    char buffer[256];
    while (1) {
        trans->err = cellHttpRecvResponse(trans->trans_id, 
            buffer, sizeof(buffer), &trans->recv
        );

        if (trans->err < 0) {
            return 0;
        }

        if (trans->recv == 0) {
            break; // Done
        }

        uint64_t offset = 0;
        while (offset < trans->recv) {
            uint64_t written = 0;
            cellFsWrite(fd, 
                buffer + offset, 
                trans->recv - offset, 
                &written
            );

            if (written == 0) {
                return 0; // Fail
            }

            offset += written;
        }
    }

    cellFsClose(fd);

    return 1;
}