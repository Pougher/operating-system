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
#define PAGE_SHIFT          12

#define PAGE_TABLE_COUNT    1024
#define PAGE_TABLE_SIZE     4194304

#define PT_VIRTUAL_OFFSET   0xC0400000

// PAGE_INDEX returns the page index of an address
#define PAGE_INDEX(ptr) ((uint32_t)ptr >> PAGE_SHIFT)

// PDIR_INDEX returns the index of an address in the page directory
#define PDIR_INDEX(ptr) ((uint32_t)ptr >> 22)

// type that represents a pointer that lies on a 4096-byte boundary (is page
// aligned). Any pointer that is declared as a page_aligned_ptr must be page
// aligned
typedef void* page_aligned_ptr;

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
    Page pages[1024] __attribute__((aligned(PAGE_SIZE)));
} Pagetable;

typedef struct {
    // the physical address of the master pagetable
    uint8_t *physical_address;

    // the virtual address of the page directory
    uint32_t *cr3;
} PagetableMetadata;

/*
 * Each index into the pagetable RAM represents the virtual address that that
 * pagetable is placed at, for example pagetable index 768 represents virtual
 * address 0xC0000000
*/

// the pagetable pointer passed is responsible for providing memory space for
// all of the other pagetables in RAM
void paging_init(PhysicalMemoryManager*, Pagetable*);

// returns the address in virtual memory of the specified pagetable index
Pagetable *paging_get_table(uint16_t);

// virtually maps a pagetable to a specific virtual address, the page table
// itself must be located in the space allocated for the master pagetable, and
// should be offset by the kernel's virtual address
void paging_vmap(uint32_t, Pagetable*);

// invalidates the TLB entry for the virtual address of a page
void paging_invalidate_tlb(void*);

#endif
