#pragma once
#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, int value, size_t count);
bool strEqual(const char *str1, const char *str2);
char *strCpy(char *dest, const char *src);