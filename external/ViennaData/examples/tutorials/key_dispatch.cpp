/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                             rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#include <iostream>
#include <string>

//main ViennaData include file:
#include "viennadata/api.hpp"

//
// Tutorial: Activating type based dispatch for a particular key
//


//
// Some class defined by the user (can be anything)
//
struct StandardClass { /* possibly sophisticated internals here */ };

//
// A key type for which a default data type will be set
//
struct MyKey {};


//
// Configure ViennaData:
//
namespace viennadata
{
  namespace config
  {
    
    //
    // The following specialization sets a compile-time key dispatch for the 'MyKey' type
    //
    template <>
    struct key_dispatch<MyKey>
    {
      typedef type_key_dispatch_tag    tag;
    };
  }
}



int main(int argc, char *argv[])
{
    StandardClass obj1;
    StandardClass obj2;

    //
    // Store a bit of data on obj1 and obj2. Note that there are no key arguments needed any longer.
    //
    viennadata::access<MyKey, double>()(obj1) = 1.23456;
    viennadata::access<MyKey, long>()(obj1) = 42;

    viennadata::access<MyKey, double>()(obj2) = 9.8765;
    viennadata::access<MyKey, long>()(obj2) = 360;

    //
    // Output data:
    //
    std::cout << "--- Data for obj1 ---" << std::endl;
    std::cout << "Data (type 'double'): " << viennadata::access<MyKey, double>()(obj1) << std::endl;
    std::cout << "Data (type 'long'):   " << viennadata::access<MyKey, long>()(obj1) << std::endl;
    std::cout << std::endl;
    std::cout << "--- Data for obj2 ---" << std::endl;
    std::cout << "Data (type 'double'): " << viennadata::access<MyKey, double>()(obj2) << std::endl;
    std::cout << "Data (type 'long'):   " << viennadata::access<MyKey, long>()(obj2) << std::endl;

  return EXIT_SUCCESS;
}
