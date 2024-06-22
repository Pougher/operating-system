#ifndef _CORE_GDT_H
#define _CORE_GDT_H

typedef struct {
    unsigned short size;
    unsigned int address;
} __attribute__((packed)) GDT;

typedef struct {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access_byte;
    unsigned char limit_and_flags;
    unsigned char base_high;
} __attribute__((packed)) GDTDescriptor;

void gdt_init_descriptor(
    int,
    unsigned int,
    unsigned int,
    unsigned char,
    unsigned char
);

void gdt_install_gdt();

void gdt_load_gdt(GDT gdt);
void gdt_load_registers();

#endif
