#include <print.h>
#include <stdio.h>

void kernel_main()
{
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLUE);
    print_str("Welcome to our 64 bit kernel!");
    print_str("\nNow we will create interfaces to \ncreate a file, \nmove a file, \nand delete that file.\n");
}
