#ifndef ISR_H
#define ISR_H

#include <stdint.h>

struct Registers {
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;  // pusha 压入
    uint32_t ds, es, fs, gs;                            // 段寄存器
    uint32_t int_no, err_code;                          // 手动压入
    uint32_t eip, cs, eflags, user_esp, user_ss;       // CPU 自动压入
};

#ifdef __cplusplus
extern "C" {
#endif

void isr_handler(Registers regs);

#ifdef __cplusplus
}
#endif

#endif