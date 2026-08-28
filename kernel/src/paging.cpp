#include "paging.h"
#include "memory.h"
#include "print.h"

static uint32_t* page_directory = nullptr;
static uint32_t* page_table_kernel = nullptr;

void paging_init() {
    page_directory = (uint32_t*)pmm_alloc();
    page_table_kernel = (uint32_t*)pmm_alloc();

    if (page_directory == nullptr || page_table_kernel == nullptr) {
        // 此时 printf 可能还不可用，但保留
        printf("PMM: failed to allocate pages for paging\n");
        return;
    }

    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
        page_table_kernel[i] = 0;
    }

    // 1. 身份映射 1MB ~ 4MB（内核代码区）
    for (uint32_t phys = 0x100000; phys < 0x400000; phys += PAGE_SIZE) {
        uint32_t index = phys / PAGE_SIZE;
        if (index < 1024) {
            page_table_kernel[index] = phys | PAGE_PRESENT | PAGE_WRITE;
        }
    }

    // 2. 映射 VGA 显存（0xB8000 ~ 0xBFFFF）
    //    这样 printf 在分页启用后仍能正常输出
    for (uint32_t phys = 0xB8000; phys < 0xC0000; phys += PAGE_SIZE) {
        uint32_t index = phys / PAGE_SIZE;
        if (index < 1024) {
            page_table_kernel[index] = phys | PAGE_PRESENT | PAGE_WRITE;
        }
    }

    uint32_t phys_page_table = (uint32_t)page_table_kernel;
    page_directory[0] = phys_page_table | PAGE_PRESENT | PAGE_WRITE;

    __asm__ volatile ("mov %0, %%cr3" : : "r"((uint32_t)page_directory));

    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));

    // 分页启用后输出确认信息
    printf("Paging enabled.\n");
}