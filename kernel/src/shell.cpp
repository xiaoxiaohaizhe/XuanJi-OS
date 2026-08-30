#include "print.h"
#include "isr.h"
#include "shell.h"
extern char key_buffer[128];
extern int key_buffer_pos;
extern int cursor;
#define MAX_CMD_LEN 128
#define MAX_HISTORY 100
static char history_data[MAX_HISTORY][MAX_CMD_LEN];
static int history_count = 0;
static int history_next_id = 1;
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
    cursor = 0;
    
    // 重新打印
    printf("=== XuanJi-OS ===\n");
}
void add_to_history(const char* cmd) {
    if (cmd[0] == '\0') return;
    
    // 复制命令到 history_data
    int i = 0;
    while (cmd[i] && i < MAX_CMD_LEN - 1) {
        history_data[history_count][i] = cmd[i];
        i++;
    }
    history_data[history_count][i] = '\0';
    history_count++;
    history_next_id++;
}
void shell_echo() {
    char* p = key_buffer;
    
    // 跳过 "echo"
    while (*p && *p != ' ') p++;
    while (*p == ' ') p++;
    
    if (*p == '\0') {
        printf("\n");
        return;
    }
    
    printf("%s\n", p);
}
void shell_history() {
    int start = history_count - history_count; 
    for (int i = 0; i < history_count; i++) {
        int num = i + 1;
        printf("%d  %s\n", num, history_data[i]);
    }
}
void shell_process_command() {
    int len = key_buffer_pos;
    while (len > 0 && (key_buffer[len - 1] == '\n' || key_buffer[len - 1] == '\r')) {
        key_buffer[len - 1] = '\0';
        len--;
    }
    key_buffer_pos = len;
    if (key_buffer[0] == '\0') {
        // 空命令，不处理
    } 
    else if (my_strncmp(key_buffer, "help", 4) == 0 && key_buffer[4] == '\0') 
    {
        printf("Commands: help, clear, echo <text>, hello,history,version\n");
    } 
    else if (my_strncmp(key_buffer, "clear", 5) == 0 && key_buffer[5] == '\0') 
    {
        shell_clear();
    } 
    else if (my_strncmp(key_buffer, "echo", 4) == 0 && key_buffer[4] == '\0') 
    {
        shell_echo();
    } 
    else if (my_strncmp(key_buffer, "hello", 5) == 0 && key_buffer[5] == '\0') 
    {
        printf("hello can I help you?\n");
    }
    else if (my_strncmp(key_buffer, "520", 3) == 0 && key_buffer[3] == '\0') 
    {
        printf("I love you!\n");
    }
    else if (my_strncmp(key_buffer, "1314", 4) == 0 && key_buffer[4] == '\0') 
    {
        printf("I will love you forever!\n");
    }
    else if (my_strncmp(key_buffer, "version", 7) == 0 && key_buffer[7] == '\0') 
    {
        printf("XuanJi-OS " VERSION "\n");
    }
    else if (my_strncmp(key_buffer, "history", 7)== 0 && key_buffer[7] == '\0') 
    {
        shell_history();
    }
    else 
    {
        printf("Unknown command: ");
        print_string(key_buffer);
        printf("\n");
    }

    //先存历史（缓冲区还没清空）
    if (key_buffer[0] != '\0') 
    {
        add_to_history(key_buffer);
    }

    // 再清空缓冲区
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