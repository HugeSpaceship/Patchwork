#ifndef PATCHWORK_HOOKS_H
#define PATCHWORK_HOOKS_H

#include <stdint.h>

// Opcode for the branch instruction
#define OPCODE_B 0x48000000

uint32_t RelativeBranch(void *destination, void *origin);

#endif //PATCHWORK_HOOKS_H