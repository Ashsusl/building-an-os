#include "drivers/memory/mem.h"

void *memCpy(void *dest, const void *src, size_t count)
{
    char *dest_ptr = static_cast<char *>(dest);
    const char *src_ptr = static_cast<const char *>(src);
    for (size_t i = 0; i < count; ++i)
    {
        dest_ptr[i] = src_ptr[i];
    }
    return dest;
}

void *memSet(void *dest, int value, size_t count)
{
    char *dest_ptr = static_cast<char *>(dest);
    for (size_t i = 0; i < count; ++i)
    {
        dest_ptr[i] = static_cast<char>(value);
    }
    return dest;
}

int memCmp(const void *ptr1, const void *ptr2, uint16_t num)
{
    const uint8_t *u8Ptr1 = (const uint8_t *)ptr1;
    const uint8_t *u8Ptr2 = (const uint8_t *)ptr2;

    for (uint16_t i = 0; i < num; i++)
        if (u8Ptr1[i] != u8Ptr2[i])
            return 1;

    return 0;
}

bool strEqual(const char *str1, const char *str2)
{
    // const char* command = "clear";
    while (*str1 != '\0' && *str2 != '\0')
    {
        if (*str1 != *str2)
        {
            return false;
        }
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0');
}

char *strCpy(char *dest, const char *src)
{
    char *dest_ptr = dest;
    while ((*dest_ptr++ = *src++))
        ;
    return dest;
}
