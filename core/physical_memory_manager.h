#ifndef _CORE_PHYSICAL_MEMORY_ALLOCATOR
#define _CORE_PHYSICAL_MEMORY_ALLOCATOR

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "format.h"
#include "util.h"

#include "../kernel/kinfo.h"

typedef struct {
    // pointer to the start of usable RAM
    uint8_t *memory_base;

    // memory base size in bytes (how large the memory base is)
    size_t mb_size;
} PhysicalMemoryManager;

extern PhysicalMemoryManager pmm;

// initializes the physical memory manager and finds the largest space of
// available RAM and allocates a 4096 byte aligned offset as the start of
// physically addressable memory
void pmm_init(multiboot_info_t*);

// reads the cr3 paging register into a u32
uint32_t pmm_read_cr3();

#endif
