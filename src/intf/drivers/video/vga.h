#pragma once

#include <stddef.h>
#include <stdint.h>

#define VGA_BUFFER ((VGA_Cell *)0xB8000)

#define TAB_SIZE 4 // Size of a tab in spaces

namespace drivers::video::VGA
{
    namespace Color
    {
        typedef enum Color : uint8_t
        {
            BLACK,
            BLUE,
            GREEN,
            CYAN,
            RED,
            MAGENTA,
            BROWN,
            LIGHT_GRAY,
            DARK_GRAY,
            LIGHT_BLUE,
            LIGHT_GREEN,
            LIGHT_CYAN,
            LIGHT_RED,
            PINK,
            YELLOW,
            WHITE,
        } Color;

        /// Converts a foreground and background color into their byte representation.
        /// The foreground color.
        /// The background color.
        /// The byte representation of the color.
        inline uint8_t from(Color foreground, Color background)
        {
            return background << 4 | foreground;
        }
    }

    /// Represents a location in the VGA where there is a cursor.
    typedef struct Cursor
    {
        size_t row;
        size_t col;
    } Cursor;

    /// Represents a single cell in the VGA tracking both the character and the brush for that character.
    typedef struct __attribute__((packed)) VGA_Cell
    {
        uint8_t character;
        uint8_t color;
    } VGA_Cell;

    void update_cursor();

    void clear_screen();

    // This is for the backspace functionality.
    void back_space();

    // These two are to print the charcters.
    void print_chr(uint8_t character);
    void print_str(const char *string);

    void int_to_string(int, char *, int);
    void print_int(int);

    void set_color(uint8_t color);

    /// Sets the active brush (foreground and background colors) to be used while printing to the VGA.
    /// The foreground color to set as active.
    /// The background color to set as active.
    inline void set_color(Color::Color foreground, Color::Color background)
    {
        set_color(Color::from(foreground, background));
    }
}