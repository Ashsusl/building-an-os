#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#pragma pack(push, 1)

namespace drivers::file::F
{
    enum class FatType
    {
        FAT12,
        FAT16,
        FAT32,
        Unknown,
    };

    const unsigned int sector_size = 512;
    // Constants for better readability
    const unsigned int BAD_CLUSTER_MARKER = 0xFF7;
    const unsigned int END_OF_FILE_MARKER = 0xFF8;
    const unsigned int FILENAME_LENGTH = 8;
    const unsigned int EXTENSION_LENGTH = 3;
    const unsigned int offset = 6400;

    // Define a directory entry structure
    struct DirectoryEntry
    {
        char filename[8];
        char extension[3];
        uint8_t attributes;
        uint32_t file_size;
        uint16_t first_cluster;
    };

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

        // this will be cast to its specific type once the driver actually knows what type of FAT this is.
        union
        {
            unsigned char extended_section[54];
            fat_extBS_32_t fat32_section;
        };

    } __attribute__((packed)) fat_BS_t;

    void createFile(const char *filename, uint8_t attributes, uint32_t fileSize);
    void writeFile(const char *filename, const char *data, unsigned int dataSize);
    void readFile(const char *filename, char *buffer, unsigned int bufferSize);
    void deleteFile(const char *filename);
    void splitFilename(const char *filename, char *name, char *extension);
    bool findDirectoryEntry(const char *filename, DirectoryEntry *dirEntry);
    char *strSplit(char *str, char delimiter, int n);
}

#pragma pack(pop)

#endif
