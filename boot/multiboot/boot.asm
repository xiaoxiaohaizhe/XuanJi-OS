section .multiboot
align 4
    dd 0x1BADB002
    dd 0x03
    dd -(0x1BADB002 + 0x03)

section .text
global start
extern kmain

; 全局变量，供 C++ 读取
global grub_magic
global grub_info

start:
    ; 先把 GRUB 参数保存到全局变量
    mov [grub_magic], eax
    mov [grub_info], ebx

    mov esp, stack_top
    call kmain
    cli
    hlt

section .data
grub_magic:
    dd 0
grub_info:
    dd 0

section .bss
align 16
stack_bottom:
    resb 16384
stack_top: