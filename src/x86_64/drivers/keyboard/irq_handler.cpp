#include "drivers/keyboard/irq_handler.h"
#include "drivers/video/vga.h"
#include "drivers/memory/mem.h"
#include "port.h"
#include "pic.h"

namespace VGA = drivers::video::VGA;
namespace Color = VGA::Color;

namespace drivers::keyboard
{
    constexpr size_t BUFFER_SIZE = 256;
    char user_input_buffer[BUFFER_SIZE];
    size_t user_input_index = 0;

    void reset_key_buffer()
    {
        memSet(user_input_buffer, 0, BUFFER_SIZE);
        user_input_index = 0;
    }

    /// The handler for the keyboard's IRQ1.
    /// Required for __attribute__((interrupt))
    void irq_handler(struct interrupt_frame *frame)
    {
        sendEOI(1); // ACK and ask for scan code

        uint8_t scan_code = read_byte();                                      // Read scan code from keyboard
        char key = get_char(scan_code < 0x80 ? scan_code : scan_code - 0x80); // Parse the scan code as a key (key release = key press + 0x80)

        if (scan_code < 0xD8)
        {
            // Key pressed
            if (scan_code < 0x80)
            {
                switch (key)
                {
                // No scan code to handle (likely an error)
                case K_NULL:
                    break;

                // Escape not implemented
                case K_ESC:
                    break;

                case K_BKSP:
                    break;

                case K_ENTR:
                    VGA::print_chr('\n');
                    break;

                case K_LCTL:
                    key_state.ctrl = true;
                    break;

                case K_LSFT:
                    key_state.shift = true;
                    break;

                case K_LALT:
                    key_state.alt = true;
                    break;

                case K_CAPS:
                    key_state.caps = !key_state.caps; // Toggle
                    break;

                // NumberLock and ScrollLock not implemented
                case K_NUML:
                case K_SCRL:
                    break;

                // Function keys not implemented
                case K_F1:
                case K_F2:
                case K_F3:
                case K_F4:
                case K_F5:
                case K_F6:
                case K_F7:
                case K_F8:
                case K_F9:
                case K_F10:
                case K_F11:
                case K_F12:
                    break;

                default:
                    if (key_state.shift)
                    {
                        VGA::print_chr(get_shifted_variant(key));
                    }
                    else if (key_state.caps)
                    {
                        VGA::print_chr(toupper(key));
                    }
                    else
                    {
                        VGA::print_chr(key);
                    }
                    break;
                }
            }
            // Key released
            else
            {
                switch (key)
                {
                // No scan code to handle (likely an error)
                case K_NULL:
                    break;

                // Escape not implemented
                case K_ESC:
                    break;

                case K_BKSP:
                    VGA::back_space();
                    VGA::update_cursor();
                    break;

                case K_ENTR:
                    if (strEqual(user_input_buffer, "clear"))
                    {
                        VGA::clear_screen();
                    }
                    else
                    {
                        VGA::print_str("\nBAD COMMAND!!");
                    }
                    VGA::print_str("\nroot::> ");
                    VGA::update_cursor();
                    reset_key_buffer();
                    break;

                case K_LCTL:
                    key_state.ctrl = false;
                    break;

                case K_LSFT:
                    key_state.shift = false;
                    break;

                case K_LALT:
                    key_state.alt = false;
                    break;

                // Doesn't fire on release since press handles the toggle
                case K_CAPS:
                    break;

                // NumberLock and ScrollLock not implemented
                case K_NUML:
                case K_SCRL:
                    break;

                // Function keys not implemented
                case K_F1:
                case K_F2:
                case K_F3:
                case K_F4:
                case K_F5:
                case K_F6:
                case K_F7:
                case K_F8:
                case K_F9:
                case K_F10:
                case K_F11:
                case K_F12:
                    break;

                default:
                    if (user_input_index < BUFFER_SIZE - 1)
                    {
                        user_input_buffer[user_input_index++] = key;
                        user_input_buffer[user_input_index] = '\0';
                    }
                    break;
                }
            }
        }

        sendEOI(1); // ACK interrupt handled
    }
}