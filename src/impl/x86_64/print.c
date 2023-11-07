#include "print.h"
#include <stdint.h>
const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;

struct Char
{
    uint8_t character;
    uint8_t color;
};

struct Char *buffer = (struct Char *)0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

void clear_row(size_t row)
{
    struct Char empty = (struct Char){
        character : ' ',
        color : color,
    };

    for (size_t col = 0; col < NUM_COLS; col++)
    {
        buffer[col + NUM_COLS * row] = empty;
    }
}

void print_clear()
{
    for (size_t i = 0; i < NUM_ROWS; i++)
    {
        clear_row(i);
    }
}

void print_newline()
{
    col = 0;

    if (row < NUM_ROWS - 1)
    {
        row++;
        return;
    }

    for (size_t row = 1; row < NUM_ROWS; row++)
    {
        for (size_t col = 0; col < NUM_COLS; col++)
        {
            struct Char character = buffer[col + NUM_COLS * row];
            buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    clear_row(NUM_COLS - 1);
}

void print_char(char character)
{
    if (character == '\n')
    {
        print_newline();
        return;
    }

    if (col >= NUM_COLS)
    {
        print_newline();
    }

    buffer[col + NUM_COLS * row] = (struct Char){
        character : (uint8_t)character,
        color : color,
    };

    col++;
}

void print_str(char *str)
{
    for (size_t i = 0; 1; i++)
    {
        char character = (uint8_t)str[i];

        if (character == '\0')
        {
            return;
        }

        print_char(character);
    }
}

void print_set_color(uint8_t foreground, uint8_t background)
{
    color = foreground + (background << 4);
}

// Function to convert an integer to a string
void int_to_string(int value, char *str, int base) {
    static char digits[] = "0123456789ABCDEF"; // For hexadecimal support
    int is_negative = 0;
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    int index = 0;
    do {
        str[index++] = digits[value % base];
        value /= base;
    } while (value > 0);

    if (is_negative) {
        str[index++] = '-';
    }

    // Reverse the string
    int start = 0;
    int end = index - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
    str[index] = '\0';
}

// Function to print an integer
void print_int(int value) {
    char buffer[12]; // Assumes 32-bit int (up to 11 characters) and null terminator
    int_to_string(value, buffer, 10); // Convert to decimal
    print_str(buffer);
}