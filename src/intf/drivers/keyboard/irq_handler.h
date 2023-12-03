#pragma once

#include "keyboard.h"

namespace drivers::keyboard
{
    /// Required by __attribute__((interrupt))
    struct interrupt_frame;

    __attribute__((interrupt)) 
    void irq_handler(interrupt_frame *frame);
    void reset_key_buffer();
}