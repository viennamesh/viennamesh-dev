/* =============================================================================
   Copyright (c) 2010, 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                ViennaUtils - The Vienna Utilities Library
                             -----------------

   authors:    Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Markus Bina                        bina@iue.tuwien.ac.at
               Karl Rupp                          rupp@iue.tuwien.ac.at

   license:    see file LICENSE in the base directory
============================================================================= */


#include "viennautils/config.hpp"

int main(int argc, char * argv[])
{
   typedef viennautils::config<viennautils::tag::xml>::type   ConfigT;

   ConfigT conf;
   conf.read("../input/test.xml");
   conf.dump();
   std::string result = conf.query("/root/head[id='0']/id");
   std::cout << "result: '" << result << "'" << std::endl;
   if(result != "<id>0</id>")
   {
      std::cerr << "Error: query returned wrong result" << std::endl;
      return -1;
   }
   conf.write("output.xml");

   return 0;
}
