#include "print.h"
void kernel_main()
{
    print_clear();
    print_set_color(PRINT_COLOR_BLUE, PRINT_COLOR_WHITE);
    print_str("Welcome to our 64 bit kernel!");
    print_str("\nNow we will create interfaces to \ncreate a file, \nmove a file, \nand delete that file.\n");
}
