#ifndef CORE_REGISTERS_H
#define CORE_REGISTERS_H

#include "format.h"

#include <stdint.h>

typedef struct {
    // segment selectors
    uint32_t ignored, fs, es, ds;

    // general purpose registers
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    // interrupt number and the error number
    uint32_t int_no, error_no;

    // other reigsters
    uint32_t eip, cs, efl, user_esp, ss;
} __attribute__((packed, aligned(4))) Registers;


// returns the values of all the CPU's registers as a struct, ignoring the
// int_no and error_no fields, as well as the CRx registers
void registers_get(Registers*);

// prints the contents of all of the registers as a formatted string
void registers_print(Registers*);

#endif
