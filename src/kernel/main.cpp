#include "drivers/video/screen.h"
#include <idt.h>
#include "file_system/fat.h"
#include "file_system/disk.h"
#include "multiboot.h"
#include "drivers/string/string.h"
#include "drivers/keyboard/irq_handler.h"

namespace STR = drivers::string::STR;
namespace SCR = drivers::video::SCR;
namespace Color = SCR::Color;
namespace F = drivers::file::F;
namespace KEY = drivers::keyboard;

void print_welcome();
void handle_command(const char *command);
void clearBuffer(char *buffer, unsigned int size);
void print_help();

char buffer[4096];

/// The entry point into the MY_OS kernel.
extern "C" void kernel_main()
{
    // Print the startup banner
    SCR::clear_screen();

    print_welcome();
    print_help();
    
    // Create temporary text input prompt
    SCR::print_str("\nroot::> ");

    init_idt();

    while (true)
    {
        if (KEY::is_command_ready())
        {
            const char *command = KEY::get_command();
            handle_command(command);
            KEY::reset_command();
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
    SCR::set_color(Color::WHITE, Color::BLACK);
    SCR::print_str("Welcome to MYOS!\n");
}

void handle_command(const char *command)
{
    if (STR::strEqual(command, "clear"))
    {
        SCR::clear_screen();
    }
    else if (STR::strEqual(command, "create"))
    {
        F::createFile("hello.txt", 0x00, 2048);
    }
    else if (STR::strEqual(command, "write"))
    {
        const char *data = "Hi!";
        F::writeFile("hello.txt", data, STR::strLen(data));
    }
    else if (STR::strEqual(command, "read"))
    {
        F::readFile("hello.txt", buffer, sizeof(buffer));
        SCR::print_str(buffer);
    }
    else if (STR::strEqual(command, "delete"))
    {
        F::deleteFile("hello.txt");
        clearBuffer(buffer, sizeof(buffer));
    }
    else if (STR::strEqual(command, "help"))
    {
       print_help();
    }
    else
    {
        SCR::print_str("\nBAD COMMAND!!");
    }
    SCR::print_str("\nroot::> ");
    SCR::update_cursor();
    return;
}

void clearBuffer(char *buffer, unsigned int size)
{
    for (unsigned int i = 0; i < size; ++i)
    {
        buffer[i] = 0;
    }
}

void print_help()
{
    SCR::clear_screen();
    SCR::print_str("clear  ::    clears the screen\n");
    SCR::print_str("create ::    Creates a file named hello.txt\n");
    SCR::print_str("write  ::    Writes Hello, World! to the file\n");
    SCR::print_str("read   ::    Reads contents from the file\n");
    SCR::print_str("delete ::    Deletes a file named hello.txt\n");
    SCR::print_str("help   ::    Help menu\n");
}
