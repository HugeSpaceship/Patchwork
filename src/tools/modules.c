#include "modules.h"

// TODO: Proper logging
void LoadAllModules() {
    for (int i = 0; i < ModuleCount; i++) {
        if (cellSysmoduleLoadModule(Modules[i])) 
            LogLn("Failed to load module of id: %d", i);
    }
}

void UnloadAllModules() {
    for (int i = 0; i < ModuleCount; i++) {
        if (cellSysmoduleUnloadModule(Modules[i])) 
            LogLn("Failed to unload module of id: %d", i);
    }
}
