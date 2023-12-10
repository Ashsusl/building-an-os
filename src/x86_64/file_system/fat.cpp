#include "file_system/fat.h"
#include "drivers/video/vga.h"
#include "port.h"
#include "stdint.h"
#include "drivers/string/string.h"

namespace STR = drivers::string::STR;
namespace drivers::video::VGA
{

    // Constants for better readability
    const unsigned int BAD_CLUSTER_MARKER = 0xFF7;
    const unsigned int END_OF_FILE_MARKER = 0xFF8;
    const unsigned int FILENAME_LENGTH = 8;
    const unsigned int EXTENSION_LENGTH = 3;

    // Function to detect FAT type and print
    void detectFatTypeAndPrint(const fat_BS_t &bootSector)
    {
        unsigned int totalSectors = (bootSector.total_sectors_16 == 0) ? bootSector.total_sectors_32 : bootSector.total_sectors_16;

        if (totalSectors < 4085)
        {
            print_str("Detected FAT12\n");
        }
        else if (totalSectors < 65525)
        {
            print_str("Detected FAT16\n");
        }
        else
        {
            print_str("Detected FAT32\n");
        }
    }

    // Function to read the boot sector
    void readBootSector(fat_BS_t *bootSector)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Copy the boot sector into the structure
        for (size_t i = 0; i < sizeof(fat_BS_t); i++)
        {
            reinterpret_cast<uint8_t *>(bootSector)[i] = disk[i];
        }
    }

    // Function to read a sector from the disk
    void readSector(unsigned int sectorNumber, unsigned char *buffer, unsigned int sectorSize)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Calculate the address of the sector
        uint8_t *sectorAddress = disk + sectorNumber * sectorSize;

        // Copy the sector into the buffer
        for (size_t i = 0; i < sectorSize; i++)
        {
            buffer[i] = sectorAddress[i];
        }
    }

    // Function to read the boot sector and print its hexadecimal values
    void readBootSectorAndPrint(fat_BS_t *bootSector)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Copy the boot sector into the structure
        for (size_t i = 0; i < sizeof(fat_BS_t); i++)
        {
            reinterpret_cast<uint8_t *>(bootSector)[i] = disk[i];

            // Print the hexadecimal value of each byte
            print_hex(bootSector->extended_section[i]);
            print_str(" "); // Add a space between each byte for better readability
        }

        print_str("\n");
    }

    // Function to read the FAT and follow the cluster chain
    void readFATandFollowChain(unsigned int activeCluster, unsigned int firstFatSector, unsigned int sectorSize)
    {
        unsigned char FATTable[sectorSize * 2];                       // needs two in case we straddle a sector
        unsigned int fatOffset = activeCluster + (activeCluster / 2); // multiply by 1.5
        unsigned int fatSector = firstFatSector + (fatOffset / sectorSize);
        unsigned int entOffset = fatOffset % sectorSize;

        // At this point, you need to read two sectors from disk starting at "fatSector" into "FATTable".
        readSector(fatSector, FATTable, sectorSize * 2);

        unsigned short tableValue = *(unsigned short *)&FATTable[entOffset];

        tableValue = (activeCluster & 1) ? tableValue >> 4 : tableValue & 0xfff;

        // The variable "tableValue" now has the information you need about the next cluster in the chain.
        if (tableValue >= END_OF_FILE_MARKER)
        {
            // There are no more clusters in the chain. This means that the whole file has been read.
            print_str("End of file reached.\n");
        }
        else if (tableValue == BAD_CLUSTER_MARKER)
        {
            // This cluster has been marked as "bad". "Bad" clusters are prone to errors and should be avoided.
            print_str("Bad cluster encountered.\n");
        }
        else
        {
            // "tableValue" is the cluster number of the next cluster in the file.
            print_str("Next cluster in the file: ");
            print_int(tableValue);
            print_str("\n");
        }
    }

    // Function to create a new file
    void createFile(const char *filename, const char *extension, uint8_t attributes, uint32_t fileSize, uint16_t firstCluster)
    {
        // Assuming you have a directory entry array representing the root directory
        // and a file allocation table (FAT) array
        DirectoryEntry *rootDirectory = reinterpret_cast<DirectoryEntry *>(0x100000); // Adjust this based on your memory mapping
        uint16_t *FAT = reinterpret_cast<uint16_t *>(0x110000);                       // Adjust this based on your memory mapping

        // Find an available entry in the root directory
        size_t entryIndex = 0;
        while (entryIndex < 512 && rootDirectory[entryIndex].filename[0] != 0x00 && rootDirectory[entryIndex].filename[0] != 0xE5)
        {
            entryIndex++;
        }

        if (entryIndex < 512)
        {
            // Copy filename, extension, attributes, fileSize, and firstCluster to the directory entry
            STR::strncpy(rootDirectory[entryIndex].filename, filename, FILENAME_LENGTH);
            STR::strncpy(rootDirectory[entryIndex].extension, extension, EXTENSION_LENGTH);
            rootDirectory[entryIndex].attributes = attributes;
            rootDirectory[entryIndex].file_size = fileSize;
            rootDirectory[entryIndex].first_cluster = firstCluster;

            // Update the FAT to mark the cluster chain as used
            size_t cluster = firstCluster;
            while (FAT[cluster] != END_OF_FILE_MARKER)
            {
                size_t nextCluster = FAT[cluster];
                FAT[cluster] = BAD_CLUSTER_MARKER; // Mark as used
                cluster = nextCluster;
            }
            FAT[cluster] = END_OF_FILE_MARKER; // Mark the last cluster in the chain

            VGA::print_str("File created successfully.\n");
        }
        else
        {
            VGA::print_str("No available entry in the root directory.\n");
        }
    }

    // Function to read the content of a file
    void readFile(const char *filename)
    {
        // Assuming you have a directory entry array representing the root directory
        // and a file allocation table (FAT) array
        DirectoryEntry *rootDirectory = reinterpret_cast<DirectoryEntry *>(0x100000); 
        uint16_t *FAT = reinterpret_cast<uint16_t *>(0x110000);                       

        // Find the file in the root directory
        size_t entryIndex = 0;
        while (entryIndex < 512 &&
               STR::strncmp(rootDirectory[entryIndex].filename, filename, FILENAME_LENGTH) != 0)
        {
            entryIndex++;
        }

        if (entryIndex < 512)
        {
            // Read the file content and print it
            size_t cluster = rootDirectory[entryIndex].first_cluster;
            while (cluster != END_OF_FILE_MARKER)
            {
                // Read the content of the cluster and print it (replace this with your actual read operation)
                VGA::print_str("Reading content from cluster ");
                VGA::print_int(cluster);
                VGA::print_str("\n");

                // Move to the next cluster
                cluster = FAT[cluster];
            }
        }
        else
        {
            VGA::print_str("\nFile not found.\n");
        }
    }


} // namespace drivers::video::VGA
