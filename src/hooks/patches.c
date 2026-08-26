#include "patches.h"

char *ServerURL = NULL;
char *ServerDigest = NULL;
char *UserAgent = NULL;
unsigned char *NetworkKey = NULL;

uint32_t ScriptHookInstruction = 0;
uint32_t NotificationEnableInstruction = 0;
static uint8_t LBP1PlaylistOverride = 1;

GamePatch LBP1Patches[] = {
    {LBP1_NETWORK_KEY_OFFSET, &NetworkKey, LBP_NETWORK_KEY_SIZE, INDIRECT},
    {LBP1_USER_AGENT_OFFSET, &UserAgent, LBP_USER_AGENT_LENGTH, INDIRECT},
    {LBP1_HTTPS_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP1_HTTP_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP1_DIGEST_OFFSET, &ServerDigest, LBP_DIGEST_LENGTH, INDIRECT},
    {LBP1_PLAYLIST_OFFSET, &LBP1PlaylistOverride, sizeof(LBP1PlaylistOverride)},
    {LBP1_RESOURCE_CHECK_OFFSET, &ScriptHookInstruction, sizeof(ScriptHookInstruction)},
    {LBP1_FORCEJOIN_PATCH_OFFSET, LBP1ForceJoinPatch, LBP1_FORCEJOIN_PATCH_LENGTH},
};

GamePatch LBP2Patches[] = {
    {LBP2_NETWORK_KEY_OFFSET, &NetworkKey, LBP_NETWORK_KEY_SIZE, INDIRECT},
    {LBP2_USER_AGENT_OFFSET, &UserAgent, LBP_USER_AGENT_LENGTH, INDIRECT},
    {LBP2_HTTPS_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP2_HTTP_URL_OFFSET, &ServerURL, LBP_PS3_PROD_URL_LENGTH, INDIRECT},
    {LBP2_DIGEST_OFFSET, &ServerDigest, LBP_DIGEST_LENGTH, INDIRECT},
    {LBP2_NOTIFICATION_ENABLE_OFFSET, &NotificationEnableInstruction, sizeof(NotificationEnableInstruction)},
    {LBP2_RESOURCE_CHECK_OFFSET, &ScriptHookInstruction, sizeof(ScriptHookInstruction)},
    {LBP2_FORCEJOIN_PATCH_OFFSET, LBP2ForceJoinPatch, LBP2_FORCEJOIN_PATCH_LENGTH},
};

GamePatch LBP3Patches[] = {
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

GamePatch LBP3JPPatches[] = {
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

size_t LBP1PatchesCount = sizeof(LBP1Patches) / sizeof(GamePatch);
size_t LBP2PatchesCount = sizeof(LBP1Patches) / sizeof(GamePatch);
size_t LBP3PatchesCount = sizeof(LBP1Patches) / sizeof(GamePatch);
size_t LBP3JPPatchesCount = sizeof(LBP1Patches) / sizeof(GamePatch);

void ApplyGamePatches(GamePatch *patches, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (patches[i].offset == 0)
            break;

        if (patches[i].type == INDIRECT) {
            void *source = *(void **)patches[i].source;
            if (source) {
                LogLn("Applying indirect patch to offset 0x%X", patches[i].offset);
                memcpy((void *)patches[i].offset, source, patches[i].size);
            }
        }
        else if (patches[i].source) {
            LogLn("Applying direct patch to offset 0x%X", patches[i].offset);
            memcpy((void *)patches[i].offset, patches[i].source, patches[i].size);
        }
    }
}

GameNumber GetLBPGameNumber() {
    if (((char *)LBP1_USER_AGENT_OFFSET)[15] == '$')return GAME_LBP1;
    if (((char *)LBP2_USER_AGENT_OFFSET)[18] == '2') return GAME_LBP2;
    if (((char *)LBP3_USER_AGENT_OFFSET)[18] == '3') return GAME_LBP3;
    if (((char *)LBP3_JP_USER_AGENT_OFFSET)[18] == '3') return GAME_LBP3_JP;
    return GAME_UNKNOWN;
}
