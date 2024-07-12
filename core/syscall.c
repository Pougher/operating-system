#include "syscall.h"
#include "sys/sysdef.h"

// the system call table that is indexed into whenever a system call is recieved
uint32_t (*system_call_table[NUM_SYSCALLS])(uint32_t, uint32_t, uint32_t)
    = { 0 };

void syscall_handler(Registers *registers) {
    (void) registers;

    const uint32_t number = *((volatile uint32_t*)SYSCALL_NUMBER);
    const uint32_t operand1 = *((volatile uint32_t*)SYSCALL_OPERAND1);
    const uint32_t operand2 = *((volatile uint32_t*)SYSCALL_OPERAND2);
    const uint32_t operand3 = *((volatile uint32_t*)SYSCALL_OPERAND3);

    // now that the system call has been reassembled, call the correct system
    // call from the table

    // not changed if a system call isnt found, so -1 indicates an error
    *((volatile uint32_t*)SYSCALL_NUMBER) = -1;

    if (number < NUM_SYSCALLS) {
        if (system_call_table[number]) {
            system_call_table[number](operand1, operand2, operand3);
        }
    }
}

void syscall_init() {
    isr_register_handler(SYSCALL_INT_NUMBER, syscall_handler);

    system_call_table[0] = SYSCALL_GET(0);
    system_call_table[1] = SYSCALL_GET(1);
    system_call_table[2] = SYSCALL_GET(2);
}
