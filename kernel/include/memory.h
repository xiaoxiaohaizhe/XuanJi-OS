#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define PAGE_SIZE 4096

// 从 GRUB 读取内存信息初始化物理内存管理器
void pmm_init(uint32_t mb_info_addr);

void* pmm_alloc();
void pmm_free(void* addr);
uint32_t pmm_used_pages();

#endif