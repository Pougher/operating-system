#ifndef CORE_REGISTERS_H
#define CORE_REGISTERS_H

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
} Registers;

#endif
