#include "memory.h"
#include "print.h"
#include <stdint.h>

// 以下结构体定义来自 GRUB Multiboot 规范
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
} __attribute__((packed));

struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

struct Page {
    Page* next;
} __attribute__((packed));

static Page* free_list = nullptr;
static uint32_t total_pages = 0;
static uint32_t used_pages = 0;

// 从 GRUB 获取物理内存信息
void pmm_init(uint32_t mb_info_addr) {
    multiboot_info* info = (multiboot_info*)mb_info_addr;

    // 检查 GRUB 是否提供了内存映射
    if (!(info->flags & (1 << 6))) {
        printf("PMM: No memory map from GRUB!\n");
        return;
    }

    printf("PMM: Memory map:\n");

    uint32_t mmap_addr = info->mmap_addr;
    uint32_t mmap_end = mmap_addr + info->mmap_length;
    multiboot_mmap_entry* entry = (multiboot_mmap_entry*)mmap_addr;

    while ((uint32_t)entry < mmap_end) {
        uint32_t addr_low = (uint32_t)entry->addr;
        uint32_t addr_high = (uint32_t)(entry->addr >> 32);
        uint32_t len_low = (uint32_t)entry->len;
        uint32_t len_high = (uint32_t)(entry->len >> 32);

        if (entry->type == 1) {  // MULTIBOOT_MEMORY_AVAILABLE
            printf("  Available: 0x%x%08x - 0x%x%08x\n",
                   addr_high, addr_low, addr_high, addr_low + len_low);

            // 忽略 1MB 以下的内存
            if (addr_low < 0x100000) {
                addr_low = 0x100000;
                len_low -= 0x100000;
            }

            // 忽略地址大于 4GB 的部分（32位系统只能访问 4GB）
            if (addr_high > 0) {
                // 32 位系统不支持 4GB 以上内存
                continue;
            }

            uint32_t start = (addr_low + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            uint32_t end = (addr_low + len_low) & ~(PAGE_SIZE - 1);

            for (uint32_t page_addr = start; page_addr < end; page_addr += PAGE_SIZE) {
                Page* page = (Page*)page_addr;
                page->next = free_list;
                free_list = page;
                total_pages++;
            }
        } else {
            printf("  Reserved: 0x%x%08x - 0x%x%08x\n",
                   addr_high, addr_low, addr_high, addr_low + len_low);
        }

        entry = (multiboot_mmap_entry*)((uint32_t)entry + entry->size + 4);
    }

    used_pages = 0;
    printf("PMM: %d pages available\n", total_pages);
}

void* pmm_alloc() {
    if (free_list == nullptr) {
        printf("PMM: out of memory!\n");
        return nullptr;
    }

    Page* page = free_list;
    free_list = page->next;
    used_pages++;
    return (void*)page;
}

void pmm_free(void* addr) {
    if (addr == nullptr) return;
    Page* page = (Page*)addr;
    page->next = free_list;
    free_list = page;
    used_pages--;
}

uint32_t pmm_used_pages() {
    return used_pages;
}