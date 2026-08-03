#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// 段选择子
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10

// GDT 描述符（8字节）
struct GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

// GDTR 结构（6字节）
struct GDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void gdt_init();
extern "C" void gdt_load();

#endif