#ifndef CORE_SYSCALL_H
#define CORE_SYSCALL_H

#include <stdint.h>

#include "registers.h"
#include "isr.h"

// macro constants that store the memory addresses of each part of a syscall
#define SYSCALL_NUMBER      0xC0BFFFF0
#define SYSCALL_OPERAND1    0xC0BFFFF4
#define SYSCALL_OPERAND2    0xC0BFFFF8
#define SYSCALL_OPERAND3    0xC0BFFFFC

// syscalls are performed upon an INT 0x40
#define SYSCALL_INT_NUMBER  0x40

// the number of currently recognized system calls
#define NUM_SYSCALLS 3

// values that are returned by a system call depending on whether its
// execution was a success or a failure
#define SYSCALL_SUCCESS 0
#define SYSCALL_FAILURE -1

// syscall macros to handle the definition of a system call.
// SYSCALL_DEFINE0 is used to define a system call that takes no arguments.
#define SYSCALL_DEFINE0(num) \
    uint32_t _syscall##num( \
        __attribute__((unused)) uint32_t _stub0, \
        __attribute__((unused)) uint32_t _stub1, \
        __attribute__((unused)) uint32_t _stub2 \
    )

// SYSCALL_DEFINE1 is used to define a system call that takes one argument,
// and each subsequent system call define defines a system call with one
// additional argument
#define SYSCALL_DEFINE1(num, arg1) \
    uint32_t _syscall##num( \
        uint32_t arg1, \
        __attribute__((unused)) uint32_t _stub1, \
        __attribute__((unused)) uint32_t _stub2 \
    )
#define SYSCALL_DEFINE2(num, arg1, arg2) \
    uint32_t _syscall##num( \
        uint32_t arg1, \
        uint32_t arg2, \
        __attribute__((unused)) uint32_t _stub2 \
    )
#define SYSCALL_DEFINE3(num, arg1, arg2, arg3) \
    uint32_t _syscall##num(uint32_t arg1, uint32_t arg2, uint32_t arg3)

// macro to construct the name of a system call function from its number
#define SYSCALL_GET(n) _syscall##n

// adds the specified system call to the system call table
#defien SYSCALL_SET(n) system_call_table[n] = _syscall##n

// generates a system call return statement
#define SYSCALL_RETURN(x) \
    *((volatile uint32_t*)SYSCALL_NUMBER) = (uint32_t)x; \
    return 0;

// macro that is used at the end of syscalls that do not return a value, to
// indicate that the system call completed successfully
#define SYSCALL_EXIT \
    *((volatile uint32_t*)SYSCALL_NUMBER) = SYSCALL_SUCCESS; \
    return 0;

// macro to define a system call prototype
#define SYSCALL_PROTO_DEFINE(x) \
    uint32_t _syscall##x(uint32_t, uint32_t, uint32_t)

// calls a numbered system call that may return a value. If a system call does
// not return a value, the return value of syscall will always be 0. This is the
// same for syscall1, syscall2 and syscall3
uint32_t syscall(uint32_t);

// multiple argument system calls
uint32_t syscall1(uint32_t, uint32_t);
uint32_t syscall2(uint32_t, uint32_t, uint32_t);
uint32_t syscall3(uint32_t, uint32_t, uint32_t, uint32_t);

// the syscall handler is called whenever interrupt 64 (0x40) is triggered, and
// is responsible for calling the syscall handler
void syscall_handler(Registers*);

// initializes the system call interface
void syscall_init();

#endif
