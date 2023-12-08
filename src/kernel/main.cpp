// #include "drivers/video/vga.h"
// #include <idt.h>
// #include "file_system/fat.h"
// #include "multiboot.h"

// namespace VGA = drivers::video::VGA;
// namespace Color = VGA::Color;

// void print_welcome();

// /// The entry point into the MY_OS kernel.
// extern "C" void kernel_main(unsigned long magic, unsigned long addr)
// {
//     // Print the startup banner
//     VGA::clear_screen();

//     print_welcome();

//     VGA::print_hex(magic);

//     VGA::print_str("\n");

//     VGA::print_hex(addr);

//     VGA::print_str("\n");

//     print_welcome();

//     // Create a fat_BS_t structure
//     fat_BS_t bootSector;

//     // Read the boot sector into the structure
//     VGA::readBootSector(&bootSector);

//     // Create temporary text input prompt
//     VGA::print_str("\nroot::> ");

//     init_idt();

//     while (true)
//         ;
// }

#include "drivers/video/vga.h"
#include <idt.h>
#include "file_system/fat.h"
#include "multiboot.h"

namespace VGA = drivers::video::VGA;
namespace Color = VGA::Color;

void print_welcome();
void print_multiboot_info(unsigned long magic, unsigned long addr);

/// The entry point into the MY_OS kernel.
extern "C" void kernel_main(unsigned long magic, unsigned long addr)
{
    // Print the startup banner
    VGA::clear_screen();

    print_welcome();

// Print multiboot information
    print_multiboot_info(magic, addr);

    // Create a fat_BS_t structure
    fat_BS_t bootSector;

    // Read the boot sector into the structure
    VGA::readBootSector(&bootSector);

    // Print the boot sector contents
    VGA::print_str("Boot Sector Contents:\n");
    VGA::readBootSectorAndPrint(&bootSector);

    // Create temporary text input prompt
    VGA::print_str("\nroot::> ");

    init_idt();

    while (true)
        ;
}

/// Prints a startup banner
void print_welcome()
{
    VGA::set_color(Color::GREEN, Color::BLACK);
    VGA::print_str("Welcome to MYOS!\n");
}

// Print multiboot information
void print_multiboot_info(unsigned long magic, unsigned long addr)
{
    VGA::set_color(Color::CYAN, Color::BLACK);
    VGA::print_str("Multiboot Magic Number: ");
    VGA::print_hex(magic);
    VGA::print_str("\n");

    VGA::print_str("Multiboot Information Structure Address: ");
    VGA::print_hex(addr);
    VGA::print_str("\n");
}