#include "idt.h"
#include "isr.h"
#include "print.h"

static IDTEntry idt[256];
IDTPtr idtr;

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr32();   //  定时器
extern "C" void isr33();   //  键盘

static void idt_set_gate(int num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = selector;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

extern "C" void idt_init() {
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E);
    idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E);

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)idt;

    // PIC 初始化
    __asm__ volatile (
        "mov $0x11, %%al\n"
        "out %%al, $0x20\n"
        "out %%al, $0xA0\n"
        "mov $0x20, %%al\n"
        "out %%al, $0x21\n"
        "mov $0x28, %%al\n"
        "out %%al, $0xA1\n"
        "mov $0x04, %%al\n"
        "out %%al, $0x21\n"
        "mov $0x02, %%al\n"
        "out %%al, $0xA1\n"
        "mov $0x01, %%al\n"
        "out %%al, $0x21\n"
        "out %%al, $0xA1\n"
        "mov $0xFC, %%al\n"      // ★ 11111100: 开 IRQ0（定时器）和 IRQ1（键盘）
        "out %%al, $0x21\n"
        "mov $0xFF, %%al\n"
        "out %%al, $0xA1\n"
        ::: "al", "memory"
    );

    //  PIT 初始化（1000Hz）
    __asm__ volatile (
        "mov $0x34, %%al\n"
        "out %%al, $0x43\n"
        "mov $0xA9, %%al\n"      // 低字节 0xA9
        "out %%al, $0x40\n"
        "mov $0x04, %%al\n"      // 高字节 0x04
        "out %%al, $0x40\n"
        ::: "al", "memory"
    );
};
void idt_get_info(uint32_t* base, uint16_t* limit) {
    *base = idtr.base;
    *limit = idtr.limit;
}

extern "C" void idt_load() {
    __asm__ volatile ("lidt %0" : : "m"(idtr) : "memory");
}