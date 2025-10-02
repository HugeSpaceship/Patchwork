//
// Created by henry on 5/21/2025.
//

#ifndef MEMORY_H
#define MEMORY_H

#define SC_COBRA_SYSCALL8                               8
#define SYSCALL8_OPCODE_PS3MAPI                    0x7777
#define PS3MAPI_OPCODE_SET_PROC_MEM                0x0032
#define PS3MAPI_OPCODE_GET_PROC_MEM				   0x0031
#include <sys/sys_types.h>

int sys_dbg_write_process_memory(sys_pid_t pid, const void *destination, const void *source, size_t size);
int sys_dbg_read_process_memory(sys_pid_t pid, void *destination, void *source, size_t size);

__attribute__((noinline)) int PS3MAPIGetMemory(sys_pid_t pid, void *destination, void *source, size_t size);
__attribute__((noinline)) int PS3MAPISetMemory(sys_pid_t pid, const void *destination, const void *source, size_t size);

int WriteProcessMemory(uint32_t pid, const void *address, const void *data, size_t size);
int ReadProcessMemory(uint32_t pid, void *address, void *data, size_t size);

#endif //MEMORY_H
