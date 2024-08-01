#include "registers.h"

char *registers_names[16] = {
    "fs", "es", "ds",
    "edi", "esi", "ebp", "esp", "ebx", "edx", "ecx", "eax",
    "eip", "cs", "efl", "usp", "ss"
};

void registers_print(Registers *regs) {
    uint32_t *registers = (uint32_t*)regs;
    uint32_t register_base = 0;

    for (int i = 1; i < 19; i++) {
        uint32_t register_value = registers[i];

        if (i != 0 && i != 12 && i != 13) {
            const int end = (register_base % 4 == 0 && register_base != 0);
            if (end) {
                printf("\n");
            }

            printf(registers_names[register_base]);
            printf("=");
            print_u32(register_value);

            if (strlen(registers_names[register_base]) == 2) printf(" ");
            printf(" ");

            register_base++;
        }
    }
}
