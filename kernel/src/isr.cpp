#include "isr.h"
#include "print.h"

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

extern "C" void isr_handler(Registers regs) {
    if (regs.int_no == 33) {
        // 读取键盘扫描码
        uint8_t scancode;
        __asm__ volatile ("inb $0x60, %0" : "=a"(scancode));
        printf("Key pressed: 0x%x\n", scancode);
        // 发送 EOI
        __asm__ volatile ("mov $0x20, %%al; out %%al, $0x20" ::: "al");
    } else {
        printf("ISR called! Interrupt: %d\n", regs.int_no);
    }
}