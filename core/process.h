#ifndef CORE_PROCESS_H
#define CORE_PROCESS_H

#include "virtual_memory_manager.h"
#include "file.h"
#include "asm_utils.h"
#include "util.h"
#include "elf.h"

// the base pointer for all processes - all processes are virtually mapped to
// this index
#define PROCESS_BASE_POINTER ((uint32_t)0x00100000)

// the base stack pointer for the user stack
#define PROCESS_STACK_POINTER ((uint32_t)0xBFFFF000)

// the base of the stack, sits at the stack pointer - 8MiB because allocations
// happen upwards, whereas the stack grows down
#define PROCESS_STACK_BASE ((uint32_t)0xBF800000)

// the size of the stack in bytes (8MiB)
#define PROCESS_STACK_SIZE ((uint32_t)0x800000)


typedef struct {
    // the user esp is loaded as the stack pointer whenever a new process is
    // spawned. Starts at 0xBFFFF000 and represents an 8MiB stack
    uint32_t user_esp;

    // the kernel esp is the stack pointer used by the kernel, and is restored
    // whenever a process finishes executing
    uint32_t kernel_esp;

    // pointer to the top of the user stack space
    char *user_stack;
} ProcessHandler;

// initializes the process structure
void process_init();

// spawns a process from a file by directly placing its bytes in memory and
// jumping to them
uint32_t process_raw_spawn(File*);

// spawns a process from a given ELF file
uint32_t process_spawn(ELF32*, char*);
#endif
