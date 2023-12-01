#include "drivers/video/vga.h"
#include <port.h>

namespace drivers::video::VGA
{
    const static size_t NUM_COLS = 80; // The number of columns in the VGA
    const static size_t NUM_ROWS = 25; // The number of rows in the VGA

    uint8_t key_buffer_index = 0;
    char *key_buffer;
    char *clear_buffer;

    /// The currently active brush/color for printing to the VGA.
    uint8_t current_color = Color::from(Color::WHITE, Color::BLACK);

    /// The current location of the cursor on the VGA.
    static Cursor cursor = {
        .row = 0,
        .col = 0,
    };

    void back_space()
    {
        if (VGA::cursor.col > 8)
        {
            VGA::cursor.col--;
            VGA::VGA_Cell empty = {
                .character = ' ',
                .color = Color::from(Color::WHITE, Color::BLACK),
            };
            VGA_BUFFER[cursor.col + NUM_COLS * cursor.row] = empty;
            update_cursor();
        }
    }

    void update_cursor()
    {
        uint16_t cursorLocation = cursor.row * NUM_COLS + cursor.col;

        // The VGA ports for setting the cursor position are 0x3D4 and 0x3D5
        outb(0x3D4, 14); // Set the high cursor byte register
        outb(0x3D5, cursorLocation >> 8);
        outb(0x3D4, 15); // Set the low cursor byte register
        outb(0x3D5, cursorLocation & 0xFF);
    }

    static void clear_row(size_t row)
    {
        const VGA_Cell empty = {
            .character = ' ',
            .color = Color::from(Color::WHITE, Color::BLACK),
        };

        for (size_t col = 0; col < NUM_COLS; col++)
        {
            VGA_BUFFER[col + NUM_COLS * row] = empty;
        }
        update_cursor();
    }

    /// Clears the entire VGA.
    void clear_screen()
    {
        cursor.row = 0;
        cursor.col = 0;

        for (size_t i = 0; i < NUM_ROWS; i++)
        {
            clear_row(i);
        }
        cursor.row = 0;
        cursor.col = 0;
        update_cursor();
    }

    /// Prints a new line to the VGA by updating the cursor's location.
    /// Handles shifting all the lines up when the entire array is filled.
    static void print_newline()
    {
        cursor.col = 0;

        // Screen still has rows beneath
        if (cursor.row < NUM_ROWS - 1)
        {
            cursor.row++;
            return;
        }

        // Shift all rows up 1
        for (size_t row = 1; row < NUM_ROWS; row++)
        {
            for (size_t col = 0; col < NUM_COLS; col++)
            {
                VGA_BUFFER[col + NUM_COLS * (row - 1)] = VGA_BUFFER[col + NUM_COLS * row];
            }
        }

        // Clear the last row
        clear_row(NUM_ROWS - 1);
    }

    /// Prints a tab to the VGA by updating the cursor's location.
    static void print_tab()
    {
        cursor.col += TAB_SIZE;
    }

    /// Prints a single character to the VGA at the current cursor location.
    /// character The character to print.
    void print_chr(uint8_t character)
    {
        if (character == '\n')
        {
            print_newline();
            return;
        }

        if (character == '\t')
        {
            print_tab();
            return;
        }

        if (cursor.col >= NUM_COLS)
        {
            print_newline();
        }

        VGA_BUFFER[cursor.col + NUM_COLS * cursor.row] = (VGA_Cell){
            .character = character,
            .color = current_color,
        };

        cursor.col++;
        update_cursor();
    }

    /// Prints a null-terminated string to the VGA starting at the current cursor location.
    /// Keeps printing the given string until a null byte is encountered.
    /// The string to print. Must be null-terminated.
    void print_str(const char *str)
    {
        for (size_t i = 0; true; i++)
        {
            uint8_t character = str[i];

            if (character == '\0')
            {
                return;
            }

            print_chr(character);
        }
    }

    /// Sets the active brush (foreground and background colors) to be used while printing to the VGA.
    /// The byte representing the color to set as active.
    void set_color(uint8_t color)
    {
        current_color = color;
    }

    // Function to convert an integer to a string
    void int_to_string(int value, char *str, int base)
    {
        static char digits[] = "0123456789ABCDEF"; // For hexadecimal support
        int is_negative = 0;
        if (value < 0)
        {
            is_negative = 1;
            value = -value;
        }

        int index = 0;
        do
        {
            str[index++] = digits[value % base];
            value /= base;
        } while (value > 0);

        if (is_negative)
        {
            str[index++] = '-';
        }

        // Reverse the string
        int start = 0;
        int end = index - 1;
        while (start < end)
        {
            char temp = str[start];
            str[start] = str[end];
            str[end] = temp;
            start++;
            end--;
        }
        str[index] = '\0';
    }

    // Function to print an integer
    void print_int(int value)
    {
        char buffer[12];                  // Assumes 32-bit int (up to 11 characters) and null terminator
        int_to_string(value, buffer, 10); // Convert to decimal
        print_str(buffer);
    }
}
