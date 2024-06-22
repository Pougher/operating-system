#include "gdt.h"

#define SEGMENT_DESCRIPTOR_COUNT 5

#define SEGMENT_BASE  0x00000
#define SEGMENT_LIMIT 0xFFFFF

#define KERNEL_CODE_ACCESS_BYTE 0x9A
#define KERNEL_DATA_ACCESS_BYTE 0x92
#define USER_CODE_ACCESS_BYTE   0xFA
#define USER_DATA_ACCESS_BYTE   0xF2

/*
 * Flags part of `limit_and_flags`.
 * 1100
 * 0 - Available for system use
 * 0 - Long mode
 * 1 - Size (0 for 16-bit, 1 for 32)
 * 1 - Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
*/
#define SEGMENT_FLAGS_PART 0x0C

static GDTDescriptor gdt_descriptors[SEGMENT_DESCRIPTOR_COUNT];

void gdt_init_descriptor(
    int index,
    unsigned int base_address,
    unsigned int limit,
    unsigned char access_byte,
    unsigned char flags) {
	gdt_descriptors[index].base_low = base_address & 0xFFFF;
	gdt_descriptors[index].base_middle = (base_address >> 16) & 0xFF;
	gdt_descriptors[index].base_high = (base_address >> 24) & 0xFF;

	gdt_descriptors[index].limit_low = limit & 0xFFFF;
	gdt_descriptors[index].limit_and_flags = (limit >> 16) & 0xF;
	gdt_descriptors[index].limit_and_flags |= (flags << 4) & 0xF0;

	gdt_descriptors[index].access_byte = access_byte;
}

void gdt_install_gdt() {
	gdt_descriptors[0].base_low = 0;
	gdt_descriptors[0].base_middle = 0;
	gdt_descriptors[0].base_high = 0;
	gdt_descriptors[0].limit_low = 0;
	gdt_descriptors[0].access_byte = 0;
	gdt_descriptors[0].limit_and_flags = 0;

	// The null descriptor which is never referenced by the processor.
	GDT* gdt_ptr = (GDT*)gdt_descriptors;
	gdt_ptr->address = (unsigned int)gdt_descriptors;
	gdt_ptr->size = (sizeof(GDTDescriptor) * SEGMENT_DESCRIPTOR_COUNT) - 1;

    // the kernel has a privilege level of 0, is executable, and is a code/data
    // segment
	gdt_init_descriptor(
        1,
        SEGMENT_BASE,
        SEGMENT_LIMIT,
        KERNEL_CODE_ACCESS_BYTE,
        SEGMENT_FLAGS_PART
    );

    // kernel data can be read from and written to, cannot be executed and grows
    // downward
	gdt_init_descriptor(
        2,
        SEGMENT_BASE,
        SEGMENT_LIMIT,
        KERNEL_DATA_ACCESS_BYTE,
        SEGMENT_FLAGS_PART
    );

    // user code can be read from and has a privilege level of 3, and is also
    // executable
    gdt_init_descriptor(
        3,
        SEGMENT_BASE,
        SEGMENT_LIMIT,
        USER_CODE_ACCESS_BYTE,
        SEGMENT_FLAGS_PART
    );

    // user data can be read from and written to, has a privilege level of 3 and
    // is not executable. Also grows downward
    gdt_init_descriptor(
        4,
        SEGMENT_BASE,
        SEGMENT_LIMIT,
        USER_DATA_ACCESS_BYTE,
        SEGMENT_FLAGS_PART
    );
	//gdt_init_descriptor(2, SEGMENT_BASE, SEGMENT_LIMIT, SEGMENT_DATA_TYPE, SEGMENT_FLAGS_PART);

	gdt_load_gdt(*gdt_ptr);
	gdt_load_registers();
}
