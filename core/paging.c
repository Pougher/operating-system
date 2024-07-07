#include "paging.h"

PagetableMetadata pt_meta;

void paging_init(PhysicalMemoryManager *pmm, Pagetable *master) {
    // find the physical address of where to put the pagetable data
    uint8_t *pagetable_physical = pmm->memory_base + pmm->mb_size -
        (sizeof(Pagetable) * PAGE_TABLE_COUNT);

    pt_meta.physical_address = pagetable_physical;

    // populate the master pagetable with the physical addresses that are going
    // to be mapped virtually
    for (size_t i = 0; i < PAGE_TABLE_COUNT; i++) {
        Page current_page = { 0 };

        current_page.frame = (uint32_t)pagetable_physical >> 12;
        current_page.present = PAGE_PRESENT;
        current_page.rw = PAGE_READWRITE;

        master->pages[i] = current_page;

        pagetable_physical += sizeof(Pagetable);
    }

    // get cr3 which holds the location of the page directory and write the
    // constructed pagetable to its address + the virtual offset of the kernel
    // + 769 * 4 (0xC0400000 virtual offset)
    union {
        uint32_t value;
        uint32_t *ptr;
    } cr3_cast;

    cr3_cast.value = pmm_read_cr3() + KERNEL_VIRTUAL_OFFSET;
    uint32_t *cr3 = cr3_cast.ptr;

    pt_meta.cr3 = cr3;
    cr3[769] = ((uint32_t)(master) -  KERNEL_VIRTUAL_OFFSET) | 0x03;

    // write all zeros to the master pagetable
    volatile uint8_t *mpt = (uint8_t*)PT_VIRTUAL_OFFSET;
    for (size_t i = 0; i < 0x40000; i++) {
        mpt[i] = 0;
    }
}

Pagetable *paging_get_table(uint16_t index) {
    return (Pagetable*) (PT_VIRTUAL_OFFSET + (index << 12));
}

void paging_vmap(uint32_t virtual_address, Pagetable *table) {
    uint32_t cr3_offset = virtual_address >> 22;
    uint8_t *pagetable_physical = (uint8_t*)
        ((uint32_t)pt_meta.physical_address +
         ((uint32_t)table - PT_VIRTUAL_OFFSET));

    // write the pagetable into the correct cr3 index
    pt_meta.cr3[cr3_offset] = ((uint32_t)(pagetable_physical) | 0x03);
}

void paging_invalidate_tlb(void *page) {
    asm volatile (
        "invlpg (%0)" :
        : "r"(page) : "memory"
    );
}
