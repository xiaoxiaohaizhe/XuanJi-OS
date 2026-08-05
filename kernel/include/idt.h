#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct IDTEntry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern IDTPtr idtr;

#ifdef __cplusplus
extern "C" {
#endif

void idt_init();
void idt_load();
void idt_get_info(uint32_t* base, uint16_t* limit);

#ifdef __cplusplus
}
#endif

#endif