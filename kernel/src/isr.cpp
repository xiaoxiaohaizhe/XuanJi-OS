#include "isr.h"
#include "print.h"
#include  "shell.h"
extern int cursor;
char key_buffer[128];
int key_buffer_pos = 0;

static bool shift_pressed = false;
static bool caps_lock_on = false;

static inline void send_eoi_master() {
    __asm__ volatile ("mov $0x20, %%al; out %%al, $0x20" ::: "al");
}

static inline void send_eoi_slave() {
    __asm__ volatile ("mov $0x20, %%al; out %%al, $0xA0" ::: "al");
}

static const char scancode_ascii[128] = {
    0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,   0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,   0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '-', 0,   0,   0,   '+', 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static const char scancode_ascii_shift[128] = {
    0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,   0,
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '_', 0,   0,   0,   '+', 0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

static char scancode_to_ascii(uint8_t scancode) {
    if (scancode >= 0x80) return 0;
    //小键盘数字键
    if (scancode == 0x4F) return '1';
    if (scancode == 0x50) return '2';
    if (scancode == 0x51) return '3';
    if (scancode == 0x4B) return '4';
    if (scancode == 0x4C) return '5';
    if (scancode == 0x4D) return '6';
    if (scancode == 0x47) return '7';
    if (scancode == 0x48) return '8';
    if (scancode == 0x49) return '9';
    if (scancode == 0x52) return '0';

    //  小键盘特殊键
    if (scancode == 0x4A) return '-';   // 小键盘 -
    if (scancode == 0x4E) return '+';   // 小键盘 +
    if (scancode == 0x53) return '.';   // 小键盘 .
    if (scancode == 0x1C) return '\n';  // 小键盘 Enter
    if (scancode == 0x2A || scancode == 0x36) { shift_pressed = true; return 0; }
    if (scancode == 0xAA || scancode == 0xB6) { shift_pressed = false; return 0; }

    if (scancode == 0x3A) { caps_lock_on = !caps_lock_on; return 0; }
    if (scancode == 0x0E) {
        return '\b';  // 退格字符
    }
    // 新增：回车键（扫描码 0x1C）
    if (scancode == 0x1C) {
        return '\n';
    }

    char c = 0;

    bool is_letter = (scancode >= 0x1E && scancode <= 0x29);

    if (caps_lock_on && is_letter) {
        c = scancode_ascii_shift[scancode];
    } else if (shift_pressed) {
        c = scancode_ascii_shift[scancode];
        if (c == 0) c = scancode_ascii[scancode];
    } else {
        c = scancode_ascii[scancode];
    }

    return c;
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

        char c = scancode_to_ascii(scancode);
        if (c == 0) return;  // 忽略无效字符

        if (c == '\b') {
            // 退格处理
            extern int cursor;
            if (cursor > 0) {
                cursor--;
                print_char(' ');
                cursor--;
            }
            // 从缓冲区移除最后一个字符
            if (key_buffer_pos > 0) {
                key_buffer_pos--;
                key_buffer[key_buffer_pos] = '\0';
            }
            return;
        }

        if (c == '\n') {
            // 回车：处理命令
            print_char(c);
            key_buffer[key_buffer_pos] = '\0';
            shell_process_command();
            return;
        }

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
            (c >= '0' && c <= '9') || c == ' ') {
            print_char(c);
            if (key_buffer_pos < 127) {
                key_buffer[key_buffer_pos++] = c;
                key_buffer[key_buffer_pos] = '\0';
            }
        }
    }
}