#include "fat12.h"
#include "disk.h"
#include "print.h"

static int my_strcmp(const char* a, const char* b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}

struct FAT12BootSector {
    uint8_t  jump[3];
    uint8_t  oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entries;
    uint16_t total_sectors;
    uint8_t  media;
    uint16_t fat_size;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
} __attribute__((packed));

static FAT12BootSector boot_sector;
static uint8_t global_buffer[512];
static uint8_t dir_buffer[512];
void fat12_init() {
    uint8_t buffer[512];
    ata_read_sector(0, buffer);
    boot_sector = *(FAT12BootSector*)buffer;
}

void fat12_list_root() {
    uint32_t root_start = boot_sector.reserved_sectors + 
                          boot_sector.num_fats * boot_sector.fat_size;
    uint32_t root_sectors = (boot_sector.root_entries * 32 + 511) / 512;

    uint8_t buffer[512];
    for (uint32_t s = 0; s < root_sectors; s++) {
        ata_read_sector(root_start + s, buffer);

        for (int j = 0; j < 512; j += 32) {
            uint8_t* entry = buffer + j;
            if (entry[0] == 0x00) return;
            if (entry[0] == 0xE5) continue;
            if (entry[11] & 0x08) continue;

            char name[13];
            int k = 0;
            for (int m = 0; m < 8; m++) {
                if (entry[m] == ' ') break;
                name[k++] = entry[m];
            }
            name[k++] = '.';
            for (int m = 8; m < 11; m++) {
                if (entry[m] == ' ') break;
                name[k++] = entry[m];
            }
            name[k] = '\0';

            for (int n = 0; name[n]; n++) {
                if (name[n] >= 'A' && name[n] <= 'Z') {
                    name[n] = name[n] - 'A' + 'a';
                }
            }

            printf("%s\n", name);
        }
    }
}

