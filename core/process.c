#include "process.h"

static ProcessHandler process_handler;

void process_init() {
    // this function actually does several things: initializes the process
    // manager and also switches the stack
    process_handler.user_esp = PROCESS_STACK_POINTER;
    process_handler.user_stack = vmm_map_memory(
        (page_aligned_ptr)PROCESS_STACK_BASE,
        PROCESS_STACK_SIZE
    );

    if (!process_handler.user_stack) {
        kpanic("Could not allocate enough space for user stack");
    }
}

uint32_t process_spawn(File *program) {
    char *executable = file_get_buffer(program);
    uint32_t executable_length = file_get_pointer(program);

    char *executable_space = vmm_map_memory(
        (page_aligned_ptr) PROCESS_BASE_POINTER,
        executable_length
    );
    memcpy(executable_space, executable, executable_length);

    // finally, jump to the executable
    asm_call((uint32_t)PROCESS_BASE_POINTER);

    printf("WE R OK");
    return 0;
}
