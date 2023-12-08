#pragma once


#include <stdint.h>
#include <port.h>


namespace drivers::mouse
{
   struct Mouse_State {
       int x;
       int y;
       bool left_button;
       bool right_button;
   };


   extern Mouse_State mouse_state;


   // Initialize the mouse
   void initialize_mouse();


   // Handle mouse interrupt
   void handle_mouse_interrupt();


   // Additional mouse-related functions as needed
}
