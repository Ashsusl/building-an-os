#include "drivers/keyboard/irq_handler.h"
#include "drivers/video/vga.h"
#include "drivers/memory/mem.h"
#include "file_system/fat.h"
#include "drivers/string/string.h"
#include "port.h"
#include "pic.h"

namespace VGA = drivers::video::VGA;
namespace STR = drivers::string::STR;
namespace Color = VGA::Color;

namespace drivers::keyboard
{
    constexpr size_t BUFFER_SIZE = 256;
    char user_input_buffer[BUFFER_SIZE];
    size_t user_input_index = 0;

    bool command_ready = false;

    bool is_command_ready()
    {
        // A command is ready if the user has pressed enter
        return command_ready;
    }

    const char *get_command()
    {
        // Return the user input buffer as the command
        return user_input_buffer;
    }

    void reset_command()
    {
        command_ready = false;
        // Reset the user input buffer
        reset_key_buffer();
    }

    void reset_key_buffer()
    {
        memSet(user_input_buffer, 0, BUFFER_SIZE);
        user_input_index = 0;
    }

    void handle_key_press(char key)
    {
        if (key == K_ENTR)
        {
            command_ready = true;
        }
        else if (key == K_BKSP)
        {
            if (user_input_index > 0)
            {
                user_input_buffer[--user_input_index] = '\0';
                VGA::back_space();
                VGA::update_cursor();
            }
        }
        else if (key == K_LSFT)
        {
            key_state.shift = true;
        }
        else if (key == K_LCTL)
        {
            key_state.ctrl = true;
        }
        else if (key == K_CAPS)
        {
            key_state.caps = !key_state.caps; // Toggle caps lock state
        }
        else
        {
            if (user_input_index < BUFFER_SIZE - 1)
            {
                user_input_buffer[user_input_index++] = key;
                user_input_buffer[user_input_index] = '\0';
            }
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
        }
    }

    void irq_handler(struct interrupt_frame *frame)
    {
        uint8_t scan_code = read_byte();
        char key = get_char(scan_code < 0x80 ? scan_code : scan_code - 0x80);

        if (scan_code < 0x80)
        {
            // Key pressed
            handle_key_press(key);
        }
        else
        {
            // Key released
            switch (key)
            {
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
                break;
            }
        }
        sendEOI(1); // ACK interrupt handled
    }
}
