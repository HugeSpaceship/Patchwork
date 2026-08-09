#include <stdint.h>
#include <string.h>
#include <sys/prx.h>
#include <sys/process.h>
#include <sys/sys_time.h>
#include <sys/memory.h>

#include <cell/hash/libsha256.h>
#include <cell/sysmodule.h>
#include <cell/http.h>
#include <netex/net.h>

#include "hooks/hooks.h"
#include "hooks/script-block.h"
#include "toml/helper.h"
#include "hooks/forcejoin-patch.h"
#include "tools/util.h"
#include "tools/fs.h"
#include "offsets.h"
#include "update.h"
#include "globals.h"

#include "toml/toml.h"
#include "toml/keymap.h"
#include "toml/tokenizer.h"

SYS_MODULE_INFO(PatchworkLBP, 0, PATCHWORK_VERSION_MAJOR, PATCHWORK_VERSION_MINOR);
SYS_MODULE_START(start);

#define USER_AGENT "PatchworkLBPX " STR(PATCHWORK_VERSION_MAJOR) "." STR(PATCHWORK_VERSION_MINOR)

int start(void);
int start(void)
{
    cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
    cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
    cellSysmoduleLoadModule(CELL_SYSMODULE_HTTP);

    char toml_buf[312];
    ReadFile(MAIN_CONFIG_PATH, toml_buf, 312);

    Lexer l = MakeLexer(toml_buf);
    TOMLEntry entries[CONFIG_ENTRY_COUNT];
    TOMLReadBuffer(&l, entries, CONFIG_ENTRY_COUNT);

    char *server_url = NULL;
    char *join_key = NULL;
    char *digest_key = NULL;
    int enable_join_key = 1;

    char *update_server_url = NULL;
    int enable_updates = 1;

    TOMLKeyMap key_map[] = {
        {CONFIG_SECTION_MAIN, "server_url", TOML_TYPE_STRING, &server_url},
        {CONFIG_SECTION_MAIN, "join_key", TOML_TYPE_STRING, &join_key},
        {CONFIG_SECTION_MAIN, "digest_key", TOML_TYPE_STRING, &digest_key},
        {CONFIG_SECTION_MAIN, "enable_join_key", TOML_TYPE_BOOL, &enable_join_key},
        {CONFIG_SECTION_UPDATES, "update_server", TOML_TYPE_STRING, &update_server_url},
        {CONFIG_SECTION_UPDATES, "enable_updates", TOML_TYPE_BOOL, &enable_updates},
    };

    TOMLApplyEntriesToKeyMap(entries, CONFIG_ENTRY_COUNT, key_map, CONFIG_ENTRY_COUNT);

    if (enable_updates && update_server_url) {
        if (sys_net_initialize_network() == 0) {
            sys_addr_t http_pool = NULL;
            sys_memory_allocate(SIZE_64K, SYS_MEMORY_PAGE_SIZE_64K, &http_pool);

            int err = DownloadUpdate((void *)http_pool, SIZE_64K, update_server_url);
            if (err == 1) {
                InstallUpdate("/dev_hdd0/plugins/patchwork.sprx");
                OPTION_DIALOG("Patchwork has updated, would you like to exit now?", ExitDialogCallback);
            }

            sys_memory_free(http_pool);

            sys_net_finalize_network();
        }
    }

    cellSysmoduleUnloadModule(CELL_SYSMODULE_HTTP);
    cellSysmoduleUnloadModule(CELL_SYSMODULE_NET);

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

    // Init patch generics
    void *network_key_offset = NULL;
    void *user_agent_offset = NULL;
    char user_agent[32];
    strcpy(user_agent, USER_AGENT);
    void *https_url_offset = NULL;
    void *http_url_offset = NULL;
    void *digest_offset = NULL;
    void *presence_url_offset = NULL;
    void *live_url_offset = NULL;
    void *notification_enable_offset = NULL;
    uint32_t notification_enable_instr = 0;
    void *rescheck_offset = NULL;
    void *rescheck_hook = NULL;
    void *forcejoin_patch_offset = NULL;
    void *forcejoin_patch = NULL;
    uint32_t forcejoin_patch_len = 0;

    uint8_t game = 0;

    if (!game && ((char *)LBP1_USER_AGENT_OFFSET)[15] == '$') {
        game = GAME_LBP1;
        network_key_offset = (void *)LBP1_NETWORK_KEY_OFFSET;
        user_agent_offset = (void *)LBP1_USER_AGENT_OFFSET;
        https_url_offset = (void *)LBP1_HTTPS_URL_OFFSET;
        http_url_offset = (void *)LBP1_HTTP_URL_OFFSET;
        digest_offset = (void *)LBP1_DIGEST_OFFSET;
        rescheck_offset = (void *)LBP1_RESOURCE_CHECK_OFFSET;
        rescheck_hook = LBP1ScriptHook;
        forcejoin_patch_offset = (void *)LBP1_FORCEJOIN_PATCH_OFFSET;
        forcejoin_patch = LBP1ForceJoinPatch;
        forcejoin_patch_len = LBP1_FORCEJOIN_PATCH_LENGTH;
    }

    if (!game && ((char *)LBP2_USER_AGENT_OFFSET)[18] == '2') {
        game = GAME_LBP2;
        network_key_offset = (void *)LBP2_NETWORK_KEY_OFFSET;
        user_agent_offset = (void *)LBP2_USER_AGENT_OFFSET;
        https_url_offset = (void *)LBP2_HTTPS_URL_OFFSET;
        http_url_offset = (void *)LBP2_HTTP_URL_OFFSET;
        digest_offset = (void *)LBP2_DIGEST_OFFSET;
        notification_enable_offset = (void *)LBP2_NOTIFICATION_ENABLE_OFFSET;
        notification_enable_instr = 0x38000000; // li r0, 0
        rescheck_offset = (void *)LBP2_RESOURCE_CHECK_OFFSET;
        rescheck_hook = LBP2ScriptHook;
        forcejoin_patch_offset = (void *)LBP2_FORCEJOIN_PATCH_OFFSET;
        forcejoin_patch = LBP2ForceJoinPatch;
        forcejoin_patch_len = LBP2_FORCEJOIN_PATCH_LENGTH;
    }

    if (!game && ((char *)LBP3_USER_AGENT_OFFSET)[18]) {
        game = GAME_LBP3;
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
        forcejoin_patch_offset = (void *)LBP3_FORCEJOIN_PATCH_OFFSET;
        forcejoin_patch = LBP3ForceJoinPatch;
        forcejoin_patch_len = LBP3_FORCEJOIN_PATCH_LENGTH;
    }

    if (!game && ((char *)LBP3_JP_USER_AGENT_OFFSET)[18]) {
        game = GAME_LBP3_JP;
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
        forcejoin_patch_offset = (void *)LBP3_JP_FORCEJOIN_PATCH_OFFSET;
        forcejoin_patch = LBP3JPForceJoinPatch;
        forcejoin_patch_len = LBP3_JP_FORCEJOIN_PATCH_LENGTH;
    }

    if (!game) {
        ERROR_DIALOG("Failed to detect game, your online is not safe!");
    } else {
        char game_num_str[4];
        UIntToStr(game_num_str, 4, game, 10);
        ReplaceNext(user_agent, 'X', *game_num_str);
        
        char *msg_buf = __builtin_alloca(sizeof(SUCCESS_MESSAGE_WITHOUT_PW));

        if (enable_join_key) {
            if (!join_key_randomized) {
                strcpy(msg_buf, SUCCESS_MESSAGE_WITH_PW);
            } else {
                strcpy(msg_buf, SUCCESS_MESSAGE_RANDOM_PW);
            }
        } else {
            strcpy(msg_buf, SUCCESS_MESSAGE_WITHOUT_PW);
        }

        ReplaceNext(msg_buf, 'X', *game_num_str);
        WriteFile("/dev_hdd0/tmp/wm_request", msg_buf, strlen(msg_buf));
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
    if (forcejoin_patch_offset && forcejoin_patch && forcejoin_patch_len) {
        memcpy(forcejoin_patch_offset, forcejoin_patch, forcejoin_patch_len);
    }

    // Exit

    return SYS_PRX_NO_RESIDENT;
}
