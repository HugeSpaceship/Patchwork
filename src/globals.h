#ifndef GLOBALS_H
#define GLOBALS_H

#include <cell/sysmodule.h>

#define STR1(x)  #x
#define STR(x)  STR1(x)

#define BUILD_VERSION = __DATE__ ""

#define MAIN_CONFIG_PATH "/dev_hdd0/plugins/patchwork/patchwork.toml"
#define CONFIG_SECTION_MAIN "patchwork"
#define CONFIG_SECTION_UPDATES "updates"
#define CONFIG_ENTRY_COUNT 6

#define SIZE_64K (64 * 1024)

#define SUCCESS_MESSAGE_WITH_PW "/popup.ps3?Patchwork%20" STR(PATCHWORK_VERSION_MAJOR)"." STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBPX%0ALobby%20password%20has%20been%20set&icon=8&snd=5"
#define SUCCESS_MESSAGE_RANDOM_PW "/popup.ps3?Patchwork%20" STR(PATCHWORK_VERSION_MAJOR)"." STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBPX%0ALobby%20password%20has%20been%20randomized&icon=8&snd=5"
#define SUCCESS_MESSAGE_WITHOUT_PW "/popup.ps3?Patchwork%20" STR(PATCHWORK_VERSION_MAJOR)"." STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBPX%0ALobby%20password%20is%20disabled&icon=8&snd=5"

static const int Modules[] = {
    CELL_SYSMODULE_FS, 
    CELL_SYSMODULE_NET,
    CELL_SYSMODULE_HTTP,
};

static const size_t ModuleCount = sizeof(Modules) / sizeof(int);

// Not reallyyy used right now, but could be useful later
typedef enum GameNumber {
    GAME_UNKNOWN = 0,
    GAME_LBP1 = 1,
    GAME_LBP2 = 2,
    GAME_LBP3 = 3,
    GAME_LBP3_JP = 4,
} GameNumber;

static int LoadAllModules() {
    int err;
    for (int i = 0; i < ModuleCount; i++) {
        err = cellSysmoduleLoadModule(Modules[i]);
        if (err != CELL_OK) break;
    }

    return err;
}

static int UnloadAllModules() {
    int err;
    for (int i = 0; i < ModuleCount; i++) {
        err = cellSysmoduleUnloadModule(Modules[i]);
        if (err != CELL_OK) break;
    }

    return err;
}

#endif //GLOBALS_H