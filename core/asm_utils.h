#ifndef CORE_ASM_UTILS_H
#define CORE_ASM_UTILS_H

#include <stdint.h>

// wrapper for the assmebly instruction "hlt" (halts the processor)
void asm_halt();

// wrapper for the assembly instruction "call" with the operand being the
// 32 bit integer passed to the function
void asm_call(uint32_t);

// returns the stack pointer (stored in esp)
uint32_t asm_get_esp();

// sets the stack pointer (stores the passed argument in esp)
void asm_set_esp(uint32_t);

#endif
