%macro isr_no_error 1
isr%+%1:
    global isr%+%1
    push 0
    push %1
    jmp isr_common
%endmacro

%macro isr_error 1
isr%+%1:
    global isr%+%1
    push %1
    jmp isr_common
%endmacro

isr_no_error 0
isr_no_error 1
isr_no_error 2
isr_no_error 3
isr_no_error 4
isr_no_error 5
isr_no_error 6
isr_no_error 7
isr_error    8
isr_no_error 9
isr_error    10
isr_error    11
isr_error    12
isr_error    13
isr_error    14
isr_no_error 15
isr_no_error 16
isr_no_error 17
isr_no_error 18
isr_no_error 19
isr_no_error 20
isr_no_error 21
isr_no_error 22
isr_no_error 23
isr_no_error 24
isr_no_error 25
isr_no_error 26
isr_no_error 27
isr_no_error 28
isr_no_error 29
isr_no_error 30
isr_no_error 31
isr_no_error 32
isr_no_error 33
isr_no_error 34
isr_no_error 35
isr_no_error 36
isr_no_error 37
isr_no_error 38
isr_no_error 39
isr_no_error 40
isr_no_error 41
isr_no_error 42
isr_no_error 43
isr_no_error 44
isr_no_error 45
isr_no_error 46
isr_no_error 47
isr_no_error 48
isr_no_error 49
isr_no_error 50
isr_no_error 51
isr_no_error 52
isr_no_error 53
isr_no_error 54
isr_no_error 55
isr_no_error 56
isr_no_error 57
isr_no_error 58
isr_no_error 59
isr_no_error 60
isr_no_error 61
isr_no_error 62
isr_no_error 63
isr_no_error 64

extern isr_handler

isr_common:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    cld

    push esp
    call isr_handler

    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popa

    add esp, 8
    iret
