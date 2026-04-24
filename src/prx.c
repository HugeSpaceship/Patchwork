#include <sys/prx.h>
#include <sys/process.h>
#include <sys/sys_time.h>

#include <cell/hash/libsha256.h>
#include <cell/sysmodule.h>

#include "hooks/hooks.h"
#include "tools/memory.h"
#include "tools/util.h"
#include "tools/fs.h"
#include "offsets.h"
#include "hooks/script-block.h"

#define STR1(x)  #x
#define STR(x)  STR1(x)

SYS_MODULE_INFO(PatchworkLBP, 0, PATCHWORK_VERSION_MAJOR, PATCHWORK_VERSION_MINOR);
SYS_MODULE_START(start);

#define LOBBY_PASSWORD_PATH "/dev_hdd0/plugins/patchwork/patchwork_lobby_password.txt"
#define GAME_URL_PATH "/dev_hdd0/plugins/patchwork/patchwork_url.txt"
#define DIGEST_PATH "/dev_hdd0/plugins/patchwork/patchwork_digest.txt"

#define SUCCESS_MESSAGE_WITH_PW "/popup.ps3?Patchwork%20"STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBP2%0ALobby%20password%20has%20been%20set&icon=8&snd=5"
#define SUCCESS_MESSAGE_WITHOUT_PW "/popup.ps3?Patchwork%20"STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBP2%0ALobby%20password%20has%20been%20randomized&icon=8&snd=5"

// Not reallyyy used right now, but could be useful later
enum GameNumber {
    GAME_LBP1 = 1,
    GAME_LBP2 = 2,
    GAME_LBP3 = 3,
    GAME_LBP3_JP = 4,
};

