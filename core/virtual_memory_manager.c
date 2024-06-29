#include "virtual_memory_manager.h"

void *vmm_map_memory(uint32_t virtual_address, uint32_t length) {
    Pagetable *table = paging_get_table((uint16_t)(virtual_address >> 22));

    for (uint32_t i = 0; i < length; i += 4096) {
        // physical address of the page that we found in memory
        uint32_t page_physical = (uint32_t)pmm_request_page();

        Page new_page = { 0 };

        new_page.frame = page_physical >> PAGE_SHIFT;
        new_page.present = PAGE_PRESENT;
        new_page.rw = PAGE_READWRITE;

        table->pages[i >> PAGE_SHIFT] = new_page;
    }

    paging_vmap(virtual_address, table);

    return (void*)virtual_address;
}

void vmm_unmap_memory(void *virtual_address, uint32_t length) {
    Pagetable *table = paging_get_table(
        (uint16_t)((uint32_t)virtual_address >> 22)
    );

    for (uint32_t i = 0; i < length; i += 4096) {
        uint32_t page_vaddress = (uint32_t)virtual_address + i;
        paging_invalidate_tlb((void*)page_vaddress);

        pmm_free_page(
            (void*)(table->pages[i >> PAGE_SHIFT].frame << PAGE_SHIFT)
        );
    }
}
