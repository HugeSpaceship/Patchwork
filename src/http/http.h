#ifndef HTTP_H
#define HTTP_H

#include <ctype.h>

#include <cell/http.h>
#include <cell/cell_fs.h>

#define TRANSACTION_POOL_SIZE 32

typedef struct {
    CellHttpClientId client;
    void *pool;
    size_t pool_size;
    int err;
} HttpContext;

typedef struct {
    CellHttpUri uri;
    CellHttpTransId trans_id;
    char pool[TRANSACTION_POOL_SIZE];
    size_t recv;
    uint64_t total;
    int status_code;
    int err;
} HttpTransaction;

HttpContext HttpContext_ctor(void *pool, size_t pool_size);
void HttpContext_dtor(HttpContext *ctx);

HttpTransaction HttpTransaction_ctor(HttpContext *ctx, char *uri_string, const char *method);
void HttpTransaction_dtor(HttpTransaction *trans);

int HttpDownloadFile(HttpContext *ctx, HttpTransaction *trans, const char *path);

#endif //HTTP_H