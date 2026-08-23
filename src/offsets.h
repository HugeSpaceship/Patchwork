#ifndef OFFSETS_H
#define OFFSETS_H

#include "globals.h"

// Global
#define LBP_PS3_PROD_URL_LENGTH 70
#define LBP_DIGEST_LENGTH       18
#define LBP_NETWORK_KEY_SIZE    16

// LBP1
#define LBP1_NETWORK_KEY_OFFSET         0x0074C418
#define LBP1_USER_AGENT_OFFSET          0x0079FDA0
#define LBP1_USER_AGENT_LENGTH                  68
#define LBP1_HTTPS_URL_OFFSET           0x00767C40
#define LBP1_HTTP_URL_OFFSET            0x00767CF8
#define LBP1_DIGEST_OFFSET              0x0079FBC0
#define LBP1_PLAYLIST_OFFSET            0x0074C2C0
#define LBP1_RESOURCE_CHECK_OFFSET      0x00081BC8
#define LBP1_FORCEJOIN_PATCH_OFFSET     0x003ab70c
#define LBP1_FORCEJOIN_PATCH_LENGTH             64

// LBP2
#define LBP2_NETWORK_KEY_OFFSET         0x00C248CC
#define LBP2_USER_AGENT_OFFSET          0x00C36320
#define LBP2_USER_AGENT_LENGTH                  22
#define LBP2_HTTPS_URL_OFFSET           0x00C51B68
#define LBP2_HTTP_URL_OFFSET            0x00C51C08
#define LBP2_DIGEST_OFFSET              0x00C361D0
#define LBP2_NOTIFICATION_ENABLE_OFFSET 0x0023B398
#define LBP2_RESOURCE_CHECK_OFFSET      0x00153D74
#define LBP2_FORCEJOIN_PATCH_OFFSET     0x00285fb8
#define LBP2_FORCEJOIN_PATCH_LENGTH            120

// LBP3 v1.26
#define LBP3_NETWORK_KEY_OFFSET         0x00D9D21C
#define LBP3_NAME_OFFSET                0x00EED406
#define LBP3_USER_AGENT_OFFSET          0x00D8BF50
#define LBP3_USER_AGENT_LENGTH                  22
#define LBP3_HTTPS_URL_OFFSET           0x00D9E5D8
#define LBP3_HTTP_URL_OFFSET            0x00D9E4C4
#define LBP3_DIGEST_OFFSET              0x00D8BD08
#define LBP3_PRESENCE_URL_OFFSET        0x00D8429C
#define LBP3_LIVE_URL_OFFSET            0x00D842EC
#define LBP3_NOTIFICATION_ENABLE_OFFSET 0x002C15D0
#define LBP3_RESOURCE_CHECK_OFFSET      0x001CFC6C
#define LBP3_FORCEJOIN_PATCH_OFFSET     0x0030561c
#define LBP3_FORCEJOIN_PATCH_LENGTH            112

// LBP3 v1.21 (Japanese latest)
#define LBP3_JP_NETWORK_KEY_OFFSET      0x00D8305C
#define LBP3_JP_NAME_OFFSET             0x00ECCE5E
#define LBP3_JP_USER_AGENT_OFFSET       0x00D71DE8
#define LBP3_JP_USER_AGENT_LENGTH               22
#define LBP3_JP_HTTPS_URL_OFFSET        0x00D84338
#define LBP3_JP_HTTP_URL_OFFSET         0x00D84224
#define LBP3_JP_DIGEST_OFFSET           0x00D71BA0
#define LBP3_JP_PRESENCE_URL_OFFSET     0x00D8429C
#define LBP3_JP_LIVE_URL_OFFSET         0x00D842EC
#define LBP3_JP_RESOURCE_CHECK_OFFSET   0x001CFB08
#define LBP3_JP_FORCEJOIN_PATCH_OFFSET  0x00301e34
#define LBP3_JP_FORCEJOIN_PATCH_LENGTH         112

