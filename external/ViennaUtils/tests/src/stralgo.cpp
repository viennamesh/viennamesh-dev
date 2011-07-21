/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>
#include <assert.h>
#include "viennautils/stralgo.hpp"

int main(int argc, char * argv[])
{
   assert(viennautils::stralgo::size("this is some text\nover several lines\nsuper test") == 3);

   assert(viennautils::stralgo::size("this is some text/over several lines/super test", "/") == 3);

   return 0;
}



