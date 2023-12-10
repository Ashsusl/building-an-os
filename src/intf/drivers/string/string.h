#pragma once
#include <stddef.h>
#include <stdint.h>

namespace drivers::string::STR
{
    bool strEqual(const char *str1, const char *str2);
    // Custom implementation of strncpy

    char *strncpy(char *destination, const char *source, size_t num);

    // Custom implementation of strncmp
    int strncmp(const char *str1, const char *str2, size_t num);
}
