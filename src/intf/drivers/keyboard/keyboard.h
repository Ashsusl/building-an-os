#pragma once

#include <stdint.h>
#include "port.h"
#include "scan_code_set_1.h"

#define KEYBOARD_DATA_PORT 0x60

namespace drivers::keyboard
{
    /// Represents the state of keyboard modifier keys.
    typedef struct Key_State {
        bool alt;
        bool caps;
        bool ctrl;
        bool shift;
    } Key_State;

    extern Key_State key_state;

    /// Reads a scan code from the keyboard's data port.
    /// The scan code.
    static inline uint8_t read_byte()
    {
        return inb(KEYBOARD_DATA_PORT);
    }

    /// Gets the character the given scan code represents from the scan code set.
    /// scan_code The scan code to get the char for.
    /// The character associated with the scan code.
    static inline uint8_t get_char(uint8_t scan_code)
    {
        return scan_code_set_1[scan_code];
    }

    /// Capitalizes the given letter.
    /// If the provided letter is not a-z it returns the passed value.
    /// The letter to capitalize.
    /// The capitalized letter.
    static inline uint8_t toupper(uint8_t letter)
    {
        // ascii a = 97, A = 65, letters are all consecutive
        if (letter >= 'a' && letter <= 'z')
        {
            return letter - 32;
        }

        return letter;
    }

    uint8_t get_shifted_variant(uint8_t key);
}