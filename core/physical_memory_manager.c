#include "physical_memory_manager.h"

PhysicalMemoryManager pmm;

void pmm_init(multiboot_info_t *mbi) {
    pmm.memory_base = 0;
    pmm.mb_size = 0;

    union {
        multiboot_uint64_t in;
        uint8_t *out;
    } conv_union;

    for (uint32_t i = 0; i < mbi->mmap_length;
        i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t *mmt = (multiboot_memory_map_t*)
            (mbi->mmap_addr + i + KERNEL_VIRTUAL_OFFSET);

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

    // check if there is enough space for the pagetable data (pagetable data
    // takes up ~4MiB)
    if (pmm.mb_size < 0x400000) {
        printf("\x8c""FATAL: Not enough space for pagetable data in RAM");
        klock();
    }

    // we have now successfully got an available segment of memory that is free
    // and can be virtually mapped
    printf("Found usable memory segment at ");
    print_u32((uint32_t)pmm.memory_base);
    printf(" with size ");
    print_u32((uint32_t)pmm.mb_size);
    printf("\n");

    printf("CR3[physical] = ");
    print_u32(pmm_read_cr3());
    printf("\n");
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
