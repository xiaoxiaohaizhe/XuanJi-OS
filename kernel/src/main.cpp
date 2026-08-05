#include "print.h"
#include "gdt.h"
#include "memory.h"
#include "idt.h"
#include <stdint.h>

extern "C" uint32_t grub_magic;
extern "C" uint32_t grub_info;

extern "C" void kmain() {
    printf("=== XuanJi OS ===\n");
    printf("GRUB magic: 0x%x, info addr: 0x%x\n", grub_magic, grub_info);

    gdt_init();
    gdt_load();

    pmm_init(grub_info);

    // 测试物理内存
    void* p1 = pmm_alloc();
    void* p2 = pmm_alloc();
    printf("Allocated: 0x%x, 0x%x\n", (uint32_t)p1, (uint32_t)p2);
    pmm_free(p1);
    printf("Freed: 0x%x\n", (uint32_t)p1);
    printf("Used pages: %d\n", pmm_used_pages());

    // IDT
    idt_init();
    idt_load();

    uint32_t base;
    uint16_t limit;
    idt_get_info(&base, &limit);
    printf("IDT loaded: base=0x%x, limit=%d\n", base, limit);

    // 测试 int 3，看系统是否重启
    printf("Testing int 3...\n");
    __asm__ volatile ("sti");
    printf("Keyboard interrupt enabled. Press any key...\n");
    printf("System halted.\n");
    while (1) {}
}