#ifndef _CORE_PHYSICAL_MEMORY_ALLOCATOR
#define _CORE_PHYSICAL_MEMORY_ALLOCATOR

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "multiboot.h"
#include "format.h"
#include "util.h"

#include "../kernel/kinfo.h"

#define PMM_BITMAP_ADDRESS 0xC0800000
#define PMM_BITMAP_SIZE    32768

typedef struct {
    // pointer to the start of usable RAM
    uint8_t *memory_base;

    // pointer to a bitmap of all of the free pages in RAM. Each 1 represents a
    // used page and each 0 represents an unused page.
    uint32_t *free_pages;

    // memory base size in bytes (how large the memory base is)
    size_t mb_size;

    // the total currently allocated memory
    uint32_t allocated_memory;
} PhysicalMemoryManager;

extern PhysicalMemoryManager pmm;

// initializes the rest of the pmm struct, must be called after
// pmm_find_usable_memory, and after paging is set up
void pmm_init();

// finds the blocks of memory that can be used as physical memory (aligned to
// 4096 bytes)
void pmm_find_usable_memory(multiboot_info_t*);

// allocates the memory needed for the free page bitmap
void pmm_allocate_bitmap();

// initializes the pmm bitmap to all 0x00000000 (no allocated pages) in regions
// that can be allocated, and in regions that are not usable, 0xffffffff is
// written there
void pmm_init_bitmap();

// marks an entire memory area as free, takes a page-aligned physical pointer
// and a size that when added to the physical pointer produces another page
// aligned physical address
void pmm_mark_region_free(void*, uint32_t);

// finds and returns a free page that exists in physical RAM. If no usable
// addresses are found, then a NULL pointer is returned
void *pmm_request_page();

// marks the physical address of a page in RAM as free
void pmm_free_page(void*);

// tests if a given amount of memory when requested will fit in memory
bool pmm_fits_in_memory(uint32_t);

// returns the total memory size of the system in bytes
uint32_t pmm_get_total_memory();

// reads the cr3 paging register into a u32
uint32_t pmm_read_cr3();

#endif
