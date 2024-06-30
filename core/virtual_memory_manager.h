#ifndef CORE_VIRTUAL_MEMORY_MANAGER_H
#define CORE_VIRTUAL_MEMORY_MANAGER_H

#include "physical_memory_manager.h"
#include "paging.h"

#include <stdint.h>

// when &'d with a virtual address, only the page bits are left behind
#define PAGE_MASK 0x003ff000

// maps a region of physical memory to a virtual address with a specific size
// (the number of bytes that region of memory consists of), and returns the
// virtual address as a pointer
void *vmm_map_memory(page_aligned_ptr, uint32_t);

// marks a region of virtual memory as being unmapped
void vmm_unmap_memory(page_aligned_ptr, uint32_t);

#endif
