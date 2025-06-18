#include <sys/prx.h>
#include <stdlib.h>
#include <sys/ppu_thread.h> // sys_ppu_thread_yield
#include <string.h>
#include <sys/process.h>
#include <sysutil/sysutil_msgdialog.h>
#include <cell/hash/libsha256.h>

#include <sys/sys_time.h>
#include <sys/timer.h>
#include <cell/cell_fs.h>

#include "offsets.h"
#include "memory.h"
#include "util.h"

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

int ReadFile(const char* path, char* buf, int buf_size) {
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

void setmem(char* buf, int value, int size) {
	for (int i = 0; i < size; i++) {
		buf[i] = value;
	}
}

void WriteFile(const char* path, void* buf, const uint64_t size) {
    int fp;

    CellFsErrno err = cellFsOpen(path, CELL_FS_O_WRONLY|CELL_FS_O_CREAT|CELL_FS_O_TRUNC, &fp, NULL, 0);
    if (err != CELL_FS_SUCCEEDED) {
        goto fail;
    }

    cellFsWrite(fp, buf, size, NULL);

    fail:
    cellFsClose(fp);
}


void patch_thread(uint64_t arg) {

    const sys_pid_t processPid = sys_process_getpid();
    uint8_t game = 0;

    char * ua = __builtin_alloca(20);

    ReadProcessMemory(processPid, (void*)LBP1_USER_AGENT_OFFSET, ua, 20);
    if (ua[15] == '$') {
        game = 1;
        sys_timer_sleep(1); // LBP1 loads the sys_fs library quite late
        goto foundGame;
    }

    ReadProcessMemory(processPid, (void*)LBP2_USER_AGENT_OFFSET, ua, 20);
    if (ua[18] == '2') {
        game = 2;
        goto foundGame;
    }

    ReadProcessMemory(processPid, (void*)LBP3_NAME_OFFSET, ua, 20);
    if (ua[18] == '3') {
        game = 3;
    }

    foundGame:

    lobby_password = __builtin_alloca(16);
	setmem(lobby_password, 0, 16);
    const int read_password = ReadFile(LOBBY_PASSWORD_PATH, lobby_password, 16);
    url = __builtin_alloca(70);
	setmem(url, 0, 70);
    const int read_url = ReadFile(GAME_URL_PATH, url, 70);
    digest = __builtin_alloca(LBP_DIGEST_LENGTH);
	setmem(digest, 0, LBP_DIGEST_LENGTH);
    const int read_digest = ReadFile(DIGEST_PATH, digest, LBP_DIGEST_LENGTH);

    int password_randomized = 0;
    int patched = 1;
    unsigned char * xxtea_key = __builtin_alloca(32);
    // Hash the lobby password so we get an unrecoverable string of a fixed length
    if (read_password) {
		lobby_password = trimEnd(lobby_password);
        cellSha256Digest(lobby_password, strlen(lobby_password), xxtea_key);
    } else {
        sys_time_sec_t sec = 0;
        sys_time_nsec_t nsec = 0;
        sys_time_get_current_time(&sec, &nsec);
        uint64_t combined_time = nsec + sec;
        cellSha256Digest(&combined_time, sizeof(uint64_t), xxtea_key);
        password_randomized = 1;
    }
	
	// Trim strings so users editing text files by hand don't cause issues
	if(read_digest) {
		digest = trimEnd(digest);
	}
	
	if(read_url) {
		url = trimEnd(url);
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
            if (read_digest) {
                WriteProcessMemory(processPid, (void*)LBP1_DIGEST_OFFSET, digest, LBP_DIGEST_LENGTH);
            }
            if (read_url) {
                WriteProcessMemory(processPid, (void*)LBP1_HTTP_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void*)LBP1_HTTPS_URL_OFFSET, url, strlen(url)+1);
            }
            msgBuf[47] = '1';

            break;
        case 2:
            user_agent = "PatchworkLBP2 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void*)LBP2_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void*)LBP2_NETWORK_KEY_OFFSET, xxtea_key, 16);
            if (read_digest) {
                WriteProcessMemory(processPid, (void*)LBP2_DIGEST_OFFSET, digest, LBP_DIGEST_LENGTH);
            }
            if (read_url) {
                WriteProcessMemory(processPid, (void*)LBP2_HTTP_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void*)LBP2_HTTPS_URL_OFFSET, url, strlen(url)+1);
            }
            msgBuf[47] = '2';
            break;
        case 3:
            user_agent = "PatchworkLBP3 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void*)LBP3_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void*)LBP3_NETWORK_KEY_OFFSET, xxtea_key, 16);
            if (read_url) {
                WriteProcessMemory(processPid, (void*)LBP3_HTTP_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void*)LBP3_HTTPS_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void*)LBP3_LIVE_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void*)LBP3_PRESENCE_URL_OFFSET, url, strlen(url)+1);
            }
            if (read_digest) {
                WriteProcessMemory(processPid, (void*)LBP3_DIGEST_OFFSET, digest, LBP_DIGEST_LENGTH);
            }
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

    sys_ppu_thread_exit(0);
}


sys_ppu_thread_t threadID = 0;

int start(void);
int start(void)
{
    sys_ppu_thread_yield(); // Yield to hopefully let IO finish loading important crap

    sys_ppu_thread_create(&threadID, patch_thread, 0, 3071, 4096, SYS_PPU_THREAD_CREATE_JOINABLE, "Patchwork-thread");

    return SYS_PRX_NO_RESIDENT;
}