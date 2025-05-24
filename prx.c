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

#include "offsets.h"
#include "memory.h"

#define STR1(x)  #x
#define STR(x)  STR1(x)

#define PATCHWORK_VERSION_MAJOR 1
#define PATCHWORK_VERSION_MINOR 0

SYS_MODULE_INFO(PatchWorkLBP, 0, PATCHWORK_VERSION_MAJOR, PATCHWORK_VERSION_MINOR);
SYS_MODULE_START(start);

char* url = NULL;
char* digest = NULL;
char* lobby_password = NULL;


#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define ERROR_DIALOG(text) cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_ERROR | CELL_MSGDIALOG_TYPE_SE_MUTE_OFF | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK, text, NULL, NULL, NULL);

#define LOBBY_PASSWORD_PATH "/dev_hdd0/tmp/patchwork_lobby_password.txt"
#define GAME_URL_PATH "/dev_hdd0/tmp/patchwork_url.txt"
#define DIGEST_PATH "/dev_hdd0/tmp/patchwork_digest.txt"

#define SUCCESS_MESSAGE_WITH_PW "/popup.ps3?Patchwork%20"STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBP2%0ALobby%20password%20has%20been%20set&icon=8&snd=5"
#define SUCCESS_MESSAGE_WITHOUT_PW "/popup.ps3?Patchwork%20"STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBP2%0ALobby%20password%20has%20been%20randomized&icon=8&snd=5"

char* ReadFile(const char* path) {
    int fp;

    CellFsErrno err = cellFsOpen(path, CELL_FS_O_RDONLY, &fp, 0, 0);
    if (err != CELL_FS_SUCCEEDED) {
        goto fail;
    }

    CellFsStat stat;
    err = cellFsFstat(fp, &stat);
    if (err != CELL_FS_SUCCEEDED) {
        ERROR_DIALOG("Failed to stat file ");
        goto fail;
    }

    if (stat.st_size == 0) {
        ERROR_DIALOG("File is empty");
        goto fail;
    }

    char* buf = __builtin_alloca(stat.st_size);

    err = cellFsRead(fp, buf, stat.st_size, NULL);
    if (err != CELL_FS_SUCCEEDED) {
        ERROR_DIALOG("Failed to read file");
        goto fail;
    }
    goto done;

    fail:
        cellFsClose(fp);
        return NULL;
    done:
        cellFsClose(fp);
        return buf;
}

void WriteFile(const char* path, void* buf, const uint64_t size) {
    int fp;

    CellFsErrno err = cellFsOpen(path, CELL_FS_O_WRONLY|CELL_FS_O_CREAT|CELL_FS_O_TRUNC, &fp, 0, 0);
    if (err != CELL_FS_SUCCEEDED) {
        goto fail;
    }

    cellFsWrite(fp, buf, size, NULL);

    fail:
    cellFsClose(fp);
}



int start(void);
int start(void)
{
    sys_ppu_thread_yield(); // Yield to hopefully let IO finish loading important crap

    const sys_pid_t processPid = sys_process_getpid();
    uint8_t game = 0;


    char * ua = __builtin_alloca(20);

    ReadProcessMemory(processPid, (void*)LBP1_USER_AGENT_OFFSET, ua, 20);
    if (ua[16] == '$') {
        game = 1;
    }

    ReadProcessMemory(processPid, (void*)LBP2_USER_AGENT_OFFSET, ua, 20);
    if (ua[18] == '2') {
        game = 2;
    }

    ReadProcessMemory(processPid, (void*)LBP3_NAME_OFFSET, ua, 20);
    if (ua[19] == '3') {
        game = 3;
    }

    lobby_password = ReadFile(LOBBY_PASSWORD_PATH);
    url = ReadFile(GAME_URL_PATH);
    digest = ReadFile(DIGEST_PATH);

    int password_randomized = 0;
    int patched = 1;
    unsigned char * xxtea_key = __builtin_alloca(32);
    // Hash the lobby password so we get an unrecoverable string of a fixed length
    if (lobby_password) {
        cellSha256Digest(lobby_password, strlen(lobby_password), xxtea_key);
    } else {
        CellRtcTick tick;
        cellRtcGetCurrentTick(&tick);
        cellSha256Digest(&tick.tick, sizeof(uint64_t), xxtea_key);
        password_randomized = 1;
    }

    char* msgBuf = __builtin_alloca(sizeof(SUCCESS_MESSAGE_WITHOUT_PW));

    if (password_randomized == 0) {
        strcpy(msgBuf, SUCCESS_MESSAGE_WITH_PW);
    } else {
        strcpy(msgBuf, SUCCESS_MESSAGE_WITHOUT_PW);
    }

    const char* user_agent;
    switch (game) {
        case 1:
            user_agent = "PatchworkLBP1 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void*)LBP1_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void*)LBP1_NETWORK_KEY_OFFSET, xxtea_key, 16);
            msgBuf[47] = '1';

            break;
        case 2:
            user_agent = "PatchworkLBP2 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void*)LBP2_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void*)LBP2_NETWORK_KEY_OFFSET, xxtea_key, 16);
            msgBuf[47] = '2';
            break;
        case 3:
            user_agent = "PatchworkLBP3 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void*)LBP3_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void*)LBP3_NETWORK_KEY_OFFSET, xxtea_key, 16);
            msgBuf[47] = '3';
            break;
        default:
            ERROR_DIALOG("Failed to detect game, your online is not safe!");
            patched = 0;
            break;
    }

    if (patched == 1) {
        WriteFile("/dev_hdd0/tmp/wm_request", msgBuf, strlen(msgBuf));
    }

    return SYS_PRX_NO_RESIDENT;
}