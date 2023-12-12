#include "isr.h"

namespace SCR = drivers::video::SCR;
namespace Color = SCR::Color;

/// The mnemonics for the different exceptions.
/// These can be found at https://wiki.osdev.org/Exceptions.
const char* exception_names[] = {
    "DE", "DB", "02", "BP",
    "OF", "BR", "UD", "NM",
    "DF", "09", "TS", "NP",
    "SS", "GP", "PF", "15",
    "MF", "AC", "MC", "XF",
    "VE", "CP", "22", "23",
    "24", "25", "26", "27",
    "HV", "VC", "SX", "31",
};

/// Handles all the exceptions for interrupt vectors 0x00-0x1F.
/// The stack frame for the exception.
void exception_handler(Exception_Stack_Frame frame)
{
    const char* exception_name = exception_names[frame.int_no];

    SCR::set_color(Color::WHITE, Color::RED);
    SCR::print_str("ERR: #");
    SCR::print_str(exception_name);
}