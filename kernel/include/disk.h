#ifndef DISK_H
#define DISK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ata_read_sector(uint32_t lba, uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif