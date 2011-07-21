/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */


#include "viennautils/dumptype.hpp"

int main(int argc, char * argv[])
{
   typedef double numeric_type;
   numeric_type mynum;

   // one can dump a type
   viennautils::dumptype<numeric_type>();

   // or the type of an instance 
   viennautils::dumptype(mynum);

   // embrace const ..
   viennautils::dumptype(numeric_type());

   return 0;
}



