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
#include <cassert>

#include "viennautils/numeric.hpp"


template<typename T>
void test()
{
   assert( viennautils::equals(T(0.333), T(0.333)) );
}

int main(int argc, char * argv[])
{
   test<float>();
   test<double>();

   return 0;
}



