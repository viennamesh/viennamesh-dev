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
// Tutorial: Storing data in a a dense manner on objects (confer to the manual for a more detailled explanation of 'dense storage')
//


//
// Some class defined by the user that provides an ID mechanism:
//
class ClassWithID
{
  public:
    ClassWithID(size_t i = 0) : id_(i) {}
    
    size_t id() const { return id_; }
  private:
    size_t id_;
    /* possibly other members here */
};

//
// Configure ViennaData:
//
namespace viennadata
{
  namespace config
  {
    //
    // tell ViennaData to use the id() member for ClassWithID as identification mechanism
    //
    template <>
    struct object_identifier<ClassWithID>
    {
      typedef object_provided_id    tag;
      typedef size_t                id_type;

      static size_t get(ClassWithID const & obj) { return obj.id(); }
    };
    
    //
    // store doubles densely, no matter which key is used:
    //
    template <typename KeyType>
    struct storage<KeyType, double, ClassWithID>
    {
      typedef dense_data_tag    tag;
    };
  
  }
  
}





int main(int argc, char *argv[])
{
    ClassWithID obj1(1);
    ClassWithID obj2(2);

    //
    // Store a bit of data on obj1 and obj2
    //
    viennadata::access<char, double>('c')(obj1) = 3.1415;                   //using key of type char, data of type double
    viennadata::access<char, std::string>('c')(obj1) = "Hello";            //using key of type char, data of type std::string
    viennadata::access<std::string, char>("some_character")(obj1) = 'a';   //using key of type std::string, data of type char

    viennadata::access<long, long>(42)(obj2) = 360;                        //using key of type long, data of type long
    viennadata::access<double, std::string>(3.1415)(obj2) = "World";       //using key of type double, data of type std::string
    viennadata::access<std::string, char>("some_character")(obj2) = '!';   //using key of type std::string, data of type char

    //
    // Output data:
    //
    std::cout << "--- Data for obj1 ---" << std::endl;
    std::cout << "Data (type 'double') for key 'c' (type 'char'): " << viennadata::access<char, double>('c')(obj1) << std::endl;
    std::cout << "Data (type 'std::string') for key 'c' (type 'char'): " << viennadata::access<char, std::string>('c')(obj1) << std::endl;
    std::cout << "Data (type 'char') for key 'some_character' (type 'std::string'): " << viennadata::access<std::string, char>("some_character")(obj1) << std::endl;
    
    std::cout << std::endl;
    
    std::cout << "--- Data for obj2 ---" << std::endl;
    std::cout << "Data (type 'long') for key '42' (type 'long'): " << viennadata::access<long, long>(42)(obj2) << std::endl;
    std::cout << "Data (type 'std::string') for key '3.1415' (type 'double'): " << viennadata::access<double, std::string>(3.1415)(obj2) << std::endl;
    std::cout << "Data (type 'char') for key 'some_character' (type 'std::string'): " << viennadata::access<std::string, char>("some_character")(obj2) << std::endl;
    
  return EXIT_SUCCESS;
}