typedef struct PatchOffsets {
    void *network_key;
    void *user_agent;
    void *https_url;
    void *http_url;
    void *digest;
    void *playlist;
    void *presence_url;
    void *live_url;
    void *notification_enable;
    void *rescheck;
    void *forcejoin_patch;
} PatchOffsets;

static PatchOffsets LBP1Offsets = {
    .network_key = (void *)LBP1_NETWORK_KEY_OFFSET,
    .user_agent = (void *)LBP1_USER_AGENT_OFFSET,
    .https_url = (void *)LBP1_HTTPS_URL_OFFSET,
    .http_url = (void *)LBP1_HTTP_URL_OFFSET,
    .digest = (void *)LBP1_DIGEST_OFFSET,
    .playlist = (void *)LBP1_PLAYLIST_OFFSET,
    .rescheck = (void *)LBP1_RESOURCE_CHECK_OFFSET,
    .forcejoin_patch = (void *)LBP1_FORCEJOIN_PATCH_OFFSET,
};

static PatchOffsets LBP2Offsets = {
    .network_key = (void *)LBP2_NETWORK_KEY_OFFSET,
    .user_agent = (void *)LBP2_USER_AGENT_OFFSET,
    .https_url = (void *)LBP2_HTTPS_URL_OFFSET,
    .http_url = (void *)LBP2_HTTP_URL_OFFSET,
    .digest = (void *)LBP2_DIGEST_OFFSET,
    .notification_enable = (void *)LBP2_NOTIFICATION_ENABLE_OFFSET,
    .rescheck = (void *)LBP2_RESOURCE_CHECK_OFFSET,
    .forcejoin_patch = (void *)LBP2_FORCEJOIN_PATCH_OFFSET,
};

static PatchOffsets LBP3Offsets = {
    .network_key = (void *)LBP3_NETWORK_KEY_OFFSET,
    .user_agent = (void *)LBP3_USER_AGENT_OFFSET,
    .https_url = (void *)LBP3_HTTPS_URL_OFFSET,
    .http_url = (void *)LBP3_HTTP_URL_OFFSET,
    .digest = (void *)LBP3_DIGEST_OFFSET,
    .presence_url = (void *)LBP3_PRESENCE_URL_OFFSET,
    .live_url = (void *)LBP3_LIVE_URL_OFFSET,
    .notification_enable = (void *)LBP3_NOTIFICATION_ENABLE_OFFSET,
    .rescheck = (void *)LBP3_RESOURCE_CHECK_OFFSET,
    .forcejoin_patch = (void *)LBP3_FORCEJOIN_PATCH_OFFSET,
};

static PatchOffsets LBP3JPOffsets = {
    .network_key = (void *)LBP3_JP_NETWORK_KEY_OFFSET,
    .user_agent = (void *)LBP3_JP_USER_AGENT_OFFSET,
    .https_url = (void *)LBP3_JP_HTTPS_URL_OFFSET,
    .http_url = (void *)LBP3_JP_HTTP_URL_OFFSET,
    .digest = (void *)LBP3_JP_DIGEST_OFFSET,
    .presence_url = (void *)LBP3_JP_PRESENCE_URL_OFFSET,
    .live_url = (void *)LBP3_JP_LIVE_URL_OFFSET,
    //.notification_enable = (void *)LBP3_JP_NOTIFICATION_ENABLE_OFFSET, // TODO: Find this
    .rescheck = (void *)LBP3_JP_RESOURCE_CHECK_OFFSET,
    .forcejoin_patch = (void *)LBP3_JP_FORCEJOIN_PATCH_OFFSET,
};

static GameNumber GetLBPGameNumber() {
    if (((char *)LBP1_USER_AGENT_OFFSET)[15] == '$')return GAME_LBP1;
    if (((char *)LBP2_USER_AGENT_OFFSET)[18] == '2') return GAME_LBP2;
    if (((char *)LBP3_USER_AGENT_OFFSET)[18]) return GAME_LBP3;
    if (((char *)LBP3_JP_USER_AGENT_OFFSET)[18]) return GAME_LBP3_JP;
    return GAME_UNKNOWN;
}

#endif //OFFSETS_H
