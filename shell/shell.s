section .bss

section .text
                global _start

_start:         mov eax, 0xcafebabe
                jmp $
                ret
