#ifndef CORE_ISR_H
#define CORE_ISR_H

#include <stdint.h>

#include "registers.h"
#include "util.h"
#include "idt.h"

#define NUM_ISRS 65

// external definitions for all of the interrupt service routines
extern void isr0(Registers*);
extern void isr1(Registers*);
extern void isr2(Registers*);
extern void isr3(Registers*);
extern void isr4(Registers*);
extern void isr5(Registers*);
extern void isr6(Registers*);
extern void isr7(Registers*);
extern void isr8(Registers*);
extern void isr9(Registers*);
extern void isr10(Registers*);
extern void isr11(Registers*);
extern void isr12(Registers*);
extern void isr13(Registers*);
extern void isr14(Registers*);
extern void isr15(Registers*);
extern void isr16(Registers*);
extern void isr17(Registers*);
extern void isr18(Registers*);
extern void isr19(Registers*);
extern void isr20(Registers*);
extern void isr21(Registers*);
extern void isr22(Registers*);
extern void isr23(Registers*);
extern void isr24(Registers*);
extern void isr25(Registers*);
extern void isr26(Registers*);
extern void isr27(Registers*);
extern void isr28(Registers*);
extern void isr29(Registers*);
extern void isr30(Registers*);
extern void isr31(Registers*);
extern void isr32(Registers*);
extern void isr33(Registers*);
extern void isr34(Registers*);
extern void isr35(Registers*);
extern void isr36(Registers*);
extern void isr37(Registers*);
extern void isr38(Registers*);
extern void isr39(Registers*);
extern void isr40(Registers*);
extern void isr41(Registers*);
extern void isr42(Registers*);
extern void isr43(Registers*);
extern void isr44(Registers*);
extern void isr45(Registers*);
extern void isr46(Registers*);
extern void isr47(Registers*);
extern void isr48(Registers*);
extern void isr49(Registers*);
extern void isr50(Registers*);
extern void isr51(Registers*);
extern void isr52(Registers*);
extern void isr53(Registers*);
extern void isr54(Registers*);
extern void isr55(Registers*);
extern void isr56(Registers*);
extern void isr57(Registers*);
extern void isr58(Registers*);
extern void isr59(Registers*);
extern void isr60(Registers*);
extern void isr61(Registers*);
extern void isr62(Registers*);
extern void isr63(Registers*);
extern void isr64(Registers*);

// handles exceptions thrown by the CPU
void isr_exception_handler(Registers*);

// default isr handler
void isr_handler(Registers*);

// registers a handler which can be called by the isr handler when an interrupt
// is encountered
void isr_register_handler(uint8_t, void (*)(Registers*));

// initializes the isr
void isr_init();

#endif
