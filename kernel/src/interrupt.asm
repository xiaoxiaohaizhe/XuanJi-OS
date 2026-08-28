section .text
global isr0, isr1, isr2, isr3,isr33
extern isr_handler
extern idtr
extern grub_magic
extern grub_info 
section .data

; ---------- 宏：定义无错误码的中断入口 ----------
%macro ISR_NOERR 1
isr%1:
    cli
    push byte 0        ; 错误码（无）
    push byte %1       ; 中断号
    jmp isr_common
%endmacro

; ---------- 宏：定义有错误码的中断入口 ----------
%macro ISR_ERR 1
isr%1:
    cli
    push byte %1       ; 错误码（由CPU压入）
    jmp isr_common
%endmacro

; ---------- 用宏生成 0-3 号中断入口 ----------
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 33

; ---------- 通用中断处理 ----------
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
    call isr_handler
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    sti
    iret

