#include "format.h"

_format_data _format_state = { 0 };

void format_init() {
    _format_state.text_size = 1;
    _format_state.foreground = 0xf;
    _format_state.background = 0x0;
    _format_state.total_columns = (640 / 8);
    _format_state.total_rows = (480 / 8);
}

void format_add_cursor_x(uint32_t x) {
    const uint32_t size_y = x / _format_state.total_columns;
    const uint32_t size_x = x % _format_state.total_columns;

    _format_state.cursor_x += size_x;
    _format_state.cursor_y += size_y;
}

void format_set_cursor_position(uint32_t x, uint32_t y) {
    _format_state.cursor_x = x;
    _format_state.cursor_y = y;
}

void putc(char c) {
    char char_string[2] = { c, '\0' };
    _FORMAT_OUTPUT_TEXT(char_string);

    format_add_cursor_x(1);
}

void printf(char *format) {
    const size_t format_length = strlen(format);

    // create a buffer to write into in order to write blocks of text out at
    // a time
    char buffer[format_length + 16];
    memset(buffer, 0, format_length + 16);

    size_t index = 0;
    size_t buffer_pointer = 0;

    // iterate over format looking for escape sequences
    while (format[index]) {
        const unsigned char uc = (unsigned char)format[index];

        if (format[index] == '\n') {
            // escape sequence for newline
            _FORMAT_OUTPUT_TEXT(buffer);
            _format_state.cursor_y++;
            _format_state.cursor_x = 0;

            memset(buffer, 0, buffer_pointer + 1);
            buffer_pointer = -1;
        } else if (uc >= 0x80 && uc <= 0x9f) {
            // changes the current text colour
            // from character value 0x80 to 0x8f, the foreground colour is
            // changed. From character value 0x90 to 0x9f, the background
            // colour is changed
            _FORMAT_OUTPUT_TEXT(buffer);
            format_add_cursor_x(buffer_pointer);

            memset(buffer, 0, buffer_pointer + 1);
            buffer_pointer = -1;
            if (uc < 0x90) {
                // foreground
                _format_state.foreground = format[index] - 0x80;
            } else {
                // background
                _format_state.background = format[index] - 0x90;
            }
        } else {
            buffer[buffer_pointer] = format[index];
        }

        index++;
        buffer_pointer++;

        // check for cursor going over a line
        if ((_format_state.cursor_x + buffer_pointer) == _format_state.total_columns) {
            _FORMAT_OUTPUT_TEXT(buffer);

            memset(buffer, 0, buffer_pointer + 1);
            buffer_pointer = 0;

            _format_state.cursor_x = 0;
            _format_state.cursor_y++;
        }
    }

    if (buffer_pointer != 0) {
        _FORMAT_OUTPUT_TEXT(buffer);
        format_add_cursor_x(buffer_pointer);
    }
}

void print_u32(uint32_t value) {
    if ((_format_state.cursor_x + 10) > _format_state.total_columns) {
        _format_state.cursor_y++;
        _format_state.cursor_x = 0;
    }
    driver_vga_u32(
        value,
        _FORMAT_CURSOR_PIXEL_X,
        _FORMAT_CURSOR_PIXEL_Y,
        _FORMAT_FOREGROUND,
        _FORMAT_BACKGROUND
    );
    _format_state.cursor_x += 10;
}
