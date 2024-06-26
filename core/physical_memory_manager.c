#include "physical_memory_manager.h"
#include "paging.h"

PhysicalMemoryManager pmm;

void pmm_init() {
    pmm_allocate_bitmap();
    pmm_init_bitmap();
}

void pmm_find_usable_memory(multiboot_info_t *mbi) {
    pmm.memory_base = 0;
    pmm.mb_size = 0;

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
            if (mmt->len > pmm.mb_size && mmt->addr >= 0x100000) {
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
        printf("\x8c""FATAL: Failed to locate contiguous RAM segment above "
               "0x00100000 (1MiB)\n");
        klock();
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
        printf("\x8c""FATAL: Not enough space for pagetable data in RAM");
        klock();
    }
}

void pmm_allocate_bitmap() {
    // work out the bitmap table page index
    uint32_t index = (pmm.mb_size - (sizeof(Pagetable) * PAGE_TABLE_COUNT
        + 0x400000)) >> 22;
    uint32_t addr = (index << 22) + (uint32_t)pmm.memory_base;

    Pagetable *bitmap_table = paging_get_table((uint16_t)index);
    for (size_t i = 0; i < 1024; i++) {
        bitmap_table->pages[i].frame = addr >> 12;
        bitmap_table->pages[i].present = PAGE_PRESENT;
        bitmap_table->pages[i].rw = PAGE_READWRITE;

        addr += PAGE_SIZE;
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

    const uint32_t page = 4096;
    pmm_mark_region_free((void*)page, page * 65);

    for (size_t i = 0; i < 20; i++) {
        print_u32(pmm.free_pages[i]);
        printf("\n");
    }
}

void *pmm_request_page() {
    // first, search through the array looking for an index that isn't all 1s
    // (has a free slot)
    for (size_t i = 0; i < PMM_BITMAP_SIZE; i++) {
        if (pmm.free_pages[i] != 0xffffffff) {
            uint8_t zero_index = 0;
            uint32_t value = pmm.free_pages[i];

            // get the index of the zero bit
            for (zero_index = 0; zero_index < 32; zero_index++) {
                if (((value >> zero_index) & 1) == 0) {
                    break;
                }
            }

            pmm.free_pages[i] |= (1 << zero_index);
        }
    }

    // TODO
    return NULL;
}

void pmm_mark_region_free(void *dst, size_t length) {
    uint32_t page_number = ((uint32_t)dst) >> 12;
    uint32_t page_end = (((uint32_t)length) >> 12) + page_number;

    // the start and end index of the page number in the bitmap
    uint16_t start_index = page_number / 32;
    uint16_t end_index = page_end / 32;

    // the start and bit offset into the byte at the start_index and the byte
    // at the end_index
    uint8_t start_bit_offset = page_number % 32;
    uint8_t end_bit_offset = page_end % 32;

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

    for (size_t i = start_index; i < end_index; i++) {
        pmm.free_pages[i] = 0x00;
    }
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
