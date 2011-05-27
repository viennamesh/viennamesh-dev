/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                             rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <string>

#include "examples/benchmarks/common.hpp"

//main ViennaData include file:
#include "viennadata/api.hpp"

//configure ViennaData:
namespace viennadata
{
  namespace config
  {
    template <>
    struct object_identifier<SlimClass>
    {
      typedef object_provided_id    tag;
      typedef size_t                  id_type;

      static size_t get(SlimClass const & obj) { return obj.id(); }
    };

    // tell ViennaData to use type-based dispatch for data_key:
    template <>
    struct key_dispatch<data_key>
    {
      typedef type_key_dispatch_tag    tag;
    };

  }
}



int main(int argc, char *argv[])
{
  std::cout << "--- Test: SlimClass, 1 000 objects, 1000 repetitions ---" << std::endl;
  run<SlimClass>(1000, 1000);

  std::cout << "--- Test: SlimClass, 1 000 000 objects ---" << std::endl;
  run<SlimClass>(1000000, 1);
  
  return EXIT_SUCCESS;
}
