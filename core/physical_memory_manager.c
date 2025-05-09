#include "physical_memory_manager.h"
#include "paging.h"

PhysicalMemoryManager pmm = { 0 };

void pmm_init() {
    pmm_allocate_bitmap();
    pmm_init_bitmap();
}

void pmm_find_usable_memory(multiboot_info_t *mbi) {
    union {
        multiboot_uint64_t in;
        uint8_t *out;
    } conv_union;

    for (uint32_t i = 0; i < mbi->mmap_length;
        i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t *mmt = (multiboot_memory_map_t*)
            (mbi->mmap_addr + i + (uint32_t)KERNEL_VIRTUAL_OFFSET);

        if (mmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
            // the memory is usable
            if (mmt->len > pmm.mb_size && mmt->addr >= 0x100000
                && mmt->addr <= UINT32_MAX) {
                conv_union.in = mmt->addr;
                pmm.memory_base = conv_union.out;

                if (mmt->addr < 0x400000) {
                    // in lower memory (<4MiB, which contains kernel code
                    // among other things) so see if we can use any higher
                    // memory
                    if ((mmt->addr + mmt->len) > 0x400000) {
                        pmm.memory_base = (uint8_t*)0x400000;
                    } else {
                        // the memory region was not large enough
                        pmm.memory_base = 0;
                        continue;
                    }
                }

                pmm.mb_size = mmt->len;
            }
        }
    }

    if (pmm.memory_base == 0) {
        kpanic("FATAL: Failed to locate contiguous RAM segment above "
               "0x00100000 (1MiB)");
    }

    if ((uint32_t)pmm.memory_base % 0x400000 != 0) {
        pmm.mb_size -= 0x400000 - ((uint32_t)pmm.memory_base % 0x400000);
        pmm.memory_base += 0x400000 - ((uint32_t)pmm.memory_base % 0x400000);
    }

    // round the total memory area to the size of a pagetable (4MiB)
    if (pmm.mb_size % 0x400000 != 0) {
        pmm.mb_size -= pmm.mb_size % 0x400000;
    }

    // check if there is enough space for the pagetable data (pagetable data
    // takes up ~4MiB)
    if (pmm.mb_size < 0xF00000) {
        kpanic("FATAL: Not enough space for pagetable data in RAM");
    }
}

void pmm_allocate_bitmap() {
    // work out the bitmap table page index
    uint32_t physical = (pmm.mb_size - (sizeof(Pagetable) * PAGE_TABLE_COUNT
        + 0x400000)) >> 22;
    uint32_t addr = (physical << 22) + (uint32_t)pmm.memory_base;

    Pagetable *bitmap_table = paging_get_table(
        (uint16_t)(PMM_BITMAP_ADDRESS >> 22)
    );

    for (uint32_t i = 0; i < 1024; i++) {
        bitmap_table->pages[i].frame = addr >> PAGE_SHIFT;
        bitmap_table->pages[i].present = PAGE_PRESENT;
        bitmap_table->pages[i].rw = PAGE_READWRITE;

        addr += PAGE_SIZE;

        uint32_t page_vaddress = (uint32_t)PMM_BITMAP_ADDRESS + i * PAGE_SIZE;
        paging_invalidate_tlb((void*)page_vaddress);
    }

    paging_vmap(PMM_BITMAP_ADDRESS, bitmap_table);

    // set the free pages pointer to this new bitmap address
    pmm.free_pages = (uint32_t*) PMM_BITMAP_ADDRESS;
}

void pmm_init_bitmap() {
    // 4GiB / 4096KiB = 1MiB of pages, each page is represented by 1 bit, so
    // each byte is 8 pages. 1MiB / 8 = 131072 bytes, 32 bits per index, so
    // 131072 / 4 = 32768
    memset(pmm.free_pages, 0xff, PMM_BITMAP_SIZE * 4);

    // mark the memory base region as free for use, minus 4 MiB which is
    // allocated for the internal memory bitmap
    pmm.allocated_memory += (pmm.mb_size - (PAGE_SIZE * 1024));
    pmm_mark_region_free(pmm.memory_base, pmm.mb_size - (PAGE_SIZE * 1024));
}

void *pmm_request_page() {
    // first, search through the array looking for an index that isn't all 1s
    // (has a free slot)
    for (uint32_t i = 0; i < PMM_BITMAP_SIZE; i++) {
        if (pmm.free_pages[i] != 0xffffffff) {
            uint32_t zero_index = 0;
            uint32_t value = pmm.free_pages[i];

            // get the index of the zero bit
            for (zero_index = 0; zero_index < 32; zero_index++) {
                if (((value >> zero_index) & 1) == 0) {
                    break;
                }
            }

            pmm.free_pages[i] |= (1 << zero_index);

            uint32_t addr = (zero_index * PAGE_SIZE) + (i * PAGE_SIZE * 32);
            pmm.allocated_memory += PAGE_SIZE;
            return (void*)addr;
        }
    }

    // memory is completely full, so return NULL
    return NULL;
}

void pmm_free_page(page_aligned_ptr page) {
    // the page number
    uint32_t pg = PAGE_INDEX(page);

    // clears the page bit (sets it to 0) to indicate a free page
    pmm.free_pages[pg >> 5] =
        pmm.free_pages[pg >> 5] & ~((uint32_t)1 << (pg % 32));

    pmm.allocated_memory -= PAGE_SIZE;
}

void pmm_mark_region_free(page_aligned_ptr dst, uint32_t length) {
    uint32_t page_number = PAGE_INDEX(dst);
    uint32_t page_end = (((uint32_t)length) >> PAGE_SHIFT) + page_number;

    // the start and end index of the page number in the bitmap
    uint32_t start_index = page_number >> 5;
    uint32_t end_index = page_end >> 5;

    // the start and bit offset into the byte at the start_index and the byte
    // at the end_index
    uint32_t start_bit_offset = page_number % 32;
    uint32_t end_bit_offset = page_end % 32;

    pmm.allocated_memory -= length;

    if (start_index == end_index) {
        pmm.free_pages[start_index] = (UINT32_MAX >> (32 - start_bit_offset)) |
            (UINT32_MAX << end_bit_offset);
        return;
    }

    if (start_bit_offset) {
        pmm.free_pages[start_index] = UINT32_MAX >> (32 - start_bit_offset);
        start_index++;
    }

    if (end_bit_offset) {
        pmm.free_pages[end_index] = 0xffffffff << end_bit_offset;
    }

    for (uint32_t i = start_index; i < end_index; i++) {
        pmm.free_pages[i] = 0x00;
    }
}

bool pmm_fits_in_memory(uint32_t size) {
    return (pmm.allocated_memory + size) < pmm.mb_size;
}

uint32_t pmm_get_total_memory() {
    return pmm.mb_size;
}

uint32_t pmm_read_cr3() {
    uint32_t cr3 = 0;
    asm volatile (
        "mov %%cr3, %%eax\n"
        "mov %%eax, %0\n"
        : "=m"(cr3) :: "%eax"
    );

    return cr3;
}
