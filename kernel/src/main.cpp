#include "print.h"
#include "gdt.h"
#include "memory.h"
#include "idt.h"
#include "paging.h"
#include <stdint.h>
#include "shell.h"
#include "disk.h"

extern "C" uint32_t grub_magic;
extern "C" uint32_t grub_info;
uint8_t sector[512];
extern "C" void kmain() {
    printf("=== XuanJi OS ===\n");

    gdt_init();
    gdt_load();
    pmm_init(grub_info);
    paging_init();
    idt_init();
    idt_load();
    ata_read_sector(0, sector);


    __asm__ volatile ("sti");
    shell_init();

    while (1) {}
}