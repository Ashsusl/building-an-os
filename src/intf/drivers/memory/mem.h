#pragma once
#include <stddef.h>
#include <stdint.h>

void *memCpy(void *dest, const void *src, size_t count);
void *memSet(void *dest, int value, size_t count);
int memCmp(const void* ptr1, const void* ptr2, uint16_t num);
bool strEqual(const char *str1, const char *str2);
char *strCpy(char *dest, const char *src);