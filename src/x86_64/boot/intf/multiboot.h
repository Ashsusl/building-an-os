#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

/* The magic number for the Multiboot2 header. */
#define MULTIBOOT2_HEADER_MAGIC          0xe85250d6

/* The magic number passed by a Multiboot2-compliant boot loader. */
#define MULTIBOOT2_BOOTLOADER_MAGIC      0x36D76289

/* The size of our stack (16KB). */
#define STACK_SIZE                      0x4000

/* Multiboot2 tag types */
#define MULTIBOOT_TAG_TYPE_END          0
#define MULTIBOOT_TAG_TYPE_CMDLINE      1
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT_TAG_TYPE_MODULE       3
#define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 4
#define MULTIBOOT_TAG_TYPE_BOOTDEV      5
#define MULTIBOOT_TAG_TYPE_MMAP         6
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER  8

/* The Multiboot2 header. */
typedef struct multiboot_header
{
  uint32_t magic;
  uint32_t architecture;
  uint32_t header_length;
  uint32_t checksum;
} multiboot_header_t;

/* The Multiboot2 tag structure. */
typedef struct multiboot_tag
{
  uint32_t type;
  uint32_t size;
} multiboot_tag_t;

/* The Multiboot2 information structure. */
typedef struct multiboot_info
{
  uint32_t total_size;
  uint32_t reserved;
} multiboot_info_t;

#endif // MULTIBOOT_H
