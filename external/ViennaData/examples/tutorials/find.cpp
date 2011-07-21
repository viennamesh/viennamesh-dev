/* =======================================================================
   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at
                             -----------------
                     ViennaData - The Vienna Data Storage Library
                             -----------------

   authors:    Karl Rupp                             rupp@iue.tuwien.ac.at

   license:    MIT (X11), see file LICENSE in the ViennaData base directory
======================================================================= */

#include <string>
#include <iostream>

//main ViennaData include file:
#include "viennadata/api.hpp"

//
// Tutorial: Basic use of viennadata::find()
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
    // Store a bit of data on obj1 and obj2
    //
    viennadata::access<char, double>('c')(obj1) = 3.1415;                   //using key of type char, data of type double
    viennadata::access<char, std::string>('c')(obj1) = "Hello";            //using key of type char, data of type std::string
    viennadata::access<std::string, char>("some_character")(obj1) = 'a';   //using key of type std::string, data of type char
    viennadata::access<MyKey, long>()(obj1) = 360;

    viennadata::access<long, long>(42)(obj2) = 360;                        //using key of type long, data of type long
    viennadata::access<double, std::string>(3.1415)(obj2) = "World";       //using key of type double, data of type std::string
    viennadata::access<std::string, char>("some_character")(obj2) = '!';   //using key of type std::string, data of type char
    viennadata::access<MyKey, double>()(obj2) = 360.0;

    //
    // Some use of find on obj1:
    //
    std::cout << "--- find() on obj1 ---" << std::endl;
    std::cout << "Searching for char data using key 'some_character' of type std::string: ";
    if (viennadata::find<std::string, char>("some_character")(obj1))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for double data using key 'some_character' of type std::string: ";
    if (viennadata::find<std::string, double>("some_character")(obj1))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for std::string data using key '3.1415' of type double: ";
    if (viennadata::find<double, std::string>(3.1415)(obj1))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for long data using key '42' of type long: ";
    if (viennadata::find<long, long>(42)(obj1))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for double data using key 'c' of type char: ";
    if (viennadata::find<char, double>('c')(obj1))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for long data using key type MyKey: ";
    if (viennadata::find<MyKey, long>()(obj1))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;

    std::cout << "Searching for double data using key type MyKey: ";
    if (viennadata::find<MyKey, double>()(obj1))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    // Some use of find on obj2:
    //
    std::cout << "--- find() on obj2 ---" << std::endl;
    std::cout << "Searching for char data using key 'some_character' of type std::string: ";
    if (viennadata::find<std::string, char>("some_character")(obj2))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for double data using key 'some_character' of type std::string: ";
    if (viennadata::find<std::string, double>("some_character")(obj2))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for std::string data using key '3.1415' of type double: ";
    if (viennadata::find<double, std::string>(3.1415)(obj2))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for long data using key '42' of type long: ";
    if (viennadata::find<long, long>(42)(obj2))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    //
    std::cout << "Searching for double data using key 'c' of type char: ";
    if (viennadata::find<char, double>('c')(obj2))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;

    //
    std::cout << "Searching for long data using key type MyKey: ";
    if (viennadata::find<MyKey, long>()(obj2))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;

    std::cout << "Searching for double data using key type MyKey: ";
    if (viennadata::find<MyKey, double>()(obj2))
      std::cout << "FOUND!";
    else
      std::cout << "not found.";
    std::cout << std::endl;
    
    
  return EXIT_SUCCESS;
}
