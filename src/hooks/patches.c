#include "patches.h"

void ApplyGamePatches(GamePatch *patches, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (patches[i].offset == 0)
            break;

        if (patches[i].type == INDIRECT) {
            void *source = *(void **)patches[i].source;
            if (source) {
                println(source);
                memcpy((void *)patches[i].offset, source, patches[i].size);
            }
        }
        else if (patches[i].source) {
            memcpy((void *)patches[i].offset, patches[i].source, patches[i].size);
        }
    }
}