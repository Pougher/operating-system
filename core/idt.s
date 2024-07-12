global idt_load

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ; enable interrupts
    sti
    ret

global idt_test
idt_test:
    int 0x00
    ret
