#include "keyboard.h"

static KeyboardState keyboard_state = { 0 };

// translation table for scancodes
static uint8_t keyboard_layout[2][128] = {
    {
        KEY_NULL, KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        '-', '=', KEY_BACKSPACE, KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u',
        'i', 'o', 'p', '[', ']', KEY_ENTER, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
        'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
        ',', '.', '/', 0, 0, 0, ' ', 0, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
        KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 0, 0, KEY_HOME, KEY_UP,
        KEY_PAGE_UP, '-', KEY_LEFT, '5', KEY_RIGHT, '+', KEY_END, KEY_DOWN,
        KEY_PAGE_DOWN, KEY_INSERT, KEY_DELETE, 0, 0, 0, KEY_F11, KEY_F12
    }, {
        KEY_NULL, KEY_ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
        '_', '+', KEY_BACKSPACE, KEY_TAB, 'Q', 'W',   'E', 'R', 'T', 'Y', 'U',
        'I', 'O', 'P',   '{', '}', KEY_ENTER, 0, 'A', 'S', 'D', 'F', 'G', 'H',
        'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N',
        'M', '<', '>', '?', 0, 0, 0, ' ', 0, KEY_F1, KEY_F2, KEY_F3, KEY_F4,
        KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 0, 0, KEY_HOME, KEY_UP,
        KEY_PAGE_UP, '-', KEY_LEFT, '5', KEY_RIGHT, '+', KEY_END, KEY_DOWN,
        KEY_PAGE_DOWN, KEY_INSERT, KEY_DELETE, 0, 0, 0, KEY_F11, KEY_F12
    }
};

static inline char keyboard_scancode_to_char(uint8_t scancode) {
    bool shift = TO_BOOL(keyboard_state.modifiers & KEY_MOD_SHIFT);
    bool capslock = TO_BOOL(keyboard_state.modifiers & KEY_MOD_CAPS);

    char character = keyboard_layout[shift][scancode];

    if (capslock) {
        character = keyboard_layout[(1 & isalpha(character)) | shift][scancode];
    }

    return character;
}

void keyboard_handler(Registers *registers) {
    (void) registers;

    uint16_t scancode = port_inb(0x60);
    uint8_t keyboard_scancode = KEYBOARD_SCANCODE(scancode);

    char key = 0;
    if (keyboard_scancode == KEY_LSHIFT || keyboard_scancode == KEY_RSHIFT) {
        SET_BIT_CONDITIONAL(
            keyboard_state.modifiers,
            KEY_MOD_SHIFT,
            KEY_PRESSED(scancode)
        );
    } else if (keyboard_scancode == KEY_LALT || keyboard_scancode == KEY_RALT) {
        SET_BIT_CONDITIONAL(
            keyboard_state.modifiers,
            KEY_MOD_ALT,
            KEY_PRESSED(scancode)
        );
    } else if (
        keyboard_scancode == KEY_LCTRL || keyboard_scancode == KEY_RCTRL) {
        SET_BIT_CONDITIONAL(
            keyboard_state.modifiers,
            KEY_MOD_CTRL,
            KEY_PRESSED(scancode)
        );
    } else if (keyboard_scancode == KEY_NUM_LOCK) {
        SET_BIT_CONDITIONAL(
            keyboard_state.modifiers,
            KEY_MOD_NLOCK,
            KEY_PRESSED(scancode)
        );
    } else if (keyboard_scancode == KEY_SCROLL_LOCK) {
        SET_BIT_CONDITIONAL(
            keyboard_state.modifiers,
            KEY_MOD_NLOCK,
            KEY_PRESSED(scancode)
        );
    } else if (keyboard_scancode == KEY_CAPS_LOCK) {
        keyboard_state.caps_lock = (keyboard_state.caps_lock + 1) % 4;

        SET_BIT_CONDITIONAL(
            keyboard_state.modifiers,
            KEY_MOD_CAPS,
            keyboard_state.caps_lock == 2
        );
    }
    else if (scancode < 128) {
        key = keyboard_scancode_to_char(keyboard_scancode);
        file_write_char(stdin, key);
        char buffer[2] = { 0 };
        file_read(stdin, buffer, 1);
        printf(buffer);
    }

    // update the key's status in the keys array
    keyboard_state.keys[(scancode & 0x7f)] = KEY_PRESSED(scancode);
}
