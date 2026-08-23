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

// For future modifications to launch args, existing ones should remain in the current order
typedef struct PatchworkLaunchArgs {
    sys_prx_id_t old_process;
    int updated;
} PatchworkLaunchArgs;

typedef struct PatchworkConfigOptions {
    char *server_url;
    char *join_key;
    char *digest_key;
    int enable_join_key;

    char *update_server_url;
    int enable_updates;
} PatchworkConfigOptions;

int GetHashedJoinKey(unsigned char *hash_buf, char *join_key) {
    int is_randomized = 0;

    if (join_key) {
        join_key = TrimEnd(join_key);
        // Hash the lobby password so we get an unrecoverable string of a fixed length
        cellSha256Digest(join_key, strlen(join_key), hash_buf);
    } else {
        // Generate random key
        sys_time_sec_t sec = 0;
        sys_time_nsec_t nsec = 0;
        sys_time_get_current_time(&sec, &nsec);
        uint64_t combined_time = nsec + sec;
        cellSha256Digest(&combined_time, sizeof(uint64_t), hash_buf);
        is_randomized = 1;
    }

    return is_randomized;
}

int TryUpdate(char *url) {
    if (sys_net_initialize_network() == 0) {
        return 2;
    }

    sys_addr_t http_pool = NULL;
    sys_memory_allocate(SIZE_64K, SYS_MEMORY_PAGE_SIZE_64K, &http_pool);
    int err = DownloadUpdate((void *)http_pool, SIZE_64K, url);
    sys_memory_free(http_pool);
    sys_net_finalize_network();

    if (err == 1) {
        InstallUpdate("/dev_hdd0/plugins/patchwork.sprx");

        println("Restarting module");

        int result = 0;
        sys_prx_id_t my_id = sys_prx_get_my_module_id();
        sys_prx_id_t new_prx_id = sys_prx_load_module(INSTALL_PATH, 0, NULL);

        PatchworkLaunchArgs args = { my_id, 1 };

        int ret = sys_prx_start_module(new_prx_id, 1, &args, &result, 0, NULL);
        if (ret < CELL_OK) {
            ERROR_DIALOG("Failed to restart patchwork");
        }
    }
    if (err == 2) {
        println("Update failed");
        ERROR_DIALOG("Failed to update patchwork");
    }
    if (err == 1) {
        println("No patchwork update available");
    }

    return err;
}

