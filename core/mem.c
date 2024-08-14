#include "mem.h"

#define LONG_MASK (sizeof(unsigned long long) - 1)

__attribute__((optimize("-fno-tree-loop-distribute-patterns")))
void memset(void *dst, int val, size_t len) {
    unsigned char *ptr = (unsigned char*)dst;
    for (size_t i = 0; i < len; i++) {
        *(ptr + i) = val;
    }
}

void memcpy(void *dst, void *src, size_t len) {
    char *char_dst = (char*)dst;
    char *char_src = (char*)src;

    for (size_t i = 0; i < len; i++) {
        *char_dst = *char_src;

        char_dst++;
        char_src++;
    }
}

size_t strlen(char *string) {
    size_t i = 0;
    while (string[i]) { i++; }
    return i;
}

uint32_t strncmp(char *str1, char *str2, size_t length) {
    while (length && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
        length--;
    }

    if (length == 0) {
        return 0;
    }

    return (*str1 - *str2);
}

uint32_t strcmp(char *str1, char *str2) {
    while(*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}
