#ifndef UPDATE_H
#define UPDATE_H

#include <sys/memory.h>
#include <sys/synchronization.h>
#include <sys/process.h>

#include "http/http.h"
#include "globals.h"
#include "toml/helper.h"
#include "tools/fs.h"

#define DOWNLOAD_PATH "/dev_hdd0/plugins/patchwork/patchwork.sprx"
#define INSTALL_PATH "/dev_hdd0/plugins/patchwork.sprx"

#define MAJOR_STR STR(PATCHWORK_VERSION_MAJOR)
#define MINOR_STR STR(PATCHWORK_VERSION_MINOR)

#define HEADER_CLIENT_VERSION "X-Patchwork-Client-Version"
#define HEADER_SPRX_HASH "X-Patchwork-Sprx-Hash"

// Header code is ugly, can be cleaned up if we had a heap allocator
// return 0 for failiure or if we are up to date, 1 if we downloaded a new update
// TODO: We need some kind of logging that can send stuff to stdout and a log file
static int DownloadUpdate(void *pool, size_t pool_size, char *server_url) {
    int err = 0;

    if (!pool || pool_size <= 0) {
        return 0; // Invalid pool, immediately fail
    }

    HttpContext ctx = HttpContext_ctor(pool, pool_size);

    HttpTransaction trans = HttpTransaction_ctor(&ctx, server_url, CELL_HTTP_METHOD_GET);

    CellHttpHeader version_header = {
        HEADER_CLIENT_VERSION, 
        MAJOR_STR "." MINOR_STR
    };

    cellHttpRequestAddHeader(trans.trans_id, &version_header);
    
    HttpTransactionSendRequest(&trans);
    if (trans.err < 0) {
        HttpTransaction_dtor(&trans);
        HttpContext_dtor(&ctx);
        return 0; // Request wasnt sent, immediately fail
    }

    CellHttpHeader hash_header;
    size_t required = 0;
    
    // Initial pass to get buffer size, this is dumb but sony said so
    trans.err = cellHttpResponseGetHeader(trans.trans_id, NULL, HEADER_SPRX_HASH, NULL, NULL, &required);
    if (trans.err < 0) {
        err = 0;
    }

    char header_pool[required];
    trans.err = cellHttpResponseGetHeader(
        trans.trans_id, 
        &hash_header, 
        HEADER_SPRX_HASH,
        header_pool, 
        required, 
        &required
    );

    if (trans.err < 0) {
        err = 0;
    }

    if (trans.status_code == 204) {
        err = 0;
    }

    if (trans.status_code == 200) {
        unsigned char hash_buf[CELL_SHA256_DIGEST_SIZE];
        HttpDownloadFile(&ctx, &trans, DOWNLOAD_PATH, hash_buf);

        uint32_t *hash = (uint32_t *)hash_buf;

        // Compare string hash provided by server to raw digest in `hash_buf`
        for (int i = 0; i < 32; i++) {
            uint8_t server_byte =
                StrToInt(hash_header.value + (i * 2), 2, 16);
            if (server_byte != hash_buf[i]) {
                err = 0;
                break;
            }
            if (i == 31) {
                err = 1;
            }
        }
    }

    HttpTransaction_dtor(&trans);
    HttpContext_dtor(&ctx);

    return err;
}

static int InstallUpdate(char *path) {
    return CopyFile(DOWNLOAD_PATH, path);
}

static void ExitDialogCallback(int button, void *userData) {
    if (button == 1) {
        sys_process_exit(0);
    }
}

#endif //UPDATE_H