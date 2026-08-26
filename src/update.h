#ifndef UPDATE_H
#define UPDATE_H

#include <sys/memory.h>
#include <sys/synchronization.h>
#include <sys/process.h>
#include <sys/prx.h>

#include <netex/net.h>

#include "http/http.h"
#include "tools/fs.h"
#include "globals.h"
#include "toml/helper.h"

#define DOWNLOAD_PATH "/dev_hdd0/plugins/patchwork/patchwork.sprx"
#define INSTALL_PATH "/dev_hdd0/plugins/patchwork.sprx"

#define MAJOR_STR STR(PATCHWORK_VERSION_MAJOR)
#define MINOR_STR STR(PATCHWORK_VERSION_MINOR)

#define HEADER_CLIENT_VERSION "X-Patchwork-Client-Version"
#define HEADER_SPRX_HASH "X-Patchwork-Sprx-Hash"

// For future modifications to launch args, existing ones should remain in the current order
typedef struct PatchworkLaunchArgs {
    sys_prx_id_t old_process;
    int updated;
} PatchworkLaunchArgs;

int DownloadUpdate(void *pool, size_t pool_size, char *server_url);
int InstallUpdate(char *path);

int TryUpdateAndInstall(char *url);
int TryRestartModule();

#endif //UPDATE_H