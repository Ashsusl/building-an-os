#include "drivers/string/string.h"

namespace drivers::string::STR
{ // Custom implementation of strncpy
    char *strnCpy(char *destination, const char *source, size_t num)
    {
        char *result = destination;

        while (num > 0 && *source != '\0')
        {
            *destination = *source;
            destination++;
            source++;
            num--;
        }

        // Pad the remaining space with null characters if needed
        while (num > 0)
        {
            *destination = '\0';
            destination++;
            num--;
        }

        return result;
    }

    // Custom implementation of strncmp
    int strnCmp(const char *str1, const char *str2, size_t num)
    {
        while (num > 0 && *str1 != '\0' && *str2 != '\0' && *str1 == *str2)
        {
            str1++;
            str2++;
            num--;
        }

        if (num == 0)
        {
            return 0; // Strings are equal up to num characters
        }

        return (int)(*str1) - (int)(*str2);
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

    char *strChr(const char *str, int c)
    {
        while (*str != '\0')
        {
            if (*str == c)
                return (char *)str;
            str++;
        }
        if (c == '\0')
            return (char *)str;
        return NULL;
    }

    unsigned int strLen(const char *str)
    {
        unsigned int length = 0;
        while (str[length] != '\0')
        {
            ++length;
        }
        return length;
    }
}