int start(void);
int start(void)
{
    const sys_pid_t process_pid = sys_process_getpid();

    cellSysmoduleLoadModule(CELL_SYSMODULE_FS);

    int password_randomized = 1;

    unsigned char xxtea_key[32];
    char *lobby_password = __builtin_alloca(16);
	memset(lobby_password, 0, 16);
    const int read_password = ReadFile(LOBBY_PASSWORD_PATH, lobby_password, 16);

    if (read_password) {
		lobby_password = TrimEnd(lobby_password);
        // Hash the lobby password so we get an unrecoverable string of a fixed length
        cellSha256Digest(lobby_password, strlen(lobby_password), xxtea_key);
        password_randomized = 0;
    } else {
        // Generate random key
        sys_time_sec_t sec = 0;
        sys_time_nsec_t nsec = 0;
        sys_time_get_current_time(&sec, &nsec);
        uint64_t combined_time = nsec + sec;
        cellSha256Digest(&combined_time, sizeof(uint64_t), xxtea_key);
    }
    
    char *url = __builtin_alloca(70);
	memset(url, 0, 70);
    const int read_url = ReadFile(GAME_URL_PATH, url, 70);

    if(read_url) url = TrimEnd(url);

    char *digest = __builtin_alloca(LBP_DIGEST_LENGTH);
	memset(digest, 0, LBP_DIGEST_LENGTH);
    const int read_digest = ReadFile(DIGEST_PATH, digest, LBP_DIGEST_LENGTH);
	
	if(read_digest) digest = TrimEnd(digest);

    char *user_agent;

    // Init patch generics
    void *network_key_offset = NULL;
    void *user_agent_offset = NULL;
    void *https_url_offset = NULL;
    void *http_url_offset = NULL;
    void *digest_offset = NULL;
    void *presence_url_offset = NULL;
    void *live_url_offset = NULL;
    void *notification_enable_offset = NULL;
    uint32_t notification_enable_instr = 0;
    void *rescheck_offset = NULL;
    void *rescheck_hook = NULL;

    uint8_t game = 0;

    if (!game && ((char *)LBP1_USER_AGENT_OFFSET)[15] == '$') {
        game = GAME_LBP1;
        user_agent = "PatchworkLBP1 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
        network_key_offset = (void *)LBP1_NETWORK_KEY_OFFSET;
        user_agent_offset = (void *)LBP1_USER_AGENT_OFFSET;
        https_url_offset = (void *)LBP1_HTTPS_URL_OFFSET;
        http_url_offset = (void *)LBP1_HTTP_URL_OFFSET;
        digest_offset = (void *)LBP1_DIGEST_OFFSET;
        rescheck_offset = (void *)LBP1_RESOURCE_CHECK_OFFSET;
        rescheck_hook = LBP1ScriptHook;
    }

    if (!game && ((char *)LBP2_USER_AGENT_OFFSET)[18] == '2') {
        game = GAME_LBP2;
        user_agent = "PatchworkLBP2 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
        network_key_offset = (void *)LBP2_NETWORK_KEY_OFFSET;
        user_agent_offset = (void *)LBP2_USER_AGENT_OFFSET;
        https_url_offset = (void *)LBP2_HTTPS_URL_OFFSET;
        http_url_offset = (void *)LBP2_HTTP_URL_OFFSET;
        digest_offset = (void *)LBP2_DIGEST_OFFSET;
        notification_enable_offset = (void *)LBP2_NOTIFICATION_ENABLE_OFFSET;
        notification_enable_instr = 0x38000000; // li r0, 0
        rescheck_offset = (void *)LBP2_RESOURCE_CHECK_OFFSET;
        rescheck_hook = LBP2ScriptHook;
    }

    if (!game && ((char *)LBP3_USER_AGENT_OFFSET)[18]) {
        game = GAME_LBP3;
        user_agent = "PatchworkLBP3 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
        network_key_offset = (void *)LBP3_NETWORK_KEY_OFFSET;
        user_agent_offset = (void *)LBP3_USER_AGENT_OFFSET;
        https_url_offset = (void *)LBP3_HTTPS_URL_OFFSET;
        http_url_offset = (void *)LBP3_HTTP_URL_OFFSET;
        digest_offset = (void *)LBP3_DIGEST_OFFSET;
        presence_url_offset = (void *)LBP3_PRESENCE_URL_OFFSET;
        live_url_offset = (void *)LBP3_LIVE_URL_OFFSET;
        notification_enable_offset = (void *)LBP3_NOTIFICATION_ENABLE_OFFSET;
        notification_enable_instr = 0x38600000; // li r3, 0
        rescheck_offset = (void *)LBP3_RESOURCE_CHECK_OFFSET;
        rescheck_hook = LBP3ScriptHook;
    }

    if (!game && ((char *)LBP3_JP_USER_AGENT_OFFSET)[18]) {
        game = GAME_LBP3_JP;
        user_agent = "PatchworkLBP3 "STR(PATCHWORK_VERSION_MAJOR)"."STR(PATCHWORK_VERSION_MINOR);
        network_key_offset = (void *)LBP3_JP_NETWORK_KEY_OFFSET;
        user_agent_offset = (void *)LBP3_JP_USER_AGENT_OFFSET;
        https_url_offset = (void *)LBP3_JP_HTTPS_URL_OFFSET;
        http_url_offset = (void *)LBP3_JP_HTTP_URL_OFFSET;
        digest_offset = (void *)LBP3_JP_DIGEST_OFFSET;
        presence_url_offset = (void *)LBP3_JP_PRESENCE_URL_OFFSET;
        live_url_offset = (void *)LBP3_JP_LIVE_URL_OFFSET;
        // notification_enable_offset = (void *)LBP3_JP_NOTIFICATION_ENABLE_OFFSET; // TODO: Find this
        // notification_enable_instr = 0x38600000;
        rescheck_offset = (void *)LBP3_JP_RESOURCE_CHECK_OFFSET;
        rescheck_hook = LBP3JPScriptHook;
    }

    if (!game) {
        ERROR_DIALOG("Failed to detect game, your online is not safe!");
    } else {
        char *msgBuf = __builtin_alloca(sizeof(SUCCESS_MESSAGE_WITHOUT_PW));

        if (password_randomized == 0) {
            strcpy(msgBuf, SUCCESS_MESSAGE_WITH_PW);
        } else {
            strcpy(msgBuf, SUCCESS_MESSAGE_WITHOUT_PW);
        }

        WriteFile("/dev_hdd0/tmp/wm_request", msgBuf, strlen(msgBuf));
    }

    size_t url_len = strlen(url) + 1;

    // Write to the chosen offets
    if (network_key_offset)
        WriteProcessMemory(process_pid, network_key_offset, xxtea_key, LBP_NETWORK_KEY_SIZE);
    if (user_agent_offset)
        WriteProcessMemory(process_pid, user_agent_offset, user_agent, strlen(user_agent) + 1);
    if (https_url_offset && url[0])
        WriteProcessMemory(process_pid, https_url_offset, url, url_len);
    if (http_url_offset && url[0])
        WriteProcessMemory(process_pid, http_url_offset, url, url_len);
    if (digest_offset && digest[0])
        WriteProcessMemory(process_pid, digest_offset, digest, LBP_DIGEST_LENGTH);
    if (presence_url_offset && url[0])
        WriteProcessMemory(process_pid, presence_url_offset, url, url_len);
    if (live_url_offset && url[0])
        WriteProcessMemory(process_pid, live_url_offset, url, url_len);
    if (notification_enable_offset && notification_enable_instr)
        *(uint32_t *)notification_enable_offset = notification_enable_instr;
    if (rescheck_offset && rescheck_hook) {
        uint32_t rescheck_instr = RelativeBranch(rescheck_hook, rescheck_offset);
        WriteProcessMemory(process_pid, rescheck_offset, &rescheck_instr, 4);
    }

    // Exit

    cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);

    return SYS_PRX_NO_RESIDENT;
}
