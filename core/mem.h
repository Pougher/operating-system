#ifndef _DRIVER_MEM_H
#define _DRIVER_MEM_H

#include <stddef.h>
#include <stdint.h>

// sets the contents of the supplied memory pointer to the value provided
__attribute__((optimize("-fno-tree-loop-distribute-patterns")))
void memset(void*, int, size_t);

// calculates the length of a null-terminated string
size_t strlen(char*);

#endif
