#include "virtual_memory_manager.h"

void *vmm_map_memory(page_aligned_ptr virtual_address, uint32_t length) {
    if (length == 0) return NULL;

    // temporary variable that is incremented every time a page boundary is
    // crossed
    char *v_address = (char*)virtual_address;

    Pagetable *table = paging_get_table((uint16_t)PDIR_INDEX(virtual_address));

    const uint32_t page_offset = ((uint32_t)virtual_address & PAGE_MASK) >>
        PAGE_SHIFT;
    const uint32_t page_range = page_offset * PAGE_SIZE + length;

    // the current page index into the current pagetable, resets every time
    // we advance to the next pagetable
    uint32_t page_index = page_offset;

    for (uint32_t i = page_offset * PAGE_SIZE; i < page_range; i += 4096) {
        // check if our allocation goes over a pagetable boundary, if it does
        // then get the next pagetable in memory and start writing to it
        if (i % PAGE_TABLE_SIZE == 0 && i != 0) {
            // firstly, map the current pagetable to that virtual address
            paging_vmap((uint32_t)v_address, table);

            // round v_address to the nearest virtual table address
            v_address = (char*)((((uint32_t)v_address >> 22) + 1) << 22);

            table = paging_get_table(
                (uint16_t)PDIR_INDEX((page_aligned_ptr)v_address)
            );

            page_index = 0;
        }
        // physical address of the page that we found in memory
        uint32_t page_physical = (uint32_t)pmm_request_page();

        if ((void*)page_physical == NULL) {
            // unmap all of the memory allocated by this vmap call since it
            // failed to find all of the required memory
            vmm_unmap_memory(virtual_address, (page_offset * PAGE_SIZE) - i);
            return NULL;
        }

        Page new_page = { 0 };

        new_page.frame = page_physical >> PAGE_SHIFT;
        new_page.present = PAGE_PRESENT;
        new_page.rw = PAGE_READWRITE;

        table->pages[page_index] = new_page;

        page_index++;
    }

    paging_vmap((uint32_t)v_address, table);

    return (void*)virtual_address;
}

void vmm_unmap_memory(page_aligned_ptr virtual_address, uint32_t length) {
    if (length == 0) return;

    char *v_address = (char*)virtual_address;

    // pointer to the pagetable that is currently being worked on, is updated
    // every time a pagetable boundary is crossed (every 4MiB)
    Pagetable *table = paging_get_table((uint16_t)PDIR_INDEX(virtual_address));

    const uint32_t page_offset = ((uint32_t)virtual_address & PAGE_MASK) >> 12;
    const uint32_t page_range = page_offset * PAGE_SIZE + length;

    uint32_t page_index = page_offset;

    for (uint32_t i = page_offset * PAGE_SIZE; i < page_range; i += 4096) {
        if (i % PAGE_TABLE_SIZE == 0 && i != 0) {
            // round v_address to the nearest virtual table address
            v_address = (char*)((((uint32_t)v_address >> 22) + 1) << 22);

            table = paging_get_table(
                (uint16_t)PDIR_INDEX((page_aligned_ptr)v_address)
            );

            page_index = 0;
        }

        uint32_t page_vaddress = (uint32_t)v_address + page_index * PAGE_SIZE;

        // invalidate the TLB entry to force the CPU to flush the prefetch
        // buffer
        paging_invalidate_tlb((void*)page_vaddress);

        // mark the physical address of the page as usable to the physical
        // memory manager
        pmm_free_page(
            (void*)(table->pages[page_index].frame << PAGE_SHIFT)
        );

        page_index++;
    }
}
