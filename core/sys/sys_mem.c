#include "sys_mem.h"

SYSCALL_DEFINE1(0, allocation_size) {
    const uint32_t addr = (uint32_t)kmalloc(allocation_size);
    SYSCALL_RETURN(addr);
}

SYSCALL_DEFINE1(1, address) {
    kfree((void*)address);
    SYSCALL_EXIT;
}

SYSCALL_DEFINE2(2, num_blocks, count) {
    const uint32_t allocation_size = num_blocks * count;

    char *addr = (char*)kmalloc(allocation_size);
    memset(addr, 0, allocation_size);

    return (uint32_t)addr;
}
