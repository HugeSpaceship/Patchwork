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

extern char *ServerURL;
extern char *ServerDigest;
extern char *UserAgent;
extern unsigned char *NetworkKey;

extern GamePatch LBP1Patches[];
extern GamePatch LBP2Patches[];
extern GamePatch LBP3Patches[];
extern GamePatch LBP3JPPatches[];

extern size_t LBP1PatchesCount;
extern size_t LBP2PatchesCount;
extern size_t LBP3PatchesCount;
extern size_t LBP3JPPatchesCount;

extern uint32_t ScriptHookInstruction;
extern uint32_t NotificationEnableInstruction;

void ApplyGamePatches(GamePatch *patches, size_t count);

GameNumber GetLBPGameNumber();

#endif //PATCHES_H