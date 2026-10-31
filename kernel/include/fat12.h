#ifndef FAT12_H
#define FAT12_H

#include <stdint.h>

// 初始化 FAT12 文件系统
void fat12_init();

// 列出根目录下的文件
void fat12_list_root();

// 读取文件内容并打印到屏幕
void fat12_read_file(const char* filename);

// 创建/写入文件（覆盖方式）
int fat12_create_file(const char* filename, const char* data, uint32_t size);

// 删除文件
int fat12_delete_file(const char* filename);

#endif