#include "isr.h"
#include "print.h"
char key_buffer[128];
int key_buffer_pos = 0;

static inline void send_eoi_master() {
    __asm__ volatile ("mov $0x20, %%al; out %%al, $0x20" ::: "al");
}

static inline void send_eoi_slave() {
    __asm__ volatile ("mov $0x20, %%al; out %%al, $0xA0" ::: "al");
}

static char scancode_to_ascii(uint8_t scancode) {
    if (scancode >= 0x1E && scancode <= 0x29) {
        return 'a' + (scancode - 0x1E);
    }
    if (scancode >= 0x2C && scancode <= 0x39) {
        return '0' + (scancode - 0x2C);
    }
    if (scancode == 0x39) return ' ';
    return 0;
}

extern "C" void isr_handler(Registers regs) {
    if (regs.int_no >= 40) {
        send_eoi_slave();
        send_eoi_master();
    } else if (regs.int_no >= 32) {
        send_eoi_master();
    }

    if (regs.int_no == 33) {
        uint8_t scancode;
        __asm__ volatile ("inb $0x60, %0" : "=a"(scancode));

        if (scancode < 0x80) {
            char c = scancode_to_ascii(scancode);
            if (c != 0 && key_buffer_pos < 127) {
                key_buffer[key_buffer_pos++] = c;
                key_buffer[key_buffer_pos] = '\0';
            }
        }
    }
}