#ifndef _DRIVER_MEM_H
#define _DRIVER_MEM_H

#include <stddef.h>
#include <stdint.h>

// sets the contents of the supplied memory pointer to the value provided
__attribute__((optimize("-fno-tree-loop-distribute-patterns")))
void memset(void*, int, size_t);

// copies len bytes from src to dst
void memcpy(void*, void*, size_t);

// calculates the length of a null-terminated string
size_t strlen(char*);

// compares two strings bounded by a specific number of bytes to check. If the
// strings are equal, then 0 is returned.
uint32_t strncmp(char*, char*, size_t);

// compares two strings, if the strings are equal then 0 is returned
uint32_t strcmp(char*, char*);

#endif
