#include "print.h"
#include "gdt.h"
#include "memory.h"
#include "idt.h"
#include "paging.h"
#include <stdint.h>

extern "C" uint32_t grub_magic;
extern "C" uint32_t grub_info;

extern char key_buffer[128];
extern int key_buffer_pos;

extern "C" void kmain() {
    printf("=== XuanJi OS ===\n");

    gdt_init();
    gdt_load();

    pmm_init(grub_info);

    idt_init();
    idt_load();

    paging_init();

    __asm__ volatile ("sti");

    printf("System ready.\n");

    while (1) {
        if (key_buffer_pos > 0) {
            for (int i = 0; i < key_buffer_pos; i++) {
                print_char(key_buffer[i]);
            }
            key_buffer_pos = 0;
        }
    }
}