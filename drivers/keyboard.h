#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#include "../core/registers.h"
#include "driver_base.h"

#include "../core/util.h"

// for the sake of simplicity, only a P/S2 Keyboard will be supported for now
#define PS2_DATA    0x60
#define PS2_COMMAND 0x64

#define KEY_NULL        0
#define KEY_ESC         0x1b
#define KEY_BACKSPACE   '\b'
#define KEY_TAB         '\t'
#define KEY_RETURN      '\r'
#define KEY_ENTER       '\n'
#define KEY_DELETE      0x91
#define KEY_HOME        0x92
#define KEY_INSERT      0x90
#define KEY_END         0x93
#define KEY_PAGE_UP     0x94
#define KEY_PAGE_DOWN   0x95
#define KEY_LEFT        0x4b
#define KEY_RIGHT       0x4d
#define KEY_UP          0x48
#define KEY_DOWN        0x50

#define KEY_F1          0x80
#define KEY_F2          (KEY_F1 + 1)
#define KEY_F3          (KEY_F1 + 2)
#define KEY_F4          (KEY_F1 + 3)
#define KEY_F5          (KEY_F1 + 4)
#define KEY_F6          (KEY_F1 + 5)
#define KEY_F7          (KEY_F1 + 6)
#define KEY_F8          (KEY_F1 + 7)
#define KEY_F9          (KEY_F1 + 8)
#define KEY_F10         (KEY_F1 + 9)
#define KEY_F11         (KEY_F1 + 10)
#define KEY_F12         (KEY_F1 + 11)

#define KEY_LCTRL       0x1d
#define KEY_RCTRL       0x1d
#define KEY_RALT        0x38
#define KEY_LALT        0x38
#define KEY_RSHIFT      0x36
#define KEY_LSHIFT      0x2a
#define KEY_NUM_LOCK    0x45
#define KEY_SCROLL_LOCK 0x46
#define KEY_CAPS_LOCK   0x3a

#define KEY_RELEASE     0x80

#define KEY_MOD_SHIFT   0x0001

// function like macro to convert a given value into a keyboard scancode
#define KEYBOARD_SCANCODE(x) ((x) & 0x7f)

// functions to indicate whether or not a given scancode indicates the press or
// release of a key
#define KEY_PRESSED(x) (!((x) & KEY_RELEASE))
#define KEY_RELEASED(x) (!!((x) & KEY_RELEASE))

// struct that contains data indicating the current state of the keyboard, such
// as pressed keys and modifiers
typedef struct {
    uint16_t modifiers;
} KeyboardState;

// called whenever a keyboard event is recieved from the PIC
void keyboard_handler(Registers*);

#endif
