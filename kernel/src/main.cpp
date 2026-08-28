#include "print.h"
#include "gdt.h"
#include "memory.h"
#include "idt.h"
#include "paging.h"
#include <stdint.h>
#include "shell.h"

extern "C" uint32_t grub_magic;
extern "C" uint32_t grub_info;

extern "C" void kmain() {
    printf("=== XuanJi OS ===\n");

    gdt_init();
    gdt_load();

    pmm_init(grub_info);

    idt_init();
    idt_load();

    printf("Testing int 33...\n");
    __asm__ volatile ("int $33");
    printf("int 33 returned.\n");

    __asm__ volatile ("sti");

    shell_init();

    while (1) {}
}