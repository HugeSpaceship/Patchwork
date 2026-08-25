#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/prx.h>
#include <sys/sys_time.h>
#include <sys/memory.h>

#include <cell/hash/libsha256.h>
#include <cell/sysmodule.h>

#include "hooks/hooks.h"
#include "hooks/patches.h"
#include "hooks/script-block.h"

#include "tools/util.h"
#include "tools/modules.h"
#include "tools/fs.h"
#include "offsets.h"
#include "update.h"
#include "globals.h"

#include "toml/toml.h"
#include "toml/helper.h"
#include "toml/keymap.h"
#include "toml/tokenizer.h"

SYS_MODULE_INFO(PatchworkLBP, 0, PATCHWORK_VERSION_MAJOR, PATCHWORK_VERSION_MINOR);
SYS_MODULE_START(start);

#define USER_AGENT "PatchworkLBPX " STR(PATCHWORK_VERSION_MAJOR) "." STR(PATCHWORK_VERSION_MINOR)

typedef struct PatchworkConfigOptions {
    char *server_url;
    char *join_key;
    char *digest_key;
    int enable_join_key;

    char *update_server_url;
    int enable_updates;
} PatchworkConfigOptions;

int CopyHashedJoinKey(unsigned char *hash_buf, char *join_key) {
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

int start(size_t args, void *argp) {
    PatchworkLaunchArgs *launch_args = NULL;
    if (args > 0 && argp) {
        launch_args = argp;
        println("PRX was reloaded");
        INFO_DIALOG("Patchwork has been updated to version " STR(PATCHWORK_VERSION_MAJOR) "." STR(PATCHWORK_VERSION_MINOR));
    }

    LoadAllModules();

    char toml_buf[312];
    ReadFile(MAIN_CONFIG_PATH, toml_buf, sizeof(toml_buf));

    Lexer l = MakeLexer(toml_buf);
    TOMLEntry entries[CONFIG_ENTRY_COUNT];
    TOMLReadBuffer(&l, entries, CONFIG_ENTRY_COUNT);

    println("Parsed toml buffer");

    PatchworkConfigOptions options;

    TOMLKeyMap key_map[] = {
        {CONFIG_SECTION_MAIN, "server_url", TOML_TYPE_STRING, &options.server_url},
        {CONFIG_SECTION_MAIN, "join_key", TOML_TYPE_STRING, &options.join_key},
        {CONFIG_SECTION_MAIN, "digest_key", TOML_TYPE_STRING, &options.digest_key},
        {CONFIG_SECTION_MAIN, "enable_join_key", TOML_TYPE_BOOL, &options.enable_join_key},
        {CONFIG_SECTION_UPDATES, "update_server", TOML_TYPE_STRING, &options.update_server_url},
        {CONFIG_SECTION_UPDATES, "enable_updates", TOML_TYPE_BOOL, &options.enable_updates},
    };

    println("Mapped config entries to data structure");

    TOMLApplyEntriesToKeyMap(entries, CONFIG_ENTRY_COUNT, key_map, CONFIG_ENTRY_COUNT);

    if (launch_args && !launch_args->updated) {
        if (options.enable_updates && options.update_server_url)
            if (TryUpdateAndInstall(options.update_server_url)) TryRestartModule();
    }

    // Update needed static patch pointers
    unsigned char join_key_hash[32];
    int join_key_randomized = 0;
    if (options.enable_join_key) {
        join_key_randomized = CopyHashedJoinKey(join_key_hash, options.join_key);
        NetworkKey = join_key_hash;
    }
    println("Setup join key");

    if (options.server_url) ServerURL = TrimEnd(options.server_url);
    if (options.digest_key) ServerDigest = TrimEnd(options.digest_key);

    println("Trimmed server url and digest keys");

    // Init patch generics
    GamePatch *patches = NULL;
    size_t patch_count = 0;

    GameNumber game = GetLBPGameNumber();
    switch (game) {
        case GAME_LBP1:
            patches = LBP1Patches;
            patch_count = sizeof(LBP1Patches) / sizeof(GamePatch);
            ScriptHookInstruction = RelativeBranch(LBP1ScriptHook, (void *)LBP1_RESOURCE_CHECK_OFFSET);
            break;
        case GAME_LBP2:
            println("Game = LBP2");
            patches = LBP2Patches;
            patch_count = sizeof(LBP2Patches) / sizeof(GamePatch);
            NotificationEnableInstruction = 0x38000000; // li r0, 0
            ScriptHookInstruction = RelativeBranch(LBP2ScriptHook, (void *)LBP2_RESOURCE_CHECK_OFFSET);
            println("Set LBP2 static patches");
            break;
        case GAME_LBP3:
            patches = LBP3Patches;
            patch_count = sizeof(LBP3Patches) / sizeof(GamePatch);
            NotificationEnableInstruction = 0x38600000; // li r3, 0
            ScriptHookInstruction = RelativeBranch(LBP3ScriptHook, (void *)LBP3_RESOURCE_CHECK_OFFSET);
            break;
        case GAME_LBP3_JP:
            patches = LBP3JPPatches;
            patch_count = sizeof(LBP3JPPatches) / sizeof(GamePatch);
            // NotificationEnableInstruction = 0x38600000; // TODO: Find this
            ScriptHookInstruction = RelativeBranch(LBP3JPScriptHook, (void *)LBP3_JP_RESOURCE_CHECK_OFFSET);
            break;
        default:
            // TODO: Procedurally build a partial patch list by scanning memory for common values
            break;
    }

    if (!game) {
        ERROR_DIALOG("Failed to detect game, your online is not safe!");
    } else {
        char user_agent[64];

        char game_num_str[4];
        UIntToStr(game_num_str, sizeof(game_num_str), game, 10);

        strcpy(user_agent, USER_AGENT);
        ReplaceNext(user_agent, 'X', *game_num_str);

        UserAgent = &user_agent[0];
        
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

        ApplyGamePatches(patches, patch_count);
    }

    // Exit
    UnloadAllModules();

    return SYS_PRX_NO_RESIDENT;
}