int start(size_t args, void *argp) {
    PatchworkLaunchArgs *launch_args = NULL;
    if (argp) {
        launch_args = argp;
        println("PRX was reloaded");
        INFO_DIALOG("Patchwork has been updated to version " STR(PATCHWORK_VERSION_MAJOR) "." STR(PATCHWORK_VERSION_MINOR));
    }

    if (!launch_args->updated && LoadAllModules() != CELL_OK) {
        println("Failed to load sysmodules");
        return SYS_PRX_STOP_FAILED;
    }

    char toml_buf[312];
    ReadFile(MAIN_CONFIG_PATH, toml_buf, sizeof(toml_buf));

    Lexer l = MakeLexer(toml_buf);
    TOMLEntry entries[CONFIG_ENTRY_COUNT];
    TOMLReadBuffer(&l, entries, CONFIG_ENTRY_COUNT);

    PatchworkConfigOptions options;

    TOMLKeyMap key_map[] = {
        {CONFIG_SECTION_MAIN, "server_url", TOML_TYPE_STRING, &options.server_url},
        {CONFIG_SECTION_MAIN, "join_key", TOML_TYPE_STRING, &options.join_key},
        {CONFIG_SECTION_MAIN, "digest_key", TOML_TYPE_STRING, &options.digest_key},
        {CONFIG_SECTION_MAIN, "enable_join_key", TOML_TYPE_BOOL, &options.enable_join_key},
        {CONFIG_SECTION_UPDATES, "update_server", TOML_TYPE_STRING, &options.update_server_url},
        {CONFIG_SECTION_UPDATES, "enable_updates", TOML_TYPE_BOOL, &options.enable_updates},
    };

    TOMLApplyEntriesToKeyMap(entries, CONFIG_ENTRY_COUNT, key_map, CONFIG_ENTRY_COUNT);

    if (options.enable_updates && options.update_server_url) {
        int err = TryUpdate(options.update_server_url);
    }

    UnloadAllModules();

    unsigned char xxtea_key[32];

    int join_key_randomized = GetHashedJoinKey(xxtea_key, options.join_key);

    // Init patch generics
    PatchOffsets *offsets;

    void *rescheck_hook = NULL;
    void *forcejoin_patch = NULL;
    uint32_t notification_enable_instr = 0;
    uint32_t forcejoin_patch_len = 0;

    char user_agent_buf[32];
    strcpy(user_agent_buf, USER_AGENT);

    GameNumber game = GetLBPGameNumber();

    if (game == GAME_LBP1) {
        offsets = &LBP1Offsets;
        rescheck_hook = LBP1ScriptHook;
        forcejoin_patch = LBP1ForceJoinPatch;
        forcejoin_patch_len = LBP1_FORCEJOIN_PATCH_LENGTH;
    }

    if (game == GAME_LBP2) {
        offsets = &LBP2Offsets;
        rescheck_hook = LBP2ScriptHook;
        forcejoin_patch = LBP2ForceJoinPatch;
        forcejoin_patch_len = LBP2_FORCEJOIN_PATCH_LENGTH;
        notification_enable_instr = 0x38000000; // li r0, 0
    }

    if (game == GAME_LBP3) {
        offsets = &LBP3Offsets;
        rescheck_hook = LBP3ScriptHook;
        forcejoin_patch = LBP3ForceJoinPatch;
        forcejoin_patch_len = LBP3_FORCEJOIN_PATCH_LENGTH;
        notification_enable_instr = 0x38600000; // li r3, 0
    }

    if (game == GAME_LBP3_JP) {
        offsets = &LBP3JPOffsets;
        rescheck_hook = LBP3JPScriptHook;
        forcejoin_patch = LBP3JPForceJoinPatch;
        forcejoin_patch_len = LBP3_JP_FORCEJOIN_PATCH_LENGTH;
        // notification_enable_instr = 0x38600000; // TODO: Find this
    }

    if (!game) {
        ERROR_DIALOG("Failed to detect game, your online is not safe!");
    } else {
        char game_num_str[4];
        UIntToStr(game_num_str, 4, game, 10);
        ReplaceNext(user_agent_buf, 'X', *game_num_str);
        
        char *msg_buf = __builtin_alloca(sizeof(SUCCESS_MESSAGE_WITHOUT_PW));

        if (options.enable_join_key) {
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
    if(options.server_url) {
        options.server_url = TrimEnd(options.server_url);
        url_len = strlen(options.server_url) + 1;
    }

    // Write to the chosen offets
    if (options.enable_join_key && offsets->network_key)
        memcpy(offsets->network_key, xxtea_key, LBP_NETWORK_KEY_SIZE);
    if (offsets->user_agent)
        memcpy(offsets->user_agent, user_agent_buf, strlen(user_agent_buf) + 1);
    if (offsets->https_url && options.server_url)
        memcpy(offsets->https_url, options.server_url, url_len);
    if (offsets->http_url && options.server_url)
        memcpy(offsets->http_url, options.server_url, url_len);
    if (offsets->digest && options.digest_key) {
        options.digest_key = TrimEnd(options.digest_key);
        memcpy(offsets->forcejoin_patch, options.digest_key, LBP_DIGEST_LENGTH);
    }
    if (offsets->presence_url && options.server_url)
        memcpy(offsets->presence_url, options.server_url, url_len);
    if (offsets->live_url && options.server_url)
        memcpy(offsets->live_url, options.server_url, url_len);
    if (offsets->notification_enable && notification_enable_instr)
        memcpy(offsets->notification_enable, &notification_enable_instr, 4);
    if (offsets->rescheck && rescheck_hook) {
        uint32_t rescheck_instr = RelativeBranch(rescheck_hook, offsets->rescheck);
        memcpy(offsets->rescheck, &rescheck_instr, 4);
    }
    if (offsets->forcejoin_patch && forcejoin_patch && forcejoin_patch_len) {
        memcpy(offsets->forcejoin_patch, forcejoin_patch, forcejoin_patch_len);
    }

    // Exit

    return SYS_PRX_NO_RESIDENT;
}
