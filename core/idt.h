#ifndef CORE_IDT_H
#define CORE_IDT_H

#include <stdint.h>

#include "registers.h"
#include "mem.h"
#include "util.h"
#include "format.h"

typedef struct {
    // lower half of the interrupt offset
    uint16_t offset_low;

    // the gdt selector of the interrupt descriptor
    uint16_t selector;

    // must be set to 0 for the descriptor to be valid
    uint8_t zero;

    // gate type, DPL and P fields
    uint8_t type_attributes;

    // the higher half of the interrupt offset
    uint16_t offset_high;
} __attribute__((packed)) InterruptDescriptor;

typedef struct {
    // 1 less than the size of the IDT in bytes
    uint16_t limit;

    // the offset of the IDT
    uint32_t base;
} __attribute__((packed)) IDTPointer;

// internally calls lidt to load the IDT
extern void idt_load(uint32_t);

// test
extern void idt_test();

// sets the idt index to a specific handler
void idt_set(uint8_t, void (*handler)(Registers*), uint16_t, uint8_t);

// initializes the interrupt descriptor table (IDT)
void idt_init();

// loads the idt with LIDT and enables interrupts
void idt_begin();

#endif
