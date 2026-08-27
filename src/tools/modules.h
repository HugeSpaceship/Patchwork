#include <stdint.h>

#include <cell/sysmodule.h>

#include "util.h"

static const int Modules[] = {
    CELL_SYSMODULE_FS, 
    CELL_SYSMODULE_NET,
    CELL_SYSMODULE_HTTP,
};

static const size_t ModuleCount = sizeof(Modules) / sizeof(int);

void LoadAllModules();
void UnloadAllModules();
