#include "util.h"

void klock() {
    while (1) {
        asm_halt();
    }
}

void kpanic(char *message) {
    format_set_cursor_position(0, 50);
    printf("\x88\x9c\n");
    printf("****************************");
    printf("\nUNRECOVERABLE PANIC OCCURRED\nType: ");
    printf(message);
    printf("\n");

    printf("Register dump:\n");
    Registers regs;
    registers_get(&regs);
    registers_print(&regs);

    klock();
}

int isalpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

uint32_t get_u32(char *pointer) {
    uint32_t value = 0;

    value = pointer[0];
    value |= ((uint32_t)pointer[1]) << 8;
    value |= ((uint32_t)pointer[2]) << 16;
    value |= ((uint32_t)pointer[3]) << 24;

    return value;
}
