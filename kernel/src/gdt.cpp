#include "gdt.h"
#include "print.h"

// GDT 表（3个描述符）
static GDTEntry gdt[3];
static GDTPtr gdtr;

// 设置一个 GDT 描述符
static void gdt_set_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[idx].base_low    = base & 0xFFFF;
    gdt[idx].base_middle = (base >> 16) & 0xFF;
    gdt[idx].base_high   = (base >> 24) & 0xFF;
    
    gdt[idx].limit_low   = limit & 0xFFFF;
    gdt[idx].granularity = (limit >> 16) & 0x0F;
    gdt[idx].granularity |= (gran & 0xF0);
    
    gdt[idx].access = access;
}

// 初始化 GDT
void gdt_init() {
    // 0. 空描述符（必须为0）
    gdt_set_entry(0, 0, 0, 0, 0);
    
    // 1. 内核代码段：基址0，4GB，可读/可执行，DPL=0
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);
    
    // 2. 内核数据段：基址0，4GB，可读写，DPL=0
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);
    
    // 设置 GDTR
    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint32_t)&gdt;
    
    printf("GDT initialized. Base: 0x%x, Limit: %d\n", gdtr.base, gdtr.limit);
}

// 加载 GDT 到 CPU
extern "C" void gdt_load() {
    __asm__ volatile (
        "lgdt %0\n"
        "ljmp $0x08, $1f\n"
        "1:\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        : : "m"(gdtr)
        : "eax", "memory"
    );
    printf("GDT loaded and segments refreshed.\n");
}