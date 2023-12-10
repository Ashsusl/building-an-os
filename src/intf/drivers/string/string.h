#pragma once
#include <stddef.h>
#include <stdint.h>

namespace drivers::string::STR
{
    bool strEqual(const char *str1, const char *str2);
    // Custom implementation of strncpy

    char *strnCpy(char *destination, const char *source, size_t num);

    // Custom implementation of strncmp
    int strnCmp(const char *str1, const char *str2, size_t num);
    char* strChr(const char* str, int c); 
    unsigned int strLen(const char *str);
}
