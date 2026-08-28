#include "print.h"
#include "isr.h"
#include "shell.h"

extern char key_buffer[128];
extern int key_buffer_pos;
extern int cursor;
static int my_strncmp(const char* a, const char* b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return (unsigned char)a[i] - (unsigned char)b[i];
        if (a[i] == '\0') return 0;
    }
    return 0;
}

void shell_clear() {
    char* video = (char*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x0F;
    }
    cursor = 0;  // 直接用顶部的 extern，不用重新声明
}

void shell_echo() {
    // 从 key_buffer 里提取参数（跳过 "echo "）
    char* p = key_buffer;
    
    // 跳过 "echo"
    while (*p == ' ') p++;
    while (*p != ' ' && *p != '\0') p++;
    while (*p == ' ') p++;
    
    if (*p == '\0') {
        printf("\n");
        return;
    }
    
    printf("\n");
    print_string(p);
    printf("\n");
}

void shell_process_command() {
    int len = key_buffer_pos;
    while (len > 0 && (key_buffer[len - 1] == '\n' || key_buffer[len - 1] == '\r')) {
        key_buffer[len - 1] = '\0';
        len--;
    }
    key_buffer_pos = len;

    printf("\n");
    if (key_buffer[0] == '\0') {
        // 空命令，不处理
    } 
    else if (my_strncmp(key_buffer, "help", 4) == 0) 
    {
        printf("Commands: help, clear, echo <text>, hello,version\n");
    } 
    else if (my_strncmp(key_buffer, "clear", 5) == 0) 
    {
        shell_clear();
    } 
    else if (my_strncmp(key_buffer, "echo", 4) == 0) 
    {
        shell_echo();
    } 
    else if (my_strncmp(key_buffer, "hello", 5) == 0) 
    {
        printf("hello can I help you?\n");
    }
    else if (my_strncmp(key_buffer, "version", 7) == 0) 
    {
        printf("XuanJi-OS v1.23.0 (2026-08-28)\n");
    } 
    else 
    {
        printf("Unknown command: ");
        print_string(key_buffer);
        printf("\n");
    }

    // 清空缓冲区，准备下一条命令
    key_buffer[0] = '\0';
    key_buffer_pos = 0;
    
    // 打印提示符（由 shell_init 调用）
    printf("> ");
}
void shell_init() {
    printf("> ");
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}