#include "print.h"

void kernel_main()
{
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome to our 64 bit kernel!");
    print_str("\nNow we will create interfaces to \ncreate a file, \nmove a file, \nand delete that file.");
    int num = 42;
    print_str("The answer is: ");
    print_int(num);
}
