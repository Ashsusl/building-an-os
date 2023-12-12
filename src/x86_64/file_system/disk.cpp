#include "file_system/fat.h"
#include "file_system/disk.h"
#include "drivers/video/screen.h"
#include "drivers/string/string.h"
#include "drivers/memory/mem.h"
#include "port.h"
#include "stdint.h"

namespace VGA = drivers::video::SCR;
namespace STR = drivers::string::STR;
namespace MEM = drivers::memory::MEM;
namespace drivers::file::F
{
    uint16_t findAvailableCluster()
    {
        fat_BS_t bootSector;
        readBootSector(&bootSector);

        // Calculate the first FAT sector
        unsigned int firstFatSector = bootSector.reserved_sector_count;

        // Read the FAT into a buffer
        unsigned char buffer[bootSector.bytes_per_sector];
        readSector(firstFatSector, buffer, bootSector.bytes_per_sector);

        // Scan the FAT for an available entry (marked with a 0)
        for (int i = 0; i < bootSector.bytes_per_sector; i += 2)
        {
            if (*(uint16_t *)(buffer + i) == 0)
            {
                // Mark the cluster as used (with an end-of-file marker)
                *(uint16_t *)(buffer + i) = END_OF_FILE_MARKER;

                // Write the FAT back to disk
                writeSector(firstFatSector, buffer, bootSector.bytes_per_sector);

                // Return the cluster number
                return i / 2;
            }
        }

        // No available clusters found
        return 0;
    }

    void writeSector(unsigned int sectorNumber, unsigned char *buffer, unsigned int sectorSize)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Calculate the address of the sector
        uint8_t *sectorAddress = disk + sectorNumber * sectorSize;

