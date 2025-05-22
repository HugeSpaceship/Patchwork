//
// Created by henry on 5/21/2025.
//

#include "memory.h"

#include <stdbool.h>
#include <sys/return_code.h>
#include <sys/syscall.h>
#include <sys/sys_types.h>

int sys_dbg_write_process_memory(sys_pid_t pid, const void* destination, const void* source, size_t size)
{
    system_call_4(905, pid, (uintptr_t)destination, size, (uintptr_t)source);
    return_to_user_prog(int);
}

__attribute__((noinline)) int PS3MAPISetMemory(sys_pid_t pid, const void* destination, const void* source, size_t size)
{
    system_call_6(SC_COBRA_SYSCALL8, SYSCALL8_OPCODE_PS3MAPI, PS3MAPI_OPCODE_SET_PROC_MEM, pid, (uintptr_t)destination, (uintptr_t)source, size);
    return_to_user_prog(int);
}

int WriteProcessMemory(uint32_t pid, const void* address, const void* data, size_t size)
{
    bool useHenSyscalls = false;

    if (!useHenSyscalls)
    {
        const int canWrite = sys_dbg_write_process_memory(pid, address, data, size);
        if (canWrite == SUCCEEDED)
        {
            return canWrite;
        }
    }

    useHenSyscalls = true;
    return PS3MAPISetMemory(pid, address, data, size);
}