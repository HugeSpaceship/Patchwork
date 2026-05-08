#ifndef UPDATE_H
#define UPDATE_H

#include <sys/memory.h>

#include "http/http.h"
#include "globals.h"

#define INSTALL_PATH "/dev_hdd0/plugins/patchwork.sprx"

static int DownloadUpdate(void *pool, size_t pool_size, char *server_url) {
    if (!pool) {
        return 0; // Pool is null
    }

    HttpContext ctx = HttpContext_ctor(pool, pool_size);

    HttpTransaction trans = HttpTransaction_ctor(&ctx, server_url, CELL_HTTP_METHOD_GET);

    HttpDownloadFile(&ctx, &trans, INSTALL_PATH);

    HttpTransaction_dtor(&trans);
    HttpContext_dtor(&ctx);

    return 1;
}

#endif //UPDATE_H