        // Copy the buffer into the sector
        MEM::memCpy(sectorAddress, buffer, sectorSize);
    }

    void readBootSector(fat_BS_t *bootSector)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Copy the boot sector into the structure
        MEM::memCpy(bootSector, disk, sizeof(fat_BS_t));
    }

    unsigned int readFATandFollowChain(unsigned int activeCluster, unsigned int firstFatSector, unsigned int sectorSize)
    {
        unsigned char FATTable[sectorSize * 2];                       // needs two in case we straddle a sector
        unsigned int fatOffset = activeCluster + (activeCluster / 2); // multiply by 1.5
        unsigned int fatSector = firstFatSector + (fatOffset / sectorSize);
        unsigned int entOffset = fatOffset % sectorSize;

        // Read two sectors from disk starting at "fatSector" into "FATTable".
        readSector(fatSector, FATTable, sectorSize * 2);

        unsigned short tableValue = *(unsigned short *)&FATTable[entOffset];

        tableValue = (activeCluster & 1) ? tableValue >> 4 : tableValue & 0xfff;

        // The variable "tableValue" now has the information you need about the next cluster in the chain.
        return tableValue; // Return the next cluster number
    }

    void readSector(unsigned int sectorNumber, unsigned char *buffer, unsigned int sectorSize)
    {
        // Assuming that the disk is memory-mapped at address 0x100000
        uint8_t *disk = reinterpret_cast<uint8_t *>(0x100000);

        // Calculate the address of the sector
        uint8_t *sectorAddress = disk + sectorNumber * sectorSize;

        // Copy the sector into the buffer
        MEM::memCpy(buffer, sectorAddress, sectorSize);
    }

    bool findDirectoryEntry(const char *filename, DirectoryEntry *dirEntry)
    {
        // Calculate the root directory sector
        fat_BS_t bootSector;
        readBootSector(&bootSector);
        unsigned int rootDirSectors = ((bootSector.root_entry_count * 32) + (bootSector.bytes_per_sector - 1)) / bootSector.bytes_per_sector;
        unsigned int firstDataSector = bootSector.reserved_sector_count + (bootSector.table_count * bootSector.table_size_16) + rootDirSectors;
        unsigned int firstSectorofRootDir = firstDataSector - rootDirSectors;

        // Read the root directory into a buffer
        unsigned char buffer[bootSector.bytes_per_sector];
        readSector(firstSectorofRootDir, buffer, bootSector.bytes_per_sector);

        // Find the directory entry for the file
        for (int i = 0; i < bootSector.bytes_per_sector; i += 32)
        {
            if (STR::strnCmp(reinterpret_cast<const char *>(buffer + i), filename, FILENAME_LENGTH) == 0)
            {
                // Copy the directory entry into the structure
                MEM::memCpy(dirEntry, buffer + i, sizeof(DirectoryEntry));
                return true;
            }
        }

        // No directory entry found
        return false;
    }

    void writeCluster(unsigned int clusterNumber, const char *data, unsigned int dataSize)
    {
        // Calculate the address of the cluster
        unsigned int clusterAddress = clusterNumber * sector_size;

        // Write the data to the cluster
        writeSector(clusterAddress, reinterpret_cast<unsigned char *>(const_cast<char *>(data)), dataSize);
    }

    void updateDirectoryEntry(const char *filename, const DirectoryEntry *dirEntry)
    {
        // Calculate the root directory sector
        fat_BS_t bootSector;
        readBootSector(&bootSector);
        unsigned int rootDirSectors = ((bootSector.root_entry_count * 32) + (bootSector.bytes_per_sector - 1)) / bootSector.bytes_per_sector;
        unsigned int firstDataSector = bootSector.reserved_sector_count + (bootSector.table_count * bootSector.table_size_16) + rootDirSectors;
        unsigned int firstSectorofRootDir = firstDataSector - rootDirSectors;

        // Read the root directory into a buffer
        unsigned char buffer[bootSector.bytes_per_sector];
        readSector(firstSectorofRootDir, buffer, bootSector.bytes_per_sector);

        // Find the directory entry for the file
        for (int i = 0; i < bootSector.bytes_per_sector; i += 32)
        {
            if (STR::strnCmp(reinterpret_cast<const char *>(buffer + i), filename, FILENAME_LENGTH) == 0)
            {
                // Copy the updated directory entry into the buffer
               MEM::memCpy(buffer + i, dirEntry, sizeof(DirectoryEntry));

                // Write the buffer back to the root directory
                writeSector(firstSectorofRootDir, buffer, bootSector.bytes_per_sector);
                return;
            }
        }
    }

    void readCluster(unsigned int clusterNumber, char *buffer, unsigned int bufferSize)
    {
        // Calculate the address of the cluster
        unsigned int clusterAddress = clusterNumber * sector_size;

        // Read the data from the cluster
        readSector(clusterAddress, reinterpret_cast<unsigned char *>(buffer), bufferSize);
    }

    void markClustersAsFree(uint16_t firstCluster)
    {
        fat_BS_t bootSector;
        readBootSector(&bootSector);

        // Calculate the first FAT sector
        unsigned int firstFatSector = bootSector.reserved_sector_count;

        // Read the FAT into a buffer
        unsigned char buffer[bootSector.bytes_per_sector];
        readSector(firstFatSector, buffer, bootSector.bytes_per_sector);

        // Traverse the FAT starting from firstCluster, marking each cluster as free
        uint16_t currentCluster = firstCluster;
        while (currentCluster < END_OF_FILE_MARKER)
        {
            // Calculate the offset of the current cluster in the buffer
            unsigned int offset = currentCluster * 2;

            // Get the next cluster from the FAT
            uint16_t nextCluster = *(uint16_t *)(buffer + offset);

            // Mark the current cluster as free
            *(uint16_t *)(buffer + offset) = 0;

            // Move to the next cluster
            currentCluster = nextCluster;
        }

        // Write the updated FAT back to disk
        writeSector(firstFatSector, buffer, bootSector.bytes_per_sector);
    }

}