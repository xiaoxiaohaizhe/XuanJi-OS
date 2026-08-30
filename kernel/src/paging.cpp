#include "paging.h"
#include "memory.h"
#include "print.h"

static uint32_t* page_directory = nullptr;
static uint32_t* page_tables[32];  // 32 个页表

void paging_init() {
    // 分配页目录
    page_directory = (uint32_t*)pmm_alloc();
    if (page_directory == nullptr) {
        printf("Paging: page_directory allocation failed!\n");
        return;
    }

    // 清零页目录
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // 分配 32 个页表（映射 128MB）
    for (int table = 0; table < 32; table++) {
        page_tables[table] = (uint32_t*)pmm_alloc();
        if (page_tables[table] == nullptr) {
            printf("Paging: page_table[%d] allocation failed!\n", table);
            return;
        }
        // 清零页表
        for (int i = 0; i < 1024; i++) {
            page_tables[table][i] = 0;
        }
        // 填页目录项
        page_directory[table] = (uint32_t)page_tables[table] | PAGE_PRESENT | PAGE_WRITE;
    }

    // 映射 0~128MB（身份映射）
    for (uint32_t table = 0; table < 32; table++) {
        for (int i = 0; i < 1024; i++) {
            uint32_t phys = (table * 1024 + i) * PAGE_SIZE;
            page_tables[table][i] = phys | PAGE_PRESENT | PAGE_WRITE;
        }
    }

    // 加载页目录
    __asm__ volatile ("mov %0, %%cr3" : : "r"((uint32_t)page_directory));

    // 开启分页
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}