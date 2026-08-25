#ifndef PATCHES_H
#define PATCHES_H

#include <stddef.h>

#include "../tools/util.h"
#include "../offsets.h"
#include "../globals.h"

#include "forcejoin-patch.h"

// Maybe we can add other patch types later, this is all i need for now
typedef enum PatchType {
    DIRECT,
    INDIRECT,    
} PatchType;

typedef struct GamePatch {
    uint32_t offset;
    void *source;
    size_t size;
    PatchType type;
} GamePatch;

// Initialize any static memory to overwrite later
static char *ServerURL = NULL;
static char *ServerDigest = NULL;
static char *UserAgent = NULL;
static unsigned char *NetworkKey = NULL;

static uint32_t ScriptHookInstruction = 0;
static uint32_t NotificationEnableInstruction = 0;
static uint8_t LBP1PlaylistOverride = 1;

static GamePatch LBP1Patches[] = {
    {LBP1_NETWORK_KEY_OFFSET, &NetworkKey, LBP_NETWORK_KEY_SIZE, INDIRECT},
    {LBP1_USER_AGENT_OFFSET, &UserAgent, LBP_USER_AGENT_LENGTH, INDIRECT},
    {LBP1_HTTPS_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP1_HTTP_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP1_DIGEST_OFFSET, &ServerDigest, LBP_DIGEST_LENGTH, INDIRECT},
    {LBP1_PLAYLIST_OFFSET, &LBP1PlaylistOverride, sizeof(LBP1PlaylistOverride)},
    {LBP1_RESOURCE_CHECK_OFFSET, &ScriptHookInstruction, sizeof(ScriptHookInstruction)},
    {LBP1_FORCEJOIN_PATCH_OFFSET, LBP1ForceJoinPatch, LBP1_FORCEJOIN_PATCH_LENGTH},
};

static GamePatch LBP2Patches[] = {
    {LBP2_NETWORK_KEY_OFFSET, &NetworkKey, LBP_NETWORK_KEY_SIZE, INDIRECT},
    {LBP2_USER_AGENT_OFFSET, &UserAgent, LBP_USER_AGENT_LENGTH, INDIRECT},
    {LBP2_HTTPS_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP2_HTTP_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP2_DIGEST_OFFSET, &ServerDigest, LBP_DIGEST_LENGTH, INDIRECT},
    {LBP2_NOTIFICATION_ENABLE_OFFSET, &NotificationEnableInstruction, sizeof(NotificationEnableInstruction)},
    {LBP2_RESOURCE_CHECK_OFFSET, &ScriptHookInstruction, sizeof(ScriptHookInstruction)},
    {LBP2_FORCEJOIN_PATCH_OFFSET, LBP2ForceJoinPatch, LBP2_FORCEJOIN_PATCH_LENGTH},
};

static GamePatch LBP3Patches[] = {
    {LBP3_NETWORK_KEY_OFFSET, &NetworkKey, LBP_NETWORK_KEY_SIZE, INDIRECT},
    {LBP3_USER_AGENT_OFFSET, &UserAgent, LBP_USER_AGENT_LENGTH, INDIRECT},
    {LBP3_HTTPS_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_HTTP_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_PRESENCE_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_LIVE_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_DIGEST_OFFSET, &ServerDigest, LBP_DIGEST_LENGTH, INDIRECT},
    {LBP3_NOTIFICATION_ENABLE_OFFSET, &NotificationEnableInstruction, sizeof(NotificationEnableInstruction)},
    {LBP3_RESOURCE_CHECK_OFFSET, &ScriptHookInstruction, sizeof(ScriptHookInstruction)},
    {LBP3_FORCEJOIN_PATCH_OFFSET, LBP3ForceJoinPatch, LBP3_FORCEJOIN_PATCH_LENGTH},
};

static GamePatch LBP3JPPatches[] = {
    {LBP3_JP_NETWORK_KEY_OFFSET, &NetworkKey, LBP_NETWORK_KEY_SIZE, INDIRECT},
    {LBP3_JP_USER_AGENT_OFFSET, &UserAgent, LBP_USER_AGENT_LENGTH, INDIRECT},
    {LBP3_JP_HTTPS_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_JP_HTTP_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_JP_PRESENCE_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_JP_LIVE_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP3_JP_DIGEST_OFFSET, &ServerDigest, LBP_DIGEST_LENGTH, INDIRECT},
    {LBP3_JP_NOTIFICATION_ENABLE_OFFSET, &NotificationEnableInstruction, sizeof(NotificationEnableInstruction)},
    {LBP3_JP_RESOURCE_CHECK_OFFSET, &ScriptHookInstruction, sizeof(ScriptHookInstruction)},
    {LBP3_JP_FORCEJOIN_PATCH_OFFSET, LBP3JPForceJoinPatch, LBP3_JP_FORCEJOIN_PATCH_LENGTH},
};

static GameNumber GetLBPGameNumber() {
    if (((char *)LBP1_USER_AGENT_OFFSET)[15] == '$')return GAME_LBP1;
    if (((char *)LBP2_USER_AGENT_OFFSET)[18] == '2') return GAME_LBP2;
    if (((char *)LBP3_USER_AGENT_OFFSET)[18]) return GAME_LBP3;
    if (((char *)LBP3_JP_USER_AGENT_OFFSET)[18]) return GAME_LBP3_JP;
    return GAME_UNKNOWN;
}

void ApplyGamePatches(GamePatch *patches, size_t count);

#endif //PATCHES_H