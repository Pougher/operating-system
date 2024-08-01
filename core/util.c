#include "util.h"

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
