#include <string.h>

#include <sys/prx.h>
#include <sys/ppu_thread.h> // sys_ppu_thread_yield
#include <sys/process.h>
#include <sys/sys_time.h>
#include <sys/timer.h>
#include <cell/hash/libsha256.h>

#include "core/memory.h"
#include "core/fs.h"
#include "toml/toml.h"
#include "helpers/util.h"
#include "helpers/message.h"
#include "offsets.h"

#define STR1(x)  #x
#define STR(x)  STR1(x)

SYS_MODULE_INFO(PatchWorkLBP, 0, PATCHWORK_VERSION_MAJOR, PATCHWORK_VERSION_MINOR);
SYS_MODULE_START(start);

char lobby_password[16] = "";
char url[70] = "";
char digest[LBP_DIGEST_LENGTH] = "";

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define CONFIG_PATH "/dev_hdd0/plugins/patchwork/patchwork_config.toml"

void patch_thread(uint64_t arg) {

    const sys_pid_t processPid = sys_process_getpid();
    uint8_t game = 0;

    int password_randomized = 1;
    unsigned char xxtea_key[32] = "";
    sys_time_sec_t sec = 0;
    sys_time_nsec_t nsec = 0;
    sys_time_get_current_time(&sec, &nsec);
    uint64_t combined_time = nsec + sec;
    cellSha256Digest(&combined_time, sizeof(uint64_t), xxtea_key);

    char ua[20] = "";
    const char *user_agent;

    ReadProcessMemory(processPid, (void*)LBP1_USER_AGENT_OFFSET, ua, 20);
    if (ua[15] == '$') {
        game = 1;
        
        // we need to patch the user agent *before* waiting for sys_fs
        // to prevent a race condition which makes the game still send the unpatched user agent
        WriteProcessMemory(processPid, (void*)LBP1_NETWORK_KEY_OFFSET, xxtea_key, 16);
        user_agent = "PatchworkLBP1 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
        WriteProcessMemory(processPid, (void*)LBP1_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);

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
        goto foundGame;
    }

    ReadProcessMemory(processPid, (void*)LBP3_JP_NAME_OFFSET, ua, 20);
    if (ua[18] == '3') {
        game = 4;
    }

    foundGame:
    ; // Blank statement after label to deal with compiler errors

    char buffer[196] = "";
    ReadFile(CONFIG_PATH, buffer, 196);

    char line[128] = "";
    char section[32] = "";
    int badParse = 0; // Display an error message if this becomes true
    size_t offset = 0;
    while (ReadLine(buffer, 196, line, 128, &offset)) {
        TomlEntry entry;
        if (ParseAsTomlEntry(line, section, &entry, &badParse)) {
            if (strcmp(entry.section, "Patchwork") == 1) {
                continue;
            }
            else if (strcmp(entry.key, "LobbyPassword") == 0) {
                strcpy(lobby_password, entry.value.str_val);
            }
            else if (strcmp(entry.key, "ServerURL") == 0) {
                strcpy(url, entry.value.str_val);
            }
            else if (strcmp(entry.key, "ServerDigest") == 0) {
                strcpy(digest, entry.value.str_val);
            }
        }
    }

    // Since we cant display an error while an error is already being displayed, this one should take precendence
    // Any errors we catch could be combined into a single large error to be displayed at the end, but I cant be arsed
    if (badParse == 1) {
        ERROR_DIALOG(WARNING_CONFIG_PARSE_FAIL);
    }
    if (!url[0]) {
        ERROR_DIALOG(WARNING_CONFIG_MISSING_URL);
    }

    int patched = 1;
    // Hash the lobby password so we get an unrecoverable string of a fixed length
    if (lobby_password[0]) {
        cellSha256Digest(lobby_password, strlen(lobby_password), xxtea_key);
        password_randomized = 0;
    }

    char msgBuf[sizeof(SUCCESS_MESSAGE_WITHOUT_PW)];

    if (password_randomized == 0) {
        strcpy(msgBuf, SUCCESS_MESSAGE_WITH_PW);
    } 
    else {
        strcpy(msgBuf, SUCCESS_MESSAGE_WITHOUT_PW);
    }

    switch (game) {
        case 1:
            if (password_randomized == 0) {
                WriteProcessMemory(processPid, (void *)LBP1_NETWORK_KEY_OFFSET, xxtea_key, 16);
            }
            if (digest[0]) {
                WriteProcessMemory(processPid, (void *)LBP1_DIGEST_OFFSET, digest, LBP_DIGEST_LENGTH);
            }
            if (url[0]) {
                WriteProcessMemory(processPid, (void *)LBP1_HTTP_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP1_HTTPS_URL_OFFSET, url, strlen(url)+1);
            }

            // Enable client-side check for LBP1 playlists
            const char *playlist_check_override = "1";
            WriteProcessMemory(processPid, (void*)LBP1_PLAYLIST_OFFSET, playlist_check_override, 1);

            msgBuf[28] = '1';
            break;
        case 2:
            user_agent = "PatchworkLBP2 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void *)LBP2_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void *)LBP2_NETWORK_KEY_OFFSET, xxtea_key, 16);
            if (digest[0]) {
                WriteProcessMemory(processPid, (void *)LBP2_DIGEST_OFFSET, digest, LBP_DIGEST_LENGTH);
            }
            if (url[0]) {
                WriteProcessMemory(processPid, (void *)LBP2_HTTP_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP2_HTTPS_URL_OFFSET, url, strlen(url)+1);
            }
            msgBuf[28] = '2';
            break;
        case 3:
            user_agent = "PatchworkLBP3 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void *)LBP3_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void *)LBP3_NETWORK_KEY_OFFSET, xxtea_key, 16);
            if (url[0]) {
                WriteProcessMemory(processPid, (void *)LBP3_HTTP_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP3_HTTPS_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP3_LIVE_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP3_PRESENCE_URL_OFFSET, url, strlen(url)+1);
            }
            if (digest[0]) {
                WriteProcessMemory(processPid, (void *)LBP3_DIGEST_OFFSET, digest, LBP_DIGEST_LENGTH);
            }
            msgBuf[28] = '3';
            break;
        case 4:
            user_agent = "PatchworkLBP3 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
            WriteProcessMemory(processPid, (void *)LBP3_JP_USER_AGENT_OFFSET, user_agent, strlen(user_agent)+1);
            WriteProcessMemory(processPid, (void *)LBP3_JP_NETWORK_KEY_OFFSET, xxtea_key, 16);
            if (url[0]) {
                WriteProcessMemory(processPid, (void *)LBP3_JP_HTTP_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP3_JP_HTTPS_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP3_JP_LIVE_URL_OFFSET, url, strlen(url)+1);
                WriteProcessMemory(processPid, (void *)LBP3_JP_PRESENCE_URL_OFFSET, url, strlen(url)+1);
            }
            if (digest[0]) {
                WriteProcessMemory(processPid, (void *)LBP3_JP_DIGEST_OFFSET, digest, LBP_DIGEST_LENGTH);
            }
            msgBuf[47] = '3';
            break;
        default:
            ERROR_DIALOG("Failed to detect game, your online is not safe!");
            patched = 0;
            break;
    }

    WMPopup(msgBuf, 8, 5);

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
