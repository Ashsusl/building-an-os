#include "file_system/fat.h"
#include "drivers/video/vga.h"
#include <port.h>
#include "stdint.h"

namespace drivers::video::VGA
{
    void detectFatTypeAndPrint(const fat_BS_t &bootSector)
    {
        // Calculate the total size of the file system in sectors
        unsigned int totalSectors = bootSector.total_sectors_16 != 0 ? bootSector.total_sectors_16 : bootSector.total_sectors_32;

        // Check the total sector count fields to determine the format
        if (totalSectors < 4085)
        {
            // FAT12
            VGA::print_str("Detected FAT12\n");
        }
        else if (totalSectors < 65525)
        {
            // FAT16
            VGA::print_str("Detected FAT16\n");
        }
        else
        {
            // FAT32
            VGA::print_str("Detected FAT32\n");
        }
    }

    void readBootSector(fat_BS_t *boot_sector)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Copy the boot sector into the structure
        for (size_t i = 0; i < sizeof(fat_BS_t); i++)
        {
            reinterpret_cast<uint8_t *>(boot_sector)[i] = disk[i];
        }
    }

    void readSector(unsigned int sector_number, unsigned char *buffer)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Calculate the address of the sector
        uint8_t *sector_address = disk + sector_number * sector_size;

        // Copy the sector into the buffer
        for (size_t i = 0; i < sector_size; i++)
        {
            buffer[i] = sector_address[i];
        }
    }

    void readBootSectorAndPrint(fat_BS_t *boot_sector)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Copy the boot sector into the structure
        for (size_t i = 0; i < sizeof(fat_BS_t); i++)
        {
            reinterpret_cast<uint8_t *>(boot_sector)[i] = disk[i];

            // Print the hexadecimal value of each byte
            VGA::print_hex(boot_sector->extended_section[i]);
            VGA::print_str(" "); // Add a space between each byte for better readability
        }

        VGA::print_str("\n");
    }

    void readFATandFollowChain(unsigned int active_cluster, unsigned int first_fat_sector, unsigned int sector_size)
    {
        unsigned char FAT_table[sector_size * 2];                        // needs two in case we straddle a sector
        unsigned int fat_offset = active_cluster + (active_cluster / 2); // multiply by 1.5
        unsigned int fat_sector = first_fat_sector + (fat_offset / sector_size);
        unsigned int ent_offset = fat_offset % sector_size;

        // At this point you need to read two sectors from disk starting at "fat_sector" into "FAT_table".
        readSector(fat_sector, FAT_table);

        unsigned short table_value = *(unsigned short *)&FAT_table[ent_offset];

        table_value = (active_cluster & 1) ? table_value >> 4 : table_value & 0xfff;

        // The variable "table_value" now has the information you need about the next cluster in the chain.
        if (table_value >= 0xFF8)
        {
            // There are no more clusters in the chain. This means that the whole file has been read.
            VGA::print_str("End of file reached.\n");
        }
        else if (table_value == 0xFF7)
        {
            // This cluster has been marked as "bad". "Bad" clusters are prone to errors and should be avoided.
            VGA::print_str("Bad cluster encountered.\n");
        }
        else
        {
            // "table_value" is the cluster number of the next cluster in the file.
            VGA::print_str("Next cluster in the file: ");
            VGA::print_int(table_value);
            VGA::print_str("\n");
        }
    }
}
