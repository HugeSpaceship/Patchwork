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

static int DownloadUpdate(void *pool, size_t pool_size, char *server_url);
static int InstallUpdate(char *path);

#endif //UPDATE_H