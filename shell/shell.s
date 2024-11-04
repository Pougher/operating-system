section .bss
section .text
                global _start

_start:         mov DWORD [0xC0BFFFF0], 0x04
                int 0x40
                jmp $