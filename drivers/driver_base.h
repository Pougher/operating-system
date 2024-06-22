#ifndef _KERNEL_DRIVER_BASE_H
#define _KERNEL_DRIVER_BASE_H

#include <stdint.h>
#include <stddef.h>

// writes an 8-bit value to the specified port number
__attribute__((regparm(2), always_inline))
static inline void port_outb(uint16_t port, uint8_t value) {
    asm volatile (
        "outb %0, %1" : : "a"(value), "Nd"(port)
    );
}

// reads an 8-bit value from the specified port number
__attribute__((regparm(1), always_inline))
static inline uint8_t port_inb(uint16_t port) {
    uint8_t ret;
    asm volatile (
        "inb %w1, %b0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory"
    );

    return ret;
}

// writes an 16-bit value to the specified port number
__attribute__((regparm(2), always_inline))
static inline void port_out(uint16_t port, uint16_t value) {
    asm volatile (
        "out %0, %1" : : "a"(value), "Nd"(port)
    );
}

// reads an 16-bit value from the specified port number
__attribute__((regparm(1), always_inline))
static inline uint16_t port_in(uint16_t port) {
    uint16_t ret;
    asm volatile (
        "in %w1, %w0"
        : "=a"(ret)
        : "Nd"(port)
        : "memory"
    );

    return ret;
}

#endif
