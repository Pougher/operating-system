global asm_halt

asm_halt:
    push ebp
    mov ebp, esp

    hlt

    pop ebp
    ret

global asm_call

asm_call:
    push ebp
    mov ebp, esp

    call [esp + 8]

    pop ebp
    ret

global asm_get_esp

asm_get_esp:
    push ebp
    mov ebp, esp

    mov eax, esp

    pop ebp
    ret

global asm_set_esp

asm_set_esp:
    push ebp
    mov ebp, esp

    mov esp, [esp + 8]

    pop ebp
    ret

