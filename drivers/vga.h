#ifndef _DRIVER_VGA_H
#define _DRIVER_VGA_H

#include <stdint.h>
#include <stddef.h>

#include "../font/font8x8.h"

#include "driver_base.h"
#include "vga_palettes.h"

#include "../core/mem.h"

#define VGA_AC			0x3c0
#define	VGA_AC_WRITE    0x3C0
#define VGA_SC_INDEX	0x3c4
#define VGA_SC_DATA		0x3c5
#define VGA_GC_INDEX	0x3ce
#define VGA_GC_DATA		0x3cf
#define VGA_CRTC_INDEX	0x3d4
#define VGA_CRTC_DATA	0x3d5

#define	VGA_MISC_WRITE	0x3C2
#define VGA_SEQ_INDEX   0x3C4
#define VGA_SEQ_DATA	0x3C5
#define	VGA_MISC_READ	0x3CC
#define	VGA_INSTAT_READ	0x3DA

#define VGA_DAC_INDEX   0x3c8
#define VGA_DAC_DATA    0x3c9

#define VGA_STAT0		0x3c2
#define VGA_STAT1		0x3da

#define VGA_SC_REG_MAPMASK	2

#define VGA_GC_REG_SR		0
#define VGA_GC_REG_EN_SR	1
#define VGA_GC_REG_ROT		3
#define VGA_GC_REG_MODE		5
#define VGA_GC_REG_MASK		8

#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21
#define	VGA_NUM_REGS		(1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + \
				VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)

// address that video memory is located at
#define VGA_ADDRESS ((uint8_t*)0xC00A0000)

// NOTE: These control functions are set up in such a way that they are always
// inlined by the compiler

// sets the write mode of the vga card
__attribute__((always_inline))
static inline void driver_vga_set_write_mode(uint32_t mode) {
    port_out(VGA_GC_INDEX, VGA_GC_REG_MODE | (mode << 8));
}

// sets the plane masks of the video mode (used in mode 12h)
__attribute__((always_inline))
static inline void driver_vga_set_plane_mask(uint32_t mask) {
    port_out(VGA_SC_INDEX, VGA_SC_REG_MAPMASK | (mask << 8));
}

// sets the bitmask that the driver is currently writing to
__attribute__((always_inline))
static inline void driver_vga_set_bitmask(uint32_t mask) {
    port_out(VGA_GC_INDEX, VGA_GC_REG_MASK | (mask << 8));
}

// register values to be set during a mode switch
extern uint8_t driver_vga_640x480x16_regs[61];

// palette register to colour register map
extern uint8_t driver_vga_palette_reg_to_colour[16];

// initializes the video card to mode 12h since we are in protected mode and
// can't use raw bios interrupts to do it for us
void driver_vga_init();

// writes the vga registers to select a specific video mode
void driver_vga_write_regs(uint8_t*);

// writes a palette to the VGA card starting at colour index 0
void driver_vga_write_palette(uint32_t*, size_t);

// puts a pixel on the screen at the requested X and Y coordinate
void driver_vga_pixel(uint16_t, uint16_t, uint8_t);

// write 8 bits (8 linear pixels) to the vga buffer at the specified X and Y
// coordinate
void driver_vga_write8(uint16_t, uint16_t, uint8_t, uint8_t);

// clears the entire 640x480 screen to the specified colour
void driver_vga_clear(unsigned char);

// draws a string to the vga output in the colour of the foreground and
// background
void driver_vga_text(const char*, uint16_t, uint16_t, uint8_t, uint8_t);

// draws a uint32_t in hexadecimal format in colour to the vga output
void driver_vga_u32(uint32_t, uint16_t, uint16_t, uint8_t, uint8_t);

#endif
