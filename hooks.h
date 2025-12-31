#ifndef PATCHWORK_HOOKS_H
#define PATCHWORK_HOOKS_H

#include <stdlib.h>

extern void *RNPCSRHook;

// Opcode for the branch instruction
#define OPCODE_B 0x48000000

#endif //PATCHWORK_HOOKS_H