#include "print.h"

// ============ 基础输出函数 ============

int cursor = 0;
void print_char(char c) {
    char* video_memory = (char*)0xB8000;
    if (c == '\n') {
        cursor = (cursor / 80 + 1) * 80;
        return;
    }
    video_memory[cursor * 2] = c;
    video_memory[cursor * 2 + 1] = 0x0F;
    cursor++;
}

void print_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i]);
    }
}

void print_int(int num) {
    if (num == 0) {
        print_char('0');
        return;
    }
    if (num < 0) {
        print_char('-');
        num = -num;
    }
    char digits[12];
    int i = 0;
    while (num > 0) {
        digits[i++] = '0' + num % 10;
        num /= 10;
    }
    while (i > 0) {
        print_char(digits[--i]);
    }
}

void print_hex(unsigned int num) {
    const char* hex = "0123456789ABCDEF";
    char digits[8];
    for (int i = 7; i >= 0; i--) {
        digits[i] = hex[num & 0xF];
        num >>= 4;
    }
    print_string("0x");
    for (int i = 0; i < 8; i++) {
        print_char(digits[i]);
    }
}

// ============ printf ============

void printf(const char* format, ...) {
    uint32_t* arg = (uint32_t*)&format + 1;

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int val = (int)*arg++;
                    print_int(val);
                    break;
                }
                case 'x': {
                    unsigned int val = *arg++;
                    print_hex(val);
                    break;
                }
                case 's': {
                    const char* str = (const char*)*arg++;
                    print_string(str);
                    break;
                }
                case 'c': {
                    char c = (char)*arg++;
                    print_char(c);
                    break;
                }
                default:
                    print_char('%');
                    print_char(format[i]);
                    break;
            }
        } else {
            print_char(format[i]);
        }
    }
}