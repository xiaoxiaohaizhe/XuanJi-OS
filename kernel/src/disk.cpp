#include "disk.h"
#include "print.h"

#define ATA_PRIMARY_DATA     0x1F0
#define ATA_PRIMARY_SECTOR   0x1F2
#define ATA_PRIMARY_LBA_LOW  0x1F3
#define ATA_PRIMARY_LBA_MID  0x1F4
#define ATA_PRIMARY_LBA_HIGH 0x1F5
#define ATA_PRIMARY_CMD      0x1F7

#define ATA_STATUS_BSY  0x80
#define ATA_STATUS_DRQ  0x08

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}
static inline void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void ata_wait() {
    uint8_t status;
    do {
        status = inb(ATA_PRIMARY_CMD);
    } while (status & ATA_STATUS_BSY);
}

extern "C" void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait();

    outb(ATA_PRIMARY_SECTOR, 1);
    outb(ATA_PRIMARY_LBA_LOW, (uint8_t)(lba & 0xFF));
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));
    outb(0x1F6, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    outb(ATA_PRIMARY_CMD, 0x20);

    ata_wait();

    uint16_t* buf16 = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        buf16[i] = inw(ATA_PRIMARY_DATA);
    }
}
extern "C" void ata_write_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait();

    outb(ATA_PRIMARY_SECTOR, 1);
    outb(ATA_PRIMARY_LBA_LOW, (uint8_t)(lba & 0xFF));
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));
    outb(0x1F6, (uint8_t)(0xE0 | ((lba >> 24) & 0x0F)));
    outb(ATA_PRIMARY_CMD, 0x30);

    // ★ 必须等 DRQ 置位
    uint8_t status;
    do {
        status = inb(ATA_PRIMARY_CMD);
    } while ((status & ATA_STATUS_BSY) || !(status & ATA_STATUS_DRQ));

    uint16_t* buf16 = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        outw(ATA_PRIMARY_DATA, buf16[i]);
    }
    
    outb(ATA_PRIMARY_CMD, 0xE7);
}