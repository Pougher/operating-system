global syscall, syscall1, syscall2, syscall3

; I am aware that this could be made faster by just using registers to hold the
; system call values, but I dont know how to tell the C compiler to not use eax,
; etc. so I decided for simplicity to write to memory instead despite it being
; slower :(
%define syscall_number 0xC0BFFFF0
%define syscall_op_1   0xC0BFFFF4
%define syscall_op_2   0xC0BFFFF8
%define syscall_op_3   0xC0BFFFFC

syscall:
    ; place the syscall number in eax and write it to the syscall number memory
    ; location (registers cant be used since GCC overwrites them)
    mov eax, [esp + 4]
    mov [syscall_number], eax

    ; interrupt 0x40 is the syscall interrupt
    int 0x40

    ; eax is the return value of the function, so load the returned syscall
    ; value from syscall_number (where it is written by the syscall handler)
    mov eax, [syscall_number]
    ret

syscall1:
    ; place the syscall number in eax
    mov eax, [esp + 4]
    mov [syscall_number], eax

    ; place the 1st operand in eax and write it to the operand address
    mov eax, [esp + 8]
    mov [syscall_op_1], eax

    int 0x40

    mov eax, [syscall_number]
    ret

syscall2:
    ; place the syscall number in eax
    mov eax, [esp + 4]
    mov [syscall_number], eax

    ; place the 1st operand in ecx and write it to the operand address
    mov eax, [esp + 8]
    mov [syscall_op_1], eax

    ; again, write the second operand to its respective memory address
    mov eax, [esp + 12]
    mov [syscall_op_2], eax

    int 0x40

    mov eax, [syscall_number]
    ret

syscall3:
    ; place the syscall number in eax
    mov eax, [esp + 4]
    mov [syscall_number], eax

    ; place the 1st operand in ecx and write it to the operand address
    mov eax, [esp + 8]
    mov [syscall_op_1], eax

    ; again, write the second operand to its respective memory address
    mov eax, [esp + 12]
    mov [syscall_op_2], eax

    ; write the third and final operand to its respective memory address
    mov eax, [esp + 16]
    mov [syscall_op_3], eax

    int 0x40

    mov eax, [syscall_number]
    ret
