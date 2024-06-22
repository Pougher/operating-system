#include "vga.h"

uint8_t driver_vga_640x480x16_regs[61] = {
/* MISC */
	0xE3,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

uint8_t driver_vga_palette_reg_to_colour[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};

void driver_vga_write_regs(uint8_t *regs) {
	unsigned i;

    // write misc. registers
	port_outb(VGA_MISC_WRITE, *regs);
	regs++;

    // write sequencer registers
	for (i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		port_outb(VGA_SEQ_INDEX, i);
		port_outb(VGA_SEQ_DATA, *regs);
		regs++;
	}

    // unlock the CRTC registers
	port_outb(VGA_CRTC_INDEX, 0x03);
	port_outb(VGA_CRTC_DATA, port_inb(VGA_CRTC_DATA) | 0x80);
	port_outb(VGA_CRTC_INDEX, 0x11);
	port_outb(VGA_CRTC_DATA, port_inb(VGA_CRTC_DATA) & ~0x80);

    // make sure that the CRTC registers remain locked
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

    // finally, write to the CRTC registers
	for (i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		port_outb(VGA_CRTC_INDEX, i);
		port_outb(VGA_CRTC_DATA, *regs);
		regs++;
	}

    // write graphics controller registers
	for (i = 0; i < VGA_NUM_GC_REGS; i++) {
		port_outb(VGA_GC_INDEX, i);
		port_outb(VGA_GC_DATA, *regs);
		regs++;
	}

    // write the attribute controller registers
	for (i = 0; i < VGA_NUM_AC_REGS; i++) {
		(void)port_inb(VGA_INSTAT_READ);
		port_outb(VGA_AC, i);
		port_outb(VGA_AC_WRITE, *regs);
		regs++;
	}

    // lock 16-colour palette and unblock display
    (void)port_inb(VGA_INSTAT_READ);
	port_outb(VGA_AC, 0x20);
}

void driver_vga_write_palette(uint32_t *palette, size_t indices) {
    // loop through all of the palette data and write the palette bytes in
    // succession
    for (size_t i = 0; i < indices; i++) {
        // unpack the palette byte into R, G and B components
        const uint32_t value = palette[i];

        uint8_t red = value >> 16;
        uint8_t green = (value >> 8) & 0xff;
        uint8_t blue = value & 0xff;

        // set the appropriate DAC colour reigster address (note that in mode
        // 12h, the palette registers and colour registers are mapped to other
        // values for compatibility reasons)
        port_outb(VGA_DAC_INDEX, driver_vga_palette_reg_to_colour[i]);

        // write R, G and B to the DAC
        port_outb(VGA_DAC_DATA, red);
        port_outb(VGA_DAC_DATA, green);
        port_outb(VGA_DAC_DATA, blue);
    }
}

void driver_vga_init() {
    driver_vga_write_regs(driver_vga_640x480x16_regs);

    // enable write mode 2 which unpacks nibbles into bitplanes
    driver_vga_set_write_mode(0x02);

    // write custom palette data to the DAC
    driver_vga_write_palette(driver_vga_palette_gruvbox, 16);
}

// disable warning on unused variable
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wstringop-overflow"

void driver_vga_pixel(uint16_t x, uint16_t y, uint8_t colour) {
    volatile uint8_t *const dest = VGA_ADDRESS + (y * 80) + (x >> 3);

    // perform a read to load the current byte across all bitplanes into the
    // latches
    // WARNING: If the VGA driver at any point breaks and displays corrupted
    // graphics, it is likely that this load is being optimized away
    volatile unsigned char tmp = *dest;

    driver_vga_set_bitmask(0x80 >> (x & 7));
    *dest = colour;
}

void driver_vga_write8(uint16_t x, uint16_t y, uint8_t bits, uint8_t colour) {
    volatile uint8_t *dest = VGA_ADDRESS + (y * 80) + (x >> 3);

    // separate out the low and high bits that need to be written into video
    // memory so that we can write on pixel values that do not align with a
    // multiple of 8
    const uint8_t low = (bits >> (x % 8));
    const uint8_t high = (bits << (8 - (x % 8)));

    // read loads video latches
    volatile unsigned char tmp = *dest;
    driver_vga_set_bitmask(low);
    *dest = colour;

    dest++;
    // write the second set of bits
    volatile unsigned char temp2 = *dest;
    driver_vga_set_bitmask(high);
    *dest = colour;
}

#pragma GCC diagnostic pop

void driver_vga_clear(unsigned char colour) {
    // enable all bitplanes
    driver_vga_set_plane_mask(0x0f);

    // set the bitmask to 0xff so we write to all of the pixels simultaneously
    driver_vga_set_bitmask(0xff);

    // write 8 pixels at a time, 640/8*480 = 38400 bytes
    memset(VGA_ADDRESS, colour, 38400);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void driver_vga_text(const char *str,
    uint16_t x,
    uint16_t y,
    uint8_t fg,
    uint8_t bg) {
    (void) bg;
    for (size_t i = 0; str[i]; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            const uint8_t bit = font8x8_basic[(uint8_t)str[i]][j];
            driver_vga_write8(i * 8 + x, j + y, bit, fg);
            driver_vga_write8(i * 8 + x, j + y, ~bit, bg);
        }
    }
}
#pragma GCC diagnostic pop

void driver_vga_u32(uint32_t num,
    uint16_t x,
    uint16_t y,
    uint8_t fg,
    uint8_t bg) {
    // buffer to hold the hex string data
    char buffer[11] = { 0 };

    // initialize the buffer to all zeros, then add the leading 'x' character
    // to make it more obvious that the output is a hex string
    memset(buffer, '0', 10);
    buffer[1] = 'x';

    int i = 0, temp;
    while (num != 0) {
        temp = num % 16;
        if (temp < 10) {
            temp = temp + 48;
        } else {
            temp = temp + 55;
        }

        buffer[9 - i] = temp;

        i++;
        num = num / 16;
    }

    driver_vga_text(buffer, x, y, fg, bg);
}
