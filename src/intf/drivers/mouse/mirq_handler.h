#pragma once


#include "mouse.h"


namespace drivers::mouse
{
   /// Required by __attribute__((interrupt))
   struct interrupt_frame;


   __attribute__((interrupt))
   void mirq_handler(interrupt_frame *frame);
}
#pragma once