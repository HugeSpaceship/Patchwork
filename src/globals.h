#ifndef GLOBALS_H
#define GLOBALS_H

#define STR1(x)  #x
#define STR(x)  STR1(x)

#define MAIN_CONFIG_PATH "/dev_hdd0/plugins/patchwork/patchwork.toml"
#define MAIN_CONFIG_SECTION "patchwork"

#define SUCCESS_MESSAGE_WITH_PW "/popup.ps3?Patchwork%20" STR(PATCHWORK_VERSION_MAJOR)"." STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBP2%0ALobby%20password%20has%20been%20set&icon=8&snd=5"
#define SUCCESS_MESSAGE_RANDOM_PW "/popup.ps3?Patchwork%20" STR(PATCHWORK_VERSION_MAJOR)"." STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBP2%0ALobby%20password%20has%20been%20randomized&icon=8&snd=5"
#define SUCCESS_MESSAGE_WITHOUT_PW "/popup.ps3?Patchwork%20" STR(PATCHWORK_VERSION_MAJOR)"." STR(PATCHWORK_VERSION_MINOR)"%20Loaded%20for%20LBP2%0ALobby%20password%20is%20disabled&icon=8&snd=5"

// Not reallyyy used right now, but could be useful later
enum GameNumber {
    GAME_LBP1 = 1,
    GAME_LBP2 = 2,
    GAME_LBP3 = 3,
    GAME_LBP3_JP = 4,
};

#endif //GLOBALS_H