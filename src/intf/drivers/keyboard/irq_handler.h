#pragma once

#include "keyboard.h"

namespace drivers::keyboard
{
    /// Required by __attribute__((interrupt))
    struct interrupt_frame;

    __attribute__((interrupt)) 
    void irq_handler(interrupt_frame *frame);
    void reset_key_buffer(char* string);
    uint16_t strLength(char *string);
    uint8_t strEqual(char *string1, char *string2);
}