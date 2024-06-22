#ifndef _CORE_PHYSICAL_MEMORY_ALLOCATOR
#define _CORE_PHYSICAL_MEMORY_ALLOCATOR

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "format.h"
#include "util.h"

#include "../kernel/kinfo.h"

#define PMM_BITMAP_ADDRESS 0xC0800000
#define PMM_BITMAP_SIZE    131072

typedef struct {
    // pointer to the start of usable RAM
    uint8_t *memory_base;

    // pointer to a bitmap of all of the free pages in RAM. Each 1 represents a
    // used page and each 0 represents an unused page.
    uint8_t *free_pages;

    // memory base size in bytes (how large the memory base is)
    size_t mb_size;
} PhysicalMemoryManager;

extern PhysicalMemoryManager pmm;

// initializes the physical memory manager and finds the largest space of
// available RAM and allocates a 4096 byte aligned offset as the start of
// physically addressable memory
void pmm_init(multiboot_info_t*);

// allocates the memory needed for the free page bitmap
void pmm_allocate_bitmaps();

// reads the cr3 paging register into a u32
uint32_t pmm_read_cr3();

#endif
