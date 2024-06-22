#ifndef _CORE_PAGING_H
#define _CORE_PAGING_H

#include "physical_memory_manager.h"
#include "format.h"

#include "../kernel/kinfo.h"

#include <stddef.h>
#include <stdint.h>

#define PAGE_PRESENT        1
#define PAGE_READONLY       0
#define PAGE_READWRITE      1
#define PAGE_USER           1
#define PAGE_KERNEL         1
#define PAGE_SIZE_4KB       0
#define PAGE_SIZE_4MB       1
#define PAGE_SIZE           4096

#define PAGE_TABLE_COUNT    1024

typedef struct {
    unsigned int present : 1;
    unsigned int rw      : 1;
    unsigned int user    : 1;
    unsigned int accessed: 1;
    unsigned int dirty   : 1;
    unsigned int unused  : 7;
    unsigned int frame   : 20;
} Page;

typedef struct {
    Page pages[1024] __attribute__((aligned(4096)));
} Pagetable;

// the pagetable pointer passed is responsible for providing memory space for
// all of the other pagetables in RAM
void paging_init(PhysicalMemoryManager*, Pagetable*);

#endif
