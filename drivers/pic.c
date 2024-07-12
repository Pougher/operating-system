#include "pic.h"

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

void pic_init() {
    pic_remap_vectors(PIC_REMAPPED_IRQ_MASTER, PIC_REMAPPED_IRQ_SLAVE);

    // disable the PIT interrupt
    pic_irq_set_mask(0x0001);
}
