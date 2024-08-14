global stack_relocate

extern vmm_map_memory
extern print_u32

%define STACK_POINTER   0xbffff000 ; where esp will point to
%define STACK_BASE      0xbf800000 ; where the stack will be allocated up from
%define STACK_SIZE      0x00800000 ; the size of the stack (8MiB)

stack_relocate:
    ; first, allocate the new stack with vmm_map_memory
    push STACK_SIZE
    push STACK_BASE
    call vmm_map_memory

    ; since C calls dont consume arguments, add $8 to esp to "pop" both
    ; variables off of the stack
    add esp, 8

    ; we now need to unwind the stack (THIS WILL BREAK IF THINGS CHANGE)
    ; get the return address of the subroutine into edx
    pop edx

    ; return address of kernel_init is ecx
    mov ecx, [ebp + 4]

    ; now we can load the new stack and jump back to kernel_init_stage_2's
    ; return address
    mov esp, STACK_POINTER
    mov ebp, esp
    jmp ecx
