#ifndef OFFSETS_H
#define OFFSETS_H

#include <stdint.h>

#include "globals.h"
#include "hooks/hooks.h"
#include "hooks/script-block.h"
#include "hooks/forcejoin-patch.h"

// Global
#define LBP_PS3_PROD_URL_LENGTH 70
#define LBP_DIGEST_LENGTH       18
#define LBP_NETWORK_KEY_SIZE    16
#define LBP_USER_AGENT_LENGTH   22 // Use minimum known size

// LBP1
#define LBP1_NETWORK_KEY_OFFSET            0x0074C418
#define LBP1_USER_AGENT_OFFSET             0x0079FDA0
#define LBP1_USER_AGENT_LENGTH                     68
#define LBP1_HTTPS_URL_OFFSET              0x00767C40
#define LBP1_HTTP_URL_OFFSET               0x00767CF8
#define LBP1_DIGEST_OFFSET                 0x0079FBC0
#define LBP1_PLAYLIST_OFFSET               0x0074C2C0
#define LBP1_RESOURCE_CHECK_OFFSET         0x00081BC8
#define LBP1_FORCEJOIN_PATCH_OFFSET        0x003ab70c
#define LBP1_FORCEJOIN_PATCH_LENGTH                64

// LBP2
#define LBP2_NETWORK_KEY_OFFSET            0x00C248CC
#define LBP2_USER_AGENT_OFFSET             0x00C36320
#define LBP2_USER_AGENT_LENGTH                     22
#define LBP2_HTTPS_URL_OFFSET              0x00C51B68
#define LBP2_HTTP_URL_OFFSET               0x00C51C08
#define LBP2_DIGEST_OFFSET                 0x00C361D0
#define LBP2_NOTIFICATION_ENABLE_OFFSET    0x0023B398
#define LBP2_RESOURCE_CHECK_OFFSET         0x00153D74
#define LBP2_FORCEJOIN_PATCH_OFFSET        0x00285fb8
#define LBP2_FORCEJOIN_PATCH_LENGTH               120

// LBP3 v1.26
#define LBP3_NETWORK_KEY_OFFSET            0x00D9D21C
#define LBP3_NAME_OFFSET                   0x00EED406
#define LBP3_USER_AGENT_OFFSET             0x00D8BF50
#define LBP3_USER_AGENT_LENGTH                     22
#define LBP3_HTTPS_URL_OFFSET              0x00D9E5D8
#define LBP3_HTTP_URL_OFFSET               0x00D9E4C4
#define LBP3_DIGEST_OFFSET                 0x00D8BD08
#define LBP3_PRESENCE_URL_OFFSET           0x00D9E53C
#define LBP3_LIVE_URL_OFFSET               0x00D9E58C
#define LBP3_NOTIFICATION_ENABLE_OFFSET    0x002C15D0
#define LBP3_RESOURCE_CHECK_OFFSET         0x001CFC6C
#define LBP3_FORCEJOIN_PATCH_OFFSET        0x0030561C
#define LBP3_FORCEJOIN_PATCH_LENGTH               112

// LBP3 v1.21 (Japanese latest)
#define LBP3_JP_NETWORK_KEY_OFFSET         0x00D8305C
#define LBP3_JP_NAME_OFFSET                0x00ECCE5E
#define LBP3_JP_USER_AGENT_OFFSET          0x00D71DE8
#define LBP3_JP_USER_AGENT_LENGTH                  22
#define LBP3_JP_HTTPS_URL_OFFSET           0x00D84338
#define LBP3_JP_HTTP_URL_OFFSET            0x00D84224
#define LBP3_JP_DIGEST_OFFSET              0x00D71BA0
#define LBP3_JP_PRESENCE_URL_OFFSET        0x00D8429C
#define LBP3_JP_LIVE_URL_OFFSET            0x00D842EC
#define LBP3_JP_NOTIFICATION_ENABLE_OFFSET 0x002BF520
#define LBP3_JP_RESOURCE_CHECK_OFFSET      0x001CFB08
#define LBP3_JP_FORCEJOIN_PATCH_OFFSET     0x00301e34
#define LBP3_JP_FORCEJOIN_PATCH_LENGTH            112

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

#endif //OFFSETS_H
