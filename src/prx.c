#include <sys/prx.h>
#include <sys/sys_time.h>

#include <cell/hash/libsha256.h>
#include <cell/sysmodule.h>

#include "hooks/hooks.h"
#include "hooks/script-block.h"
#include "tools/util.h"
#include "tools/fs.h"
#include "offsets.h"
#include "globals.h"

#include "toml/toml.h"
#include "toml/keymap.h"
#include "toml/tokenizer.h"

SYS_MODULE_INFO(PatchworkLBP, 0, PATCHWORK_VERSION_MAJOR, PATCHWORK_VERSION_MINOR);
SYS_MODULE_START(start);

int start(void);
int start(void)
{
    cellSysmoduleLoadModule(CELL_SYSMODULE_FS);

    char toml_buf[256];
    ReadFile(MAIN_CONFIG_PATH, toml_buf, 256);

    Lexer l = MakeLexer(toml_buf);
    TOMLEntry entries[4];
    TOMLReadBuffer(&l, entries, 4);

    char *server_url = NULL;
    char *join_key = NULL;
    char *digest_key = NULL;
    int enable_join_key = 1;

    TOMLKeyMap key_map[] = {
        {MAIN_CONFIG_SECTION, "server_url", TOML_TYPE_STRING, &server_url},
        {MAIN_CONFIG_SECTION, "join_key", TOML_TYPE_STRING, &join_key},
        {MAIN_CONFIG_SECTION, "digest_key", TOML_TYPE_STRING, &digest_key},
        {MAIN_CONFIG_SECTION, "enable_join_key", TOML_TYPE_BOOL, &enable_join_key},
    };

    TOMLApplyEntriesToKeyMap(entries, 4, key_map, 4);

    unsigned char xxtea_key[32];
    int join_key_randomized = 1;

    if (enable_join_key) {
        if (join_key) {
            join_key = TrimEnd(join_key);
            // Hash the lobby password so we get an unrecoverable string of a fixed length
            cellSha256Digest(join_key, strlen(join_key), xxtea_key);
            join_key_randomized = 0;
        } else {
            // Generate random key
            sys_time_sec_t sec = 0;
            sys_time_nsec_t nsec = 0;
            sys_time_get_current_time(&sec, &nsec);
            uint64_t combined_time = nsec + sec;
            cellSha256Digest(&combined_time, sizeof(uint64_t), xxtea_key);
        }
    }

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

        if (enable_join_key) {
            if (!join_key_randomized) {
                strcpy(msgBuf, SUCCESS_MESSAGE_WITH_PW);
            } else {
                strcpy(msgBuf, SUCCESS_MESSAGE_RANDOM_PW);
            }
        } else {
            strcpy(msgBuf, SUCCESS_MESSAGE_WITHOUT_PW);
        }

        WriteFile("/dev_hdd0/tmp/wm_request", msgBuf, strlen(msgBuf));
    }

    size_t url_len = 0;
    if(server_url) {
        server_url = TrimEnd(server_url);
        url_len = strlen(server_url) + 1;
    }

    // Write to the chosen offets
    if (enable_join_key && network_key_offset)
        memcpy(network_key_offset, xxtea_key, LBP_NETWORK_KEY_SIZE);
    if (user_agent_offset)
        memcpy(user_agent_offset, user_agent, strlen(user_agent) + 1);
    if (https_url_offset && server_url)
        memcpy(https_url_offset, server_url, url_len);
    if (http_url_offset && server_url)
        memcpy(http_url_offset, server_url, url_len);
    if (digest_offset && digest_key) {
        digest_key = TrimEnd(digest_key);
        memcpy(digest_offset, digest_key, LBP_DIGEST_LENGTH);
    }
    if (presence_url_offset && server_url)
        memcpy(presence_url_offset, server_url, url_len);
    if (live_url_offset && server_url)
        memcpy(live_url_offset, server_url, url_len);
    if (notification_enable_offset && notification_enable_instr)
        memcpy(notification_enable_offset, &notification_enable_instr, 4);
    if (rescheck_offset && rescheck_hook) {
        uint32_t rescheck_instr = RelativeBranch(rescheck_hook, rescheck_offset);
        memcpy(rescheck_offset, &rescheck_instr, 4);
    }

    // Exit

    cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);

    return SYS_PRX_NO_RESIDENT;
}
