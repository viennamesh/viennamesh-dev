/* =======================================================================

                     ---------------------------------------
                     ViennaMesh - The Vienna Meshing Library
                     ---------------------------------------
                            
   maintainers:    
               Josef Weinbub                      weinbub@iue.tuwien.ac.at
               Johann Cervenka                   cervenka@iue.tuwien.ac.at

   developers:
               Franz Stimpfl, Rene Heinzl, Philipp Schwaha

   license:    
               LGPL, see file LICENSE in the ViennaMesh base directory

   Copyright (c) 2010, Institute for Microelectronics, TU Vienna.
   http://www.iue.tuwien.ac.at

   ======================================================================= */
   
//
// *** SYSTEM includes
//
#include <iostream>
//
// *** GSSE includes
//
#include <gsse/datastructure/domain.hpp>
#include <gsse/datastructure/domain_reader_gssev01.hpp>
#include <gsse/datastructure/domain_write_gssev01.hpp>
#include <gsse/datastructure/domain_algorithms.hpp>
//
// *** LOCAL includes
//
#include "classify.hpp"

static void printHeader()
{
  std::cout << "  " << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaMesh: VGModeler Mesh Classifier 1.0,  2010              --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "--  developed by:                                                --" << std::endl;
  std::cout << "--    Josef Weinbub, Franz Stimpfl, Rene Heinzl, Philipp Schwaha --" << std::endl;
  std::cout << "--  maintained by:                                               --" << std::endl;
  std::cout << "--    Josef Weinbub, Johann Cervenka                             --" << std::endl;
  std::cout << "--  Institute for Microelectronics, TU Wien, Austria             --" << std::endl;
  std::cout << "--  http://www.iue.tuwien.ac.at                                  --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "  " << std::endl;
}

//
// ===================================================================================
//
template< typename Domain >
int process( std::string inputfile, 
             std::string prefix = "" )
{
   Domain domain;

   gsse::read_domain_gssev01(domain, inputfile);      

   gml::classify( domain, prefix );
   
//   gsse::write_domain_gssev01(domain, outputfile);      
   return 0;
}
//
// ===================================================================================
//
int main(int argc, char** argv)
{
   printHeader();

   if ((argc < 2) || (argc > 2))
   {
      std::cout << "Error:: wrong number of input parameters .." << std::endl;
      std::cout << std::endl;
      std::cout << "usage: " << argv[0] << " " 
                << "INPUT-GAU 2 / 3 - FILE   - ie: mesh.gau3" << std::endl;
      return -1;
   }

   std::string filename(argv[1]);
   std::string::size_type pos = filename.rfind(".")+1;
   std::string file_ending = filename.substr(pos);    

   std::string::size_type pos2 = filename.rfind("/")+1;
   std::string prefix;
   if( pos2 != std::string::npos )
   {
      prefix = filename.substr(pos2,pos-pos2-1);
   }
   else
   {
      prefix = filename.substr(0,pos-pos2-1);
   }
//   std::cout << "prefix: " << prefix << std::endl;
   if(file_ending == "gau3")
      return process< gsse::create_domain<3, 3, 1>::type > (argv[1], prefix);
   else
   if(file_ending == "gau2")
      return process< gsse::create_domain<2, 2, 1>::type > (argv[1], prefix);
   else std::cout << "file dimension not supported .." << std::endl;   
   
   
   return 0;
}
   
