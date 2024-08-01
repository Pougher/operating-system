#include "pic.h"

static void (*const pic_irq_table[])(Registers*) = {
    NULL,
    keyboard_handler
};

void pic_disable() {
    port_outb(PIC_MASTER_DATA_PORT, 0xff);
    port_outb(PIC_SLAVE_DATA_PORT, 0xff);
}

void pic_irq_set_mask(uint16_t mask) {
    port_outb(PIC_MASTER_DATA_PORT, mask & 0xff);
    port_outb(PIC_SLAVE_DATA_PORT, (mask >> 8) & 0xff);
}

void pic_remap_vectors(uint8_t master, uint8_t slave) {
    uint8_t a1 = port_inb(PIC_MASTER_DATA_PORT);
    uint8_t a2 = port_inb(PIC_SLAVE_DATA_PORT);

    port_outb(PIC_MASTER_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);
    driver_io_wait();
    port_outb(PIC_SLAVE_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);
    driver_io_wait();

    port_outb(PIC_MASTER_DATA_PORT, master);
    driver_io_wait();
    port_outb(PIC_SLAVE_DATA_PORT, slave);
    driver_io_wait();

    port_outb(PIC_MASTER_DATA_PORT, 4);
    driver_io_wait();
    port_outb(PIC_SLAVE_DATA_PORT, 2);
    driver_io_wait();

    port_outb(PIC_MASTER_DATA_PORT, ICW4_8086);
    driver_io_wait();
    port_outb(PIC_SLAVE_DATA_PORT, ICW4_8086);
    driver_io_wait();

    port_outb(PIC_MASTER_DATA_PORT, a1);
    port_outb(PIC_SLAVE_DATA_PORT, a2);
}

static uint16_t get_irq_reg(uint8_t ocw3) {
    // ocw3 is sent to the PIC cmd register to get the register values which
    // are then chained together to form a 16 bit value representing the
    // register state of both PICs
    port_outb(PIC_MASTER_COMMAND_PORT, ocw3);
    port_outb(PIC_SLAVE_COMMAND_PORT, ocw3);
    return (
        ((uint16_t)port_inb(PIC_SLAVE_COMMAND_PORT)) << 8) |
        (uint16_t)port_inb(PIC_MASTER_COMMAND_PORT);
}

uint16_t pic_get_isr() {
    return get_irq_reg(PIC_READ_ISR);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        port_outb(PIC_SLAVE_COMMAND_PORT, PIC_EOI);
    }

    port_outb(PIC_MASTER_COMMAND_PORT, PIC_EOI);
}

void pic_irq_handler(Registers *registers) {
    const uint32_t irq = registers->int_no - PIC_REMAPPED_IRQ_MASTER;

    // spurious IRQ check to make sure the IRQ is valid
    if (registers->int_no == 0x27 || registers->int_no == 0x2e) {
        const uint32_t isr = pic_get_isr();

        // if the corresponding isr bit for the interrupt is not set then return
        // from the interrupt handler as the IRQ was not valid
        if (!(isr & (1 << irq))) {
            // if the irq is from the slave then an EOI must be sent to the
            // master since the master doesn't know if the IRQ was real or not
            if (irq >= 8) pic_send_eoi(0);
            return;
        }
    }

    // call the appropriate PIC irq handler
    // NOTE: These functions do not need to send an EOI as this is done for
    // them by the pic irq handler itself
    pic_irq_table[irq](registers);

    pic_send_eoi(irq);
}

void pic_init() {
    pic_remap_vectors(PIC_REMAPPED_IRQ_MASTER, PIC_REMAPPED_IRQ_SLAVE);

    // disable the PIT interrupt
    pic_irq_set_mask(0x0001);
}
