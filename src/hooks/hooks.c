#include "hooks.h"
#include <stdint.h>

uint32_t RelativeBranch(void *destination, void *origin) {
    return OPCODE_B + (((uint32_t)destination - (uint32_t)origin) & 0x3FFFFFF);
}