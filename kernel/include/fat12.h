#ifndef FAT12_H
#define FAT12_H

#include <stdint.h>

void fat12_init();
void fat12_list_root();
void fat12_read_file(const char* filename);

#endif