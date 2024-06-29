#ifndef CORE_VIRTUAL_MEMORY_MANAGER_H
#define CORE_VIRTUAL_MEMORY_MANAGER_H

#include "physical_memory_manager.h"
#include "paging.h"

#include <stdint.h>

// maps a region of physical memory to a virtual address with a specific size
// (the number of bytes that region of memory consists of), and returns the
// virtual address as a pointer
void *vmm_map_memory(uint32_t, uint32_t);

// marks a region of virtual memory as being unmapped
void vmm_unmap_memory(void*, uint32_t);

#endif
