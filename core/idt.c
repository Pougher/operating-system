#include "idt.h"

// interrupt descriptor table

__attribute__((aligned(0x10)))
static struct {
    InterruptDescriptor entries[256];
    IDTPointer pointer;
} idt;

void idt_set(
    uint8_t index,
    void (*handler)(Registers*),
    uint16_t selector,
    uint8_t type) {
    idt.entries[index] = (InterruptDescriptor) {
        .offset_low = ((uint32_t)handler) & UINT16_MAX,
        .selector = selector,
        .zero = 0,
        .type_attributes = type | 0x60,
        .offset_high = (((uint32_t)handler) >> 16) & UINT16_MAX
    };
}

void idt_init() {
    idt.pointer.limit = sizeof(idt.entries) - 1;
    idt.pointer.base = (uint32_t)&idt.entries[0];

    memset(&idt.entries[0], 0, sizeof(idt.entries));
}

void idt_begin() {
    idt_load((uint32_t)&idt.pointer);
}
