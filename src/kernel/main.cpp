#include "drivers/video/vga.h"
#include <idt.h>
#include "file_system/fat.h"
#include "multiboot.h"
#include "drivers/string/string.h"
#include "drivers/keyboard/irq_handler.h"

namespace STR = drivers::string::STR;
namespace VGA = drivers::video::VGA;
namespace Color = VGA::Color;
namespace F = drivers::file::F;

void print_welcome();
void handle_command(const char *command);

/// The entry point into the MY_OS kernel.
extern "C" void kernel_main()
{
    // Print the startup banner
    VGA::clear_screen();

    print_welcome();

    // Create temporary text input prompt
    VGA::print_str("\nroot::> ");

    init_idt();

    while (true)
    {
        if (drivers::keyboard::is_command_ready())
        {
            const char *command = drivers::keyboard::get_command();
            handle_command(command);
            drivers::keyboard::reset_command();
        }
        else
        {
            asm volatile("hlt");
        }
    }
}

/// Prints a startup banner
void print_welcome()
{
    VGA::set_color(Color::GREEN, Color::BLACK);
    VGA::print_str("Welcome to MYOS!\n");
}

void handle_command(const char *command)
{
    if (STR::strEqual(command, "clear"))
    {
        VGA::clear_screen();
    }
    else if (STR::strEqual(command, "create"))
    {
        F::createFile("hello.txt", 0x00, 1024);
    }
    else if (STR::strEqual(command, "read"))
    {
        char buffer[1024];
        F::readFile("hello.txt", buffer, sizeof(buffer));
        VGA::print_str(buffer);
    }
    else if (STR::strEqual(command, "write"))
    {
        const char *data = "Hello, World!";
        F::writeFile("hello.txt", data, STR::strLen(data));
    }
    else if (STR::strEqual(command, "read"))
    {
        char buffer[1024];
        F::readFile("hello.txt", buffer, sizeof(buffer));
        VGA::print_str(buffer);
    }
    else if(STR::strEqual(command, "delete"))
    {
        F::deleteFile("hello.txt");
    }
    else
    {
        VGA::print_str("\nBAD COMMAND!!");
    }
    VGA::print_str("\nroot::> ");
    VGA::update_cursor();
    return;
}
