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

uint32_t process_raw_spawn(File *program) {
    char *executable = file_get_buffer(program);
    uint32_t executable_length = file_get_pointer(program);

    char *executable_space = vmm_map_memory(
        (page_aligned_ptr) PROCESS_BASE_POINTER,
        executable_length
    );
    memcpy(executable_space, executable, executable_length);

    // finally, jump to the executable
    asm_call((uint32_t)PROCESS_BASE_POINTER);

    // TODO: Return PID
    return 0;
}

uint32_t process_spawn(ELF32* elf, char *file) {
    for (uint32_t i = 0; i < elf->header.section_header_num; i++) {
        if (elf->section_headers[i].sh_type == ELF_SHT_PROGBITS) {
            char *location = vmm_map_memory(
                (page_aligned_ptr)elf->header.entry,
                4096
            );
            memcpy(
                (uint8_t*)elf->header.entry,
                file + elf->section_headers[i].sh_offset,
                elf->section_headers[i].sh_size
            );
        }
    }
    asm_call(0x100000);
    return 0;
}
