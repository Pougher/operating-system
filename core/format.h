#ifndef _CORE_FORMAT_H
#define _CORE_FORMAT_H

#include "../drivers/vga.h"

// structure containing information about the state of all format functions,
// for example the current foreground and background colour
typedef struct {
    // the rows and columns present on the current display
    uint16_t total_rows;
    uint16_t total_columns;

    // the foreground and background colours that are currently in use
    uint8_t foreground;
    uint8_t background;

    // the current position of the text cursor in text space
    uint8_t cursor_x;
    uint8_t cursor_y;

    // the current size of the text that is being output
    uint8_t text_size;
} _format_data;

extern _format_data _format_state;

#define _FORMAT_CURSOR_PIXEL_X \
    (_format_state.cursor_x * (_format_state.text_size * 8))
#define _FORMAT_CURSOR_PIXEL_Y \
    (_format_state.cursor_y * (_format_state.text_size * 8))
#define _FORMAT_FOREGROUND (_format_state.foreground)
#define _FORMAT_BACKGROUND (_format_state.background)
#define _FORMAT_OUTPUT_TEXT(buffer) \
    driver_vga_text(buffer, \
        _FORMAT_CURSOR_PIXEL_X, \
        _FORMAT_CURSOR_PIXEL_Y, \
        _FORMAT_FOREGROUND, \
        _FORMAT_BACKGROUND \
    )

// initializes the internal _format_data struct and sets its initial state
void format_init();

// internal function to add an X position to the cursor and compute a rounded
// coordinate position for the cursor
void format_add_cursor_x(uint32_t);

// sets the cursor's x and y coordinates to the values passed to the function
void format_set_cursor_position(uint32_t, uint32_t);

// print formatted string. If a string contains an escape character or
// recognized escape sequence, then it will be output whilst being formatted
// as such (NOTE: does not support scrolling)
void printf(char*);

// prints an unsigned 32 bit integer to the console
void print_u32(uint32_t);

#endif
