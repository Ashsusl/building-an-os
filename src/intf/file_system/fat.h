#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include "drivers/video/vga.h"
#include <port.h>

#pragma pack(push, 1)

typedef struct DirectoryEntry
{
    uint8_t name[11];     // File name (8.3 format)
    uint8_t attr;         // File attributes
    uint8_t ntRes;        // Reserved byte for Windows NT
    uint8_t crtTimeTenth; // Millisecond stamp at file creation time
    uint16_t crtTime;     // Time file was created
    uint16_t crtDate;     // Date file was created
    uint16_t lstAccDate;  // Last access date
    uint16_t firstClusHI; // High word of this entry's first cluster number
    uint16_t wrtTime;     // Last write time
    uint16_t wrtDate;     // Last write date
    uint16_t firstClusLO; // Low word of this entry's first cluster number
    uint32_t fileSize;    // 32-bit DWORD holding this file's size in bytes
} __attribute__((packed)) DirectoryEntry;

typedef struct fat_extBS_32
{
    // extended fat32 stuff
    unsigned int table_size_32;
    unsigned short extended_flags;
    unsigned short fat_version;
    unsigned int root_cluster;
    unsigned short fat_info;
    unsigned short backup_BS_sector;
    unsigned char reserved_0[12];
    unsigned char drive_number;
    unsigned char reserved_1;
    unsigned char boot_signature;
    unsigned int volume_id;
    unsigned char volume_label[11];
    unsigned char fat_type_label[8];

} __attribute__((packed)) fat_extBS_32_t;

typedef struct fat_extBS_16
{
    // extended fat12 and fat16 stuff
    unsigned char bios_drive_num;
    unsigned char reserved1;
    unsigned char boot_signature;
    unsigned int volume_id;
    unsigned char volume_label[11];
    unsigned char fat_type_label[8];

} __attribute__((packed)) fat_extBS_16_t;

typedef struct fat_BS
{
    unsigned char bootjmp[3];
    unsigned char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char table_count;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned short head_side_count;
    unsigned int hidden_sector_count;
    unsigned int total_sectors_32;

    // this will be cast to it's specific type once the driver actually knows what type of FAT this is.
    union
    {
        unsigned char extended_section[54];
        fat_extBS_32_t fat32_section;
    };

} __attribute__((packed)) fat_BS_t;

#pragma pack(pop)

namespace drivers::video::VGA
{
    enum class FatType
    {
        FAT12,
        FAT16,
        FAT32,
        Unknown,
    };

    const unsigned int sector_size = 512;

    // Function declarations
    void readBootSector(fat_BS_t *boot_sector);
    void detectFatTypeAndPrint(const fat_BS_t &bootSector);
    void readFATandFollowChain(unsigned int active_cluster, unsigned int first_fat_sector, unsigned int sector_size);
    void readSector(unsigned int sector_number, unsigned char *buffer);
    void detectFatFileSystem(fat_BS_t *fat_boot, fat_extBS_32_t *fat_boot_ext_32);
    void readBootSectorAndPrint(fat_BS_t *boot_sector);
}

#endif
