/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */


#include "viennautils/messages.hpp"

int main(int argc, char * argv[])
{
   viennautils::msg::error("some error occured!");          // printed in red
   viennautils::msg::warning("this might be a problem!");   // printed in yellow
   viennautils::msg::info("this result is worth watching!");// printed in green

   return 0;
}



