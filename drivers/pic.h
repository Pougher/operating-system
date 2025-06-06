#ifndef DRIVER_PIC_H
#define DRIVER_PIC_H

#include <stdint.h>

#include "driver_base.h"
#include "keyboard.h"

#include "../core/registers.h"
#include "../core/format.h"

// Driver for the 8259 PIC, will not work if the PIC is not present in a
// computer system (no clue why that wouldn't be the case) - will either cause
// some weird undefined behaviour or some sort of fault

#define PIC_MASTER_COMMAND_PORT 0x20
#define PIC_MASTER_DATA_PORT    0x21
#define PIC_SLAVE_COMMAND_PORT  0xa0
#define PIC_SLAVE_DATA_PORT     0xa1

// indicates that ICW4 will be present
#define ICW1_ICW4               0x01
// cascade mode
#define ICW1_SINGLE             0x02
// call address interval 4
#define ICW1_INTERVAL4          0x04
// level triggered mode
#define ICW1_LEVEL              0x08
// initialization (required)
#define ICW1_INIT               0x10

// 8086 mode
#define ICW4_8086               0x01
// normal EOI
#define ICW4_AUTO               0x02
// buffered mode/slave
#define ICW4_BUF_SLAVE          0x08
// buffered mode/master
#define ICW4_BUF_MASTER         0x0C
// special fully nested
#define ICW4_SFNM               0x10

// remapped interrupt numbers for the PIC
#define PIC_REMAPPED_IRQ_MASTER 0x20
#define PIC_REMAPPED_IRQ_SLAVE  0x28

// OCW3 commands to read PIC registers
#define PIC_READ_ISR            0x0b
#define PIC_READ_IRR            0x0a

// when EOI is sent to the PIC, it informs the PIC that an interrupt has been
// acknowledged and has ended, allowing the PIC to send further IRQs
#define PIC_EOI                 0x20

// disables the PIC
void pic_disable();

// sets the IMR across both the slave and master PICs, represented by a single
// 16 bit integer, with the lower 8 bits representing the master and the upper
// 8 bits representing the slave
void pic_irq_set_mask(uint16_t);

// remaps the vector numbers used by the PIC
void pic_remap_vectors(uint8_t, uint8_t);

// returns the ISR (in service register) which can be used as a mask to see
// what IRQs are currently being raised
uint16_t pic_get_isr();

// sends an EOI (end of interrupt) to the PIC
void pic_send_eoi(uint8_t);

// interrupt handler for PIC irqs (interrupts 32-48)
void pic_irq_handler(Registers*);

// initializes the PIC
void pic_init();

#endif
