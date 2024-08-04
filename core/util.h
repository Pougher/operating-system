#ifndef _CORE_UTIL_H
#define _CORE_UTIL_H

#include "format.h"
#include "mem.h"
#include "registers.h"

// sets the bit indicated by value v in variable x
#define SET_BIT(x, v) ((x) |= (v))

// clears the bit indicated by the value v in variable x
#define CLEAR_BIT(x, v) ((x) &= (~(v)))

// clears or sets a bit depending on if a value is 0 or 1
#define SET_BIT_CONDITIONAL(x, v, c) \
    ((x) = ((x) | (v)) ^ (v * !(c)))

// converts a non-boolean quanitity to a boolean value
#define TO_BOOL(v) (!!(v))

// locks the entire kernel by disabling interrupts and halting
void klock();

// clears the screen and prints in red a panic message, then locks the kernel
void kpanic(char*);

// tests whether or not a character is within the english the alphabet
int isalpha(char);

#endif
