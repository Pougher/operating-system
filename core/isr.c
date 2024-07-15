#include "isr.h"

// list of strings that correspond to each of the default ISR exception
// numbers
static char *isr_exceptions[32] = {
    "Divide by zero",
    "Single step interrupt",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Coprocessor not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack segment fault",
    "General protection fault",
    "Page fault",
    "Unrecognized interrupt",
    "x87 FPU Exception",
    "Alignment check",
    "Machine check",
    "SIMD Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Unrecognized interrupt",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED"
};

// list of isr routines
static void (*stubs[NUM_ISRS])(Registers*) = {
    isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
    isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31,
    isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39,
    isr40, isr41, isr42, isr43, isr44, isr45, isr46, isr47,
    isr48, isr49, isr50, isr51, isr52, isr53, isr54, isr55,
    isr56, isr57, isr58, isr59, isr60, isr61, isr62, isr63,
    isr64
};

// list of handlers that are called by the main isr_handler upon an interrupt
// occuring
static void (*handlers[256])(Registers*) = { 0 };

void isr_handler(Registers *regs) {
    if (handlers[regs->int_no]) {
        handlers[regs->int_no](regs);
    }
}

void isr_exception_handler(Registers *regs) {
    kpanic(isr_exceptions[regs->int_no]);
}

void isr_register_handler(
    uint8_t index,
    void (*handler)(Registers*)) {
    handlers[index] = handler;
}

void isr_init() {
    for (uint32_t i = 0; i < NUM_ISRS; i++) {
        idt_set(i, stubs[i], 0x08, 0x8e);
    }

    // register default CPU exceptions
    for (uint32_t i = 0; i < 32; i++) {
        isr_register_handler(i, isr_exception_handler);
    }

    // register PIC IRQs
    for (uint32_t i = 32; i < 48; i++) {
        isr_register_handler(i, pic_irq_handler);
    }
}
