#include <sys/prx.h>
#include <stdlib.h>
#include <sys/ppu_thread.h> // sys_ppu_thread_yield
#include <string.h>
#include <sys/process.h>
#include <sysutil/sysutil_msgdialog.h>
#include <cell/hash/libsha256.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <cell/rtc/error.h> // must be included here because rtcsvc doesn't include it (for some reason)
#include <cell/rtc/rtcsvc.h>

#include <cell/fs/cell_fs_file_api.h>
#include <sys/fs_external.h>

#include "config.h"
#include "offsets.h"
#include "memory.h"

SYS_MODULE_INFO(PatchWorkLBP, 0, 1, 0);
SYS_MODULE_START(start);

const char* url = NULL;
const char* digest = NULL;
const char* user_agent = "PatchworkLBP2 1.0\0";
const char* lobby_password = NULL;


#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define ERROR_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_ERROR | CELL_MSGDIALOG_TYPE_SE_MUTE_OFF | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL);

#define CONFIG_PATH "/dev_hdd0/tmp/patchwork.conf"

int start(void);
int start(void)
{
    sys_ppu_thread_yield(); // Yield to hopefully let IO finish loading important crap

    int fp;

    CellFsErrno err = cellFsOpen(CONFIG_PATH, CELL_FS_O_RDONLY, &fp, 0, 0);
    if (err != CELL_FS_SUCCEEDED) {
        ERROR_DIALOG("Failed to load " CONFIG_PATH);
    }

    CellFsStat stat;
    err = cellFsFstat(fp, &stat);
    if (err != CELL_FS_SUCCEEDED) {
        ERROR_DIALOG("Failed to stat " CONFIG_PATH);
    }

    char* buf = __builtin_alloca(stat.st_size);

    uint64_t n = 0;

    err = cellFsRead(fp, buf, stat.st_size, &n);
    if (err != CELL_FS_SUCCEEDED) {
        ERROR_DIALOG("Failed to read " CONFIG_PATH);
    }

    ERROR_DIALOG(buf);

    url = getValue("url", buf);
    lobby_password = getValue("lobby_password", buf);

    // ERROR_DIALOG("Test Error");


    // ini_t* cfg = ini_load(CONFIG_PATH);
    // if (!cfg) {
    //
    // } else {
    //     url = ini_get(cfg, "server", "url");
    //     if (!url) {
    //         ERROR_DIALOG("Invalid or missing server url in " CONFIG_PATH);
    //     }
    //     // url = malloc(MIN(strlen(tmp_url), LBP_PS3_URL_LENGTH) + 1);
    //     // memcpy((void *)url, tmp_url, strlen(tmp_url));
    //     // free((void *)tmp_url);
    //     // memset((void*)(url+strlen(tmp_url)), 0, 1);
    //
    //
    //     // const char* tmp_digest = ini_get(cfg, "server", "digest");
    //     // if (tmp_digest) {
    //     //     digest = malloc(MIN(strlen(tmp_digest), LBP_DIGEST_LENGTH) + 1);
    //     //     memcpy((void *)url, tmp_digest, strlen(tmp_digest));
    //     //     free((void *)tmp_digest);
    //     //     memset((void*)(url+strlen(tmp_digest)), 0, 1); // Set the last byte to null, just in case
    //     // }
    // }



    const sys_pid_t processPid = sys_process_getpid();

    // Write game server URL
    if (url) {
        WriteProcessMemory(processPid, (void*)LBP2_HTTP_URL_OFFSET, url, strlen(url)+1);
        WriteProcessMemory(processPid, (void*)LBP2_HTTPS_URL_OFFSET, url, strlen(url)+1);
    }

    // Write user-agent
    WriteProcessMemory(processPid, (void*)LBP2_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);

    if (lobby_password) {
        unsigned char * xxtea_key = __builtin_alloca(32);
        cellSha256Digest(lobby_password, strlen(lobby_password), xxtea_key);
        WriteProcessMemory(processPid, (void*)LBP2_NETWORK_KEY_OFFSET, xxtea_key, 16);
    } else {
        unsigned char * xxtea_key = __builtin_alloca(32);
        CellRtcTick tick;
        cellRtcGetCurrentTick(&tick);
        cellSha256Digest(&tick.tick, sizeof(uint64_t), xxtea_key);
        WriteProcessMemory(processPid, (void*)LBP2_NETWORK_KEY_OFFSET, xxtea_key, 16);
    }


    // Write digest if applicable
    if (digest) {
        WriteProcessMemory(processPid, (void*)LBP2_DIGEST_OFFSET, digest, strlen(digest)+1);
    }


    return SYS_PRX_NO_RESIDENT;
}