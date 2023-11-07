#include <print.h>
#include <isr.h>
#include <vga.h>

void kernel_main()
{
    isr_install();

    set_cursor_pos(0, 0);
    clearwin(COLOR_GRN, COLOR_BLK);

    const char *first = "\n\n\n\nWe can now handle some special characters.";

    putstr(first, COLOR_GRN, COLOR_BLK);

    const char *second = "\nLike tab \t and newline.";

    putstr(second, COLOR_GRN, COLOR_BLK);

    const char *third = "\nAnd it scrolls!";
    for (u16_t i = 0; i < 18; i++)
    {
        putstr(third, COLOR_GRN, COLOR_BLK);
    }

    putstr("\nThis interrupt is most likely NOT a double-fault, but a problem with us not remapping IRQ 8, so it shows up on this channel\n", COLOR_GRN, COLOR_BLK);

//    print_clear();
//   print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLUE);
//     print_str("Welcome to our 64 bit kernel!");
}
