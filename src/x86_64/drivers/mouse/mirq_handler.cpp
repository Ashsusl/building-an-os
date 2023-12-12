#include "drivers/mouse/mirq_handler.h"
#include "port.h"
#include "drivers/video/screen.h"

namespace SCR = drivers::video::SCR;
namespace Color = SCR::Color;

namespace custom
{
    template <typename T>
    constexpr const T &min(const T &a, const T &b)
    {
        return a < b ? a : b;
    }

    template <typename T>
    constexpr const T &max(const T &a, const T &b)
    {
        return a > b ? a : b;
    }
}


namespace drivers::mouse
{
    constexpr uint8_t MOUSE_DATA_PORT = 0x60;
    constexpr int MOUSE_SENSITIVITY = 2;

    void initialize_mouse()
    {
        // Add your mouse initialization code here if needed
    }

    void handle_mouse_interrupt()
    {
        uint8_t extra_bits = inb(MOUSE_DATA_PORT);
        uint8_t move_x = inb(MOUSE_DATA_PORT);
        uint8_t move_y = inb(MOUSE_DATA_PORT);

        int x = static_cast<int>(move_x) - (static_cast<int>(extra_bits << 4) & 0x100);
        int y = static_cast<int>(move_y) - (static_cast<int>(extra_bits << 3) & 0x100);

        if (!(x == 65 && y == 65) && !(x == 97 && y == -159))
        {
            // Clear the previous cursor
            SCR::clear_mouse_cursor(SCR::get_cursor_col(), SCR::get_cursor_row());

            // Update cursor position
            SCR::set_cursor_position(custom::max(static_cast<size_t>(0), custom::min(SCR::get_cursor_col(), static_cast<size_t>(79))),
                                     custom::max(static_cast<size_t>(0), custom::min(SCR::get_cursor_row(), static_cast<size_t>(24))));

            // Draw the new cursor
            SCR::draw_mouse_cursor(SCR::get_cursor_col(), SCR::get_cursor_row());
        }

        // Acknowledge the end of interrupt to the interrupt controller
        outb(0x20, 0x20); // Assuming IRQ 12 is used for the mouse
    }
}
