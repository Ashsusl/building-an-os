#ifndef DISK_H
#define DISK_H

#pragma once

#include <stdint.h>
#include "file_system/fat.h"

namespace drivers::file::F
{
    uint16_t findAvailableCluster();
    void writeSector(unsigned int sectorNumber, unsigned char *buffer, unsigned int sectorSize);
    void readBootSector(fat_BS_t *bootSector);
    unsigned int readFATandFollowChain(unsigned int activeCluster, unsigned int firstFatSector, unsigned int sectorSize);
    void readSector(unsigned int sectorNumber, unsigned char *buffer, unsigned int sectorSize);
    void writeCluster(unsigned int clusterNumber, const char *data, unsigned int dataSize);
    void updateDirectoryEntry(const char *filename, const DirectoryEntry *dirEntry);
    void readCluster(unsigned int clusterNumber, char *buffer, unsigned int bufferSize);
    void markClustersAsFree(uint16_t firstCluster);

}

#endif