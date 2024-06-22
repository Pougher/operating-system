#include "mem.h"

#define LONG_MASK (sizeof(unsigned long long) - 1)

__attribute__((optimize("-fno-tree-loop-distribute-patterns")))
void memset(void *dst, int val, size_t len) {
    unsigned char *ptr = (unsigned char*)dst;
    for (size_t i = 0; i < len; i++) {
        *(ptr + i) = val;
    }
}

size_t strlen(char *string) {
    size_t i = 0;
    while (string[i]) { i++; }
    return i;
}
