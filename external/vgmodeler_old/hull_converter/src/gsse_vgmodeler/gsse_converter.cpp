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

// *** system includes
//
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unistd.h>


// ** BOOST includes
//
#include<boost/lexical_cast.hpp>
#include<boost/thread/thread.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/bind.hpp>

// *** GSSE includes
//
#include "gsse_meshing_includes.hpp"
//#include "gsse_vgmodeler.hpp"
#include "include/hin.hpp"

#include "include/threadpool.hpp"


   
// --------------------------------------



// #########################################################
// #########################################################



// #########################################################
// #########################################################

int create_surface_mesh(const std::string& filename_in, gsse::domain_32t& domain, int meshing_type=0)
{
  gsse_surface_interface   gsse_surface;

//  std::cout << "## Reading HIN file: " << filename_in  << std::endl; 
  gsse_surface.readGeometry(filename_in);

//  std::cout << "## Converting file" << std::endl;
  gsse_surface.triangulateSurface();
      
  //std::cout << "## Finalizing surface " << std::endl;
  gsse_surface.extractSurface(domain);

  return 0;
}



static void printHeader()
{
  std::cout << "  " << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "-- ViennaMesh: VGModeler Hull Mesh Converter 1.0,  2010          --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "--  developed by:                                                --" << std::endl;
  std::cout << "--    Franz Stimpfl, Rene Heinzl, Philipp Schwaha                --" << std::endl;
  std::cout << "--  maintained by:                                               --" << std::endl;
  std::cout << "--    Josef Weinbub, Johann Cervenka                             --" << std::endl;
  std::cout << "--  Institute for Microelectronics, TU Wien, Austria             --" << std::endl;
  std::cout << "--  http://www.iue.tuwien.ac.at                                  --" << std::endl;
  std::cout << "-------------------------------------------------------------------" << std::endl;
  std::cout << "  " << std::endl;
}


extern int convert_dfise(const std::string& filename_in, const std::string& filename_out);

// *********************************
//
int main(int argc, char** argv)
{

  printHeader();

  if (argc < 3)
    {
      std::cout << "Usage: " <<  argv[0] << " input_file.{bnd|hin|gau32} output_file.gau32" << std::endl;
      return -1;
    } 


  std::string filename_in(argv[1]);
  std::string filename_out(argv[2]);
  std::vector<std::string> orient_changer;
  gsse::domain_32t  domain_input;

  std::cout << "## input file: " << filename_in  << std::endl;
  std::cout << "## input file: " << filename_out  << std::endl;  

  try
    {
      std::string::size_type pos = filename_in.rfind(".")+1;
      std::string filename_in_extension = filename_in.substr(pos, filename_in.size());

      typedef gsse::domain_traits<gsse::domain_32t>::point_t		  point_t;
      std::map<point_t,long> additional_vertices;
     
      // 
      // SURFACE MESHING
      //
      /*
      if (filename_in_extension == "bnd")
      {
         std::cout << "## Converting BND file: " << filename_in << std::endl;
         //convert_dfise(filename_in, "temp.hin");

         //create_surface_mesh("temp.hin", domain_input);    
         //int ret = system("rm temp.hin");
	 create_surface_mesh_bnd(filename_in, domain_input);
      }
      else*/
      if (filename_in_extension == "hin" || filename_in_extension == "bnd")      
      { 
         std::cout << "## Converting HIN file: " << filename_in  << std::endl; 

         create_surface_mesh(filename_in, domain_input);    
      }
      else if (filename_in_extension == "gau32")      
      {  
         domain_input.read_file(filename_in, false);

      }
      domain_input.write_file(filename_out);
    }
  catch (std::exception& ex)
    { 
      std::cout << ex.what() << std::endl;
    }

  return 0;
}








