#include "file_system/fat.h"
#include "file_system/disk.h"
#include "drivers/video/screen.h"
#include "drivers/string/string.h"
#include "drivers/memory/mem.h"
#include "port.h"
#include "stdint.h"

namespace SCR = drivers::video::SCR;
namespace STR = drivers::string::STR;
namespace MEM = drivers::memory::MEM;
namespace drivers::file::F
{
    void createFile(const char *filename, uint8_t attributes, uint32_t fileSize)
    {
        fat_BS_t bootSector;
        readBootSector(&bootSector);

        char name[FILENAME_LENGTH + 1];
        char extension[EXTENSION_LENGTH + 1];

        // Split the filename into a name and an extension
        splitFilename(filename, name, extension);

        // Calculate the root directory sector
        unsigned int rootDirSectors = ((bootSector.root_entry_count * 32) + (bootSector.bytes_per_sector - 1)) / bootSector.bytes_per_sector;
        unsigned int firstDataSector = bootSector.reserved_sector_count + (bootSector.table_count * bootSector.table_size_16) + rootDirSectors;
        unsigned int firstSectorofRootDir = firstDataSector - rootDirSectors;

        // Read the root directory into a buffer
        unsigned char buffer[bootSector.bytes_per_sector];
        readSector(firstSectorofRootDir, buffer, bootSector.bytes_per_sector);

        // Find an empty directory entry (first byte of filename will be 0x00)
        for (int i = 0; i < bootSector.bytes_per_sector; i += 32)
        {
            if (buffer[i] == 0x00)
            {
                // Find an available cluster
                uint16_t firstCluster = findAvailableCluster();

                // If no available cluster was found, print an error message and return
                if (firstCluster == 0)
                {
                    SCR::print_str("\nNo available clusters found\n");
                    return;
                }

                // Copy the filename and extension
                MEM::memCpy(buffer + i, filename, FILENAME_LENGTH);
                MEM::memCpy(buffer + i + 8, extension, EXTENSION_LENGTH);

                // Set the file attributes
                buffer[i + 11] = attributes;

                // Set the file size
                *(uint32_t *)(buffer + i + 28) = fileSize;

                // Set the first cluster
                *(uint16_t *)(buffer + i + 26) = firstCluster;

                // Write the sector back to disk
                writeSector(firstSectorofRootDir, buffer, bootSector.bytes_per_sector);

                SCR::print_str("\nFile created successfully\n");
                return;
            }
        }

        SCR::print_str("\nNo empty directory entries found\n");
    }

    void writeFile(const char *filename, const char *data, unsigned int dataSize)
    {
        // Find the file's directory entry
        DirectoryEntry dirEntry;
        if (!findDirectoryEntry(filename, &dirEntry))
        {
            SCR::print_str("\nFile not found\n");
            return;
        }

        // Write the data to the file's first cluster
        writeCluster(dirEntry.first_cluster, data, dataSize);

        // Update the file's size in its directory entry
        dirEntry.file_size = dataSize;

        // Write the updated directory entry back to the root directory
        updateDirectoryEntry(filename, &dirEntry);

        SCR::print_str("\nData written successfully\n");
    }

    void readFile(const char *filename, char *buffer, unsigned int bufferSize)
    {
        char name[FILENAME_LENGTH + 1];
        char extension[EXTENSION_LENGTH + 1];

        // Split the filename into a name and an extension
        splitFilename(filename, name, extension);
        // Find the file's directory entry
        DirectoryEntry dirEntry;
        if (!findDirectoryEntry(filename, &dirEntry))
        {
            SCR::print_str("\nFile not found\n");
            return;
        }

        // Read the data from the file's first cluster
        readCluster(dirEntry.first_cluster, buffer, bufferSize);

        SCR::print_str("\nData read successfully\n");
        buffer[dirEntry.file_size] = '\0';
    }

    void deleteFile(const char *filename)
    {
        fat_BS_t bootSector;
        readBootSector(&bootSector);

        // Calculate the root directory sector
        unsigned int rootDirSectors = ((bootSector.root_entry_count * 32) + (bootSector.bytes_per_sector - 1)) / bootSector.bytes_per_sector;
        unsigned int firstDataSector = bootSector.reserved_sector_count + (bootSector.table_count * bootSector.table_size_16) + rootDirSectors;
        unsigned int firstSectorofRootDir = firstDataSector - rootDirSectors;

        // Read the root directory into a buffer
        unsigned char buffer[bootSector.bytes_per_sector];
        char name[FILENAME_LENGTH + 1];
        char extension[EXTENSION_LENGTH + 1];

        // Split the filename into a name and an extension
        splitFilename(filename, name, extension);

        readSector(firstSectorofRootDir, buffer, bootSector.bytes_per_sector);

        // Find the directory entry for the file
        for (int i = 0; i < bootSector.bytes_per_sector; i += 32)
        {
            if (STR::strnCmp(reinterpret_cast<const char *>(buffer + i), filename, FILENAME_LENGTH) == 0)
            {
                // Get the first cluster of the file
                uint16_t firstCluster = *(uint16_t *)(buffer + i + 26);

                // Mark the clusters used by the file as free in the FAT
                markClustersAsFree(firstCluster);

                // Remove the directory entry
                buffer[i] = 0xE5;

                // Write the sector back to disk
                writeSector(firstSectorofRootDir, buffer, bootSector.bytes_per_sector);

                SCR::print_str("\nFile deleted successfully\n");
                return;
            }
        }

        SCR::print_str("\nFile not found\n");
    }

    void splitFilename(const char *filename, char *name, char *extension)
    {
        // Find the last occurrence of '.'
        const char *dot = STR::strChr(filename, '.');

        if (!dot || dot == filename)
        {
            // No extension found, copy the whole filename
            STR::strnCpy(name, filename, FILENAME_LENGTH);
            extension[0] = '\0'; // Empty extension
        }
        else
        {
            // Copy the name and extension
            STR::strnCpy(name, filename, dot - filename);
            STR::strnCpy(extension, dot + 1, EXTENSION_LENGTH);
        }
    }

}
