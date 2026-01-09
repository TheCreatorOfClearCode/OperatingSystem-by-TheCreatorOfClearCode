section .multiboot
    align 4
    dd 0x1BADB002
    dd 0x0
    dd -(0x1BADB002+0x0)

section .text
global _start
extern kernel_main

_start:
    cli
    mov esp, stack_top

    push ebx
    push eax

    call kernel_main

.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top: