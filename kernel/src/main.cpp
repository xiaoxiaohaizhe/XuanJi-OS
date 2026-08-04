#include "print.h"
#include "gdt.h"
#include "memory.h"
#include <stdint.h>

// 声明 boot.asm 里定义的全局变量
extern "C" uint32_t grub_magic;
extern "C" uint32_t grub_info;

extern "C" void kmain() {
    uint32_t magic = grub_magic;
    uint32_t addr = grub_info;

    printf("=== XuanJi OS ===\n");
    printf("GRUB magic: 0x%x, info addr: 0x%x\n", magic, addr);

    if (magic != 0x2BADB002) {
        printf("Invalid GRUB magic! System halted.\n");
        while (1) {}
    }

    gdt_init();
    gdt_load();

    // 用 GRUB 传入的 addr 初始化物理内存管理器
    pmm_init(addr);

    // 测试分配
    void* p1 = pmm_alloc();
    void* p2 = pmm_alloc();
    printf("Allocated: 0x%x, 0x%x\n", (uint32_t)p1, (uint32_t)p2);

    pmm_free(p1);
    printf("Freed: 0x%x\n", (uint32_t)p1);
    printf("Used pages: %d\n", pmm_used_pages());

    printf("System halted.\n");
    while (1) {}
}