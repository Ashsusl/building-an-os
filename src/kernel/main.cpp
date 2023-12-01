#include "drivers/video/vga.h"
#include <idt.h>
namespace VGA = drivers::video::VGA;
namespace Color = VGA::Color;

void print_welcome();

/// The entry point into the StasisOS kernel.
extern "C" void kernel_main()
{
    // Print the startup banner
    VGA::clear_screen();
    print_welcome();

    // Create temporary text input prompt
    VGA::print_str("root::> ");

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