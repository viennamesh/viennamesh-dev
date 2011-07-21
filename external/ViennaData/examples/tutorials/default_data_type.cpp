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
// Tutorial: Setting a default data type for a particular key
//


//
// Some class defined by the user (can be anything)
//
struct StandardClass { /* possibly sophisticated internals here */ };

//
// A key type for which a default data type will be set (see the tutorial in key_dispatch.cpp)
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
    // configure 'long' as default data type for keys of type 'char'
    //
    template <>
    struct default_data_for_key<char>
    {
      typedef long    type;
    };
    
    //
    // configure 'double' as default data type for 'MyKey'
    //
    template <>
    struct default_data_for_key<MyKey>
    {
      typedef double    type;
    };
    
    //
    // in addition, use a compile-time key dispatch for the 'MyKey' type
    //
    template <>
    struct key_dispatch<MyKey>
    {
      typedef type_key_dispatch_tag    tag;
    };
  }
}



void print(StandardClass const & obj)
{
    std::cout << "Data (type 'long',   key type 'char' ): " << viennadata::access<char>('c')(obj) << std::endl;
    std::cout << "Data (type 'double', key type 'MyKey'): " << viennadata::access<MyKey, double>()(obj) << std::endl;
    std::cout << "Data (type 'double', key type 'MyKey'): " << viennadata::access<MyKey>()(obj) << std::endl;
    std::cout << "Data (type 'long',   key type 'MyKey'): " << viennadata::access<MyKey, long>()(obj) << std::endl;
}
  
int main(int argc, char *argv[])
{
    StandardClass obj1;
    StandardClass obj2;

    //
    // Store a bit of data on obj1 and obj2
    //
    viennadata::access<char>('c')(obj1) = 360;       //default data type for keys of type char is 'long'
    viennadata::access<MyKey, double>()(obj1) = 1.23456;
    viennadata::access<MyKey>()(obj1) = 3.1415;      //equivalent to the previous line, since 'double' is the default data argument
    viennadata::access<MyKey, long>()(obj1) = 42;

    viennadata::access<char>('c')(obj2) = 42;       //default data type for keys of type char is 'long'
    viennadata::access<MyKey, double>()(obj2) = 9.8765;
    viennadata::access<MyKey>()(obj2) = 2.71;        //equivalent to the previous line, since 'double' is the default data argument
    viennadata::access<MyKey, long>()(obj2) = 360;

    //
    // Output data:
    //
    std::cout << "--- Data for obj1 ---" << std::endl;
    print(obj1);
    std::cout << std::endl;
    std::cout << "--- Data for obj2 ---" << std::endl;
    print(obj2);
    std::cout << std::endl;
    
    
    //
    // Default data type can also be used for copy and erase operations:
    //
    viennadata::copy<char>('c')(obj1, obj2);
    viennadata::copy<char>()(obj2, obj1);
    viennadata::copy<MyKey>()(obj1, obj2);
    viennadata::erase<char>('c')(obj1);
    viennadata::erase<char>()(obj2);
    viennadata::erase<MyKey>()(obj1);
    
    //
    // Output data:
    //
    std::cout << "--- Data for obj1 ---" << std::endl;
    print(obj1);
    std::cout << std::endl;
    std::cout << "--- Data for obj2 ---" << std::endl;
    print(obj2);
    std::cout << std::endl;

    
    //
    // Default data type can also be used for 'move' and find operations:
    //
    viennadata::move<char>('c')(obj2, obj1);     //move data for key 'c'
    viennadata::move<char>()(obj1, obj2);        //move all data for keys of type 'char'
    viennadata::move<MyKey>()(obj2, obj1);

    if (viennadata::find<char>('c')(obj2))
      std::cout << "Found data of default type using key 'c' of type 'char' on obj2." << std::endl;
      
    if (viennadata::find<char>('c')(obj1) == NULL)
      std::cout << "No data found of default type using key 'c' of type 'char' on obj1." << std::endl;

    std::cout << "--- Data for obj1 ---" << std::endl;
    print(obj1);
    std::cout << std::endl;
    std::cout << "--- Data for obj2 ---" << std::endl;
    print(obj2);
    std::cout << std::endl;
    
    return EXIT_SUCCESS;
}
