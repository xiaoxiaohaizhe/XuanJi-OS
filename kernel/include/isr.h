#ifndef ISR_H
#define ISR_H

#include <stdint.h>

struct Registers {
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t ds, es, fs, gs;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, user_esp, user_ss;
};

#ifdef __cplusplus
extern "C" {
#endif

void isr_handler(Registers regs);

#ifdef __cplusplus
}
#endif

//时间相关
extern uint64_t system_ticks;
uint64_t get_uptime_ms();

#endif