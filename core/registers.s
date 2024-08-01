global registers_get

get_eip:
    mov dword ecx, [esp]
    ret

get_flags:
    pushfd
    pop ecx
    ret

registers_get:
    push eax
    mov eax, [esp + 8]

    ; write the registers to the struct fields
    mov word [eax + 4], fs
    mov word [eax + 8], es
    mov word [eax + 12], ds
    mov word [eax + 60], cs
    mov word [eax + 72], ss

    mov dword [eax + 16], edi
    mov dword [eax + 20], esi
    mov dword [eax + 24], ebp
    mov dword [eax + 28], esp
    mov dword [eax + 32], ebx
    mov dword [eax + 36], edx
    mov dword [eax + 40], ecx
    pop ecx
    mov dword [eax + 44], ecx

    ; in x86, when a function is called, eip is pushed onto the stack, and the
    ; value of eip is popped off of the stack by the ret instruction, so loading
    ; ecx from the current stack pointer will place the value of eip into ecx
    call get_eip
    mov dword [eax + 56], ecx
    call get_flags
    mov dword [eax + 64], ecx
    mov dword [eax + 68], esp

    ret