void fat12_read_file(const char* filename) {
    uint32_t root_start = boot_sector.reserved_sectors + 
                          boot_sector.num_fats * boot_sector.fat_size;
    uint32_t root_sectors = (boot_sector.root_entries * 32 + 511) / 512;
    uint32_t data_start = root_start + root_sectors;

    uint8_t buffer[512];
    for (uint32_t s = 0; s < root_sectors; s++) {
        ata_read_sector(root_start + s, buffer);

        for (int j = 0; j < 512; j += 32) {
            uint8_t* entry = buffer + j;
            if (entry[0] == 0x00) return;
            if (entry[0] == 0xE5) continue;
            if (entry[11] & 0x08) continue;

            char name[13];
            int k = 0;
            for (int m = 0; m < 8; m++) {
                if (entry[m] == ' ') break;
                name[k++] = entry[m];
            }
            name[k++] = '.';
            for (int m = 8; m < 11; m++) {
                if (entry[m] == ' ') break;
                name[k++] = entry[m];
            }
            name[k] = '\0';

            for (int n = 0; name[n]; n++) {
                if (name[n] >= 'A' && name[n] <= 'Z') {
                    name[n] = name[n] - 'A' + 'a';
                }
            }

            if (my_strcmp(name, filename) == 0) {
                uint16_t cluster = entry[26] | (entry[27] << 8);
                uint32_t size = entry[28] | (entry[29] << 8) | 
                                (entry[30] << 16) | (entry[31] << 24);

                uint32_t file_sector = data_start + (cluster - 2) * boot_sector.sectors_per_cluster;
                bool last_newline = false;

                for (uint32_t i = 0; i < size; i += 512) {
                    ata_read_sector(file_sector + (i / 512), buffer);
                    for (int m = 0; m < 512 && (i + m) < size; m++) {
                        print_char(buffer[m]);
                        if (i + m == size - 1) {
                            last_newline = (buffer[m] == '\n');
                        }
                    }
                }

                if (!last_newline) {
                    printf("\n");
                }
                return;
            }
        }
    }
    printf("File not found: %s\n", filename);
}
// 写入文件（支持覆盖）
int fat12_create_file(const char* filename, const char* data, uint32_t size) {
    uint32_t root_start = boot_sector.reserved_sectors + 
                          boot_sector.num_fats * boot_sector.fat_size;
    uint32_t root_sectors = (boot_sector.root_entries * 32 + 511) / 512;
    uint32_t data_start = root_start + root_sectors;

    int free_entry_sector = -1;
    int free_entry_offset = -1;

    // 1. 找空闲的目录项
    for (uint32_t s = 0; s < root_sectors; s++) {
        ata_read_sector(root_start + s, dir_buffer);
        for (int j = 0; j < 512; j += 32) {
            uint8_t* entry = dir_buffer + j;
            if (entry[0] == 0x00 || entry[0] == 0xE5) {
                free_entry_sector = s;
                free_entry_offset = j;
                break;
            }
        }
        if (free_entry_sector != -1) break;
    }

    if (free_entry_sector == -1) {
        printf("Root directory full!\n");
        return -1;
    }

    // 2. 找空闲簇（这里简化：直接找簇号 2，假设每次都是覆盖第一个空闲簇）
    uint16_t cluster = 2; // 为了简单测试，直接写死 2。如果写多个文件，需遍历 FAT 表。

    // 3. 写入文件内容
    uint32_t sector = data_start + (cluster - 2) * boot_sector.sectors_per_cluster;
    for (uint32_t i = 0; i < size; i += 512) {
        // 清空 global_buffer
        for (int c = 0; c < 512; c++) global_buffer[c] = 0;
        
        for (int m = 0; m < 512 && (i + m) < size; m++) {
            global_buffer[m] = data[i + m];
        }
        ata_write_sector(sector + (i / 512), global_buffer);
    }

    // 4. 更新目录项
    ata_read_sector(root_start + free_entry_sector, dir_buffer);
    int base = free_entry_offset;
    
    for (int i = 0; i < 32; i++) dir_buffer[base + i] = 0;
    for (int i = 0; i < 11; i++) dir_buffer[base + i] = ' ';

    int k = 0;
    int idx = 0;
    while (filename[k] && filename[k] != '.' && idx < 8) {
        char c = filename[k++];
        if (c >= 'a' && c <= 'z') c -= 32;
        dir_buffer[base + idx++] = c;
    }
    if (filename[k] == '.') k++;
    idx = 8;
    while (filename[k] && idx < 11) {
        char c = filename[k++];
        if (c >= 'a' && c <= 'z') c -= 32;
        dir_buffer[base + idx++] = c;
    }
    
    dir_buffer[base + 11] = 0x20; // 属性
    dir_buffer[base + 26] = cluster & 0xFF;
    dir_buffer[base + 27] = (cluster >> 8) & 0xFF;
    dir_buffer[base + 28] = size & 0xFF;
    dir_buffer[base + 29] = (size >> 8) & 0xFF;
    dir_buffer[base + 30] = (size >> 16) & 0xFF;
    dir_buffer[base + 31] = (size >> 24) & 0xFF;

    ata_write_sector(root_start + free_entry_sector, dir_buffer);
    return 0;
}

// 删除文件
int fat12_delete_file(const char* filename) {
    uint32_t root_start = boot_sector.reserved_sectors + 
                          boot_sector.num_fats * boot_sector.fat_size;
    uint32_t root_sectors = (boot_sector.root_entries * 32 + 511) / 512;

    for (uint32_t s = 0; s < root_sectors; s++) {
        ata_read_sector(root_start + s, dir_buffer);
        for (int j = 0; j < 512; j += 32) {
            uint8_t* entry = dir_buffer + j;
            if (entry[0] == 0x00) return -1;
            if (entry[0] == 0xE5) continue;

            char name[13];
            int k = 0;
            for (int m = 0; m < 8; m++) {
                if (entry[m] == ' ') break;
                name[k++] = entry[m];
            }
            if (entry[8] != ' ') {
                name[k++] = '.';
                for (int m = 8; m < 11; m++) {
                    if (entry[m] == ' ') break;
                    name[k++] = entry[m];
                }
            }
            name[k] = '\0';
            for (int n = 0; name[n]; n++) {
                if (name[n] >= 'A' && name[n] <= 'Z') name[n] = name[n] - 'A' + 'a';
            }

            if (my_strcmp(name, filename) == 0) {
                entry[0] = 0xE5; // 标记删除
                ata_write_sector(root_start + s, dir_buffer);
                return 0;
            }
        }
    }
    return -1;
}