#include "hooks.h"

// TODO: If we have a debug log level, we can exclusively do this log to the tty without having to call PrintLn

uint32_t RelativeBranch(void *destination, void *origin) {
    PrintLn("Generating relative branch from %04X to %04X", (uint32_t)origin, (uint32_t)destination);
    return OPCODE_B + (((uint32_t)destination - (uint32_t)origin) & 0x3FFFFFF);
}