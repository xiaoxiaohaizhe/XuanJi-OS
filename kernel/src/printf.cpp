#include "print.h"
// 已有的字符输出函数
void print_char(char c) {
    char* video_memory = (char*) 0xB8000;
    static int cursor = 0;
    if (c == '\n') {
        // 换行处理：跳到下一行
        cursor = (cursor / 80 + 1) * 80;
        return;
    }
    video_memory[cursor * 2] = c;
    video_memory[cursor * 2 + 1] = 0x0F;
    cursor++;
}

// 直接调用 print_string，使函数具备字符串输出能力
void print_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i]);
    }
}

void print_int(int num) { /* 同上 */ }
void print_hex(unsigned int num) { /* 同上 */ }

void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'd': print_int(va_arg(args, int)); break;
                case 'x': print_hex(va_arg(args, unsigned int)); break;
                case 's': print_string(va_arg(args, const char*)); break;
                case 'c': print_char((char)va_arg(args, int)); break;
                default: print_char('%'); print_char(format[i]); break;
            }
        } else {
            print_char(format[i]);
        }
    }
    va_end(args);
}