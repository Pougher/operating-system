#include "virtual_memory_manager.h"

void *vmm_map_memory(page_aligned_ptr virtual_address, uint32_t length) {
    Pagetable *table = paging_get_table((uint16_t)PDIR_INDEX(virtual_address));

    const uint32_t page_offset = ((uint32_t)virtual_address & PAGE_MASK) >> 12;
    const uint32_t page_range = page_offset * PAGE_SIZE + length;

    for (uint32_t i = page_offset * PAGE_SIZE; i < page_range; i += 4096) {
        // physical address of the page that we found in memory
        uint32_t page_physical = (uint32_t)pmm_request_page();

        Page new_page = { 0 };

        new_page.frame = page_physical >> PAGE_SHIFT;
        new_page.present = PAGE_PRESENT;
        new_page.rw = PAGE_READWRITE;

        table->pages[i >> PAGE_SHIFT] = new_page;
    }

    paging_vmap((uint32_t)virtual_address, table);

    return (void*)virtual_address;
}

void vmm_unmap_memory(page_aligned_ptr virtual_address, uint32_t length) {
    Pagetable *table = paging_get_table((uint16_t)PDIR_INDEX(virtual_address));

    for (uint32_t i = 0; i < length; i += 4096) {
        uint32_t page_vaddress = (uint32_t)virtual_address + i;
        paging_invalidate_tlb((void*)page_vaddress);

        pmm_free_page(
            (void*)(table->pages[i >> PAGE_SHIFT].frame << PAGE_SHIFT)
        );
    }
}
