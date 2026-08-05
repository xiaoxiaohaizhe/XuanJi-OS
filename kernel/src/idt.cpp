#include "idt.h"
#include "isr.h"
#include "print.h"

static IDTEntry idt[256];
IDTPtr idtr;


static void idt_set_gate(int num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = selector;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

extern "C" void idt_init() {
    for (int i = 0; i < 256; i++) {
        idt[i].base_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].flags = 0;
        idt[i].base_high = 0;
    }

    extern uint32_t isr0;
    idt_set_gate(0, (uint32_t)&isr0, 0x08, 0x8E);

    extern uint32_t isr33;
    idt_set_gate(33, (uint32_t)&isr33, 0x08, 0x8E);

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)idt;

    // 初始化 PIC，启用键盘中断
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
        "mov $0xFD, %%al\n"
        "out %%al, $0x21\n"
        ::: "al", "memory"
    );

    printf("IDT initialized.\n");
}
void idt_get_info(uint32_t* base, uint16_t* limit) {
    *base = idtr.base;
    *limit = idtr.limit;
}
extern "C" void idt_load() {
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}