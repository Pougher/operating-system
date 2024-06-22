#include "paging.h"

void paging_init(PhysicalMemoryManager *pmm, Pagetable *master) {
    // find the physical address of where to put the pagetable data
    const uint8_t *pagetable_physical = pmm->memory_base + pmm->mb_size -
        (sizeof(Pagetable) * PAGE_TABLE_COUNT);

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
    // + 784 * 4 (0xC4000000 virtual offset)
    union {
        uint32_t value;
        uint32_t *ptr;
    } cr3_cast;

    cr3_cast.value = pmm_read_cr3() + KERNEL_VIRTUAL_OFFSET;
    uint32_t *cr3 = cr3_cast.ptr;

    cr3[784] = ((uint32_t)(master) -  KERNEL_VIRTUAL_OFFSET) | 0x03;

    // test write to that area
    *((volatile uint8_t*)0xC43FFFFF) = 0xaa;
    print_u32(*(volatile uint8_t*)0xC4000000);
}
