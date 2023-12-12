#pragma once
#include <stddef.h>
#include <stdint.h>

namespace drivers::memory::MEM
{
    void *memCpy(void *dest, const void *src, size_t count);
    void *memSet(void *dest, int value, size_t count);
    int memCmp(const void *ptr1, const void *ptr2, uint16_t num);
}
