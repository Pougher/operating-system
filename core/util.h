#ifndef _CORE_UTIL_H
#define _CORE_UTIL_H

#include "format.h"
#include "mem.h"
#include "registers.h"

// locks the entire kernel by disabling interrupts and halting
void klock();

// clears the screen and prints in red a panic message, then locks the kernel
void kpanic(char*);

#endif
