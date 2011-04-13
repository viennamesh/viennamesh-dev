/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "viennautils/config.hpp"
#include "viennautils/convert.hpp"
#include "viennautils/contio.hpp"

#include "vgmodeler/hull_generation.hpp"

namespace viennamesh {
namespace query {

template<typename ConfigT>
std::string dimension_topology(ConfigT const& config)
{
   return config.query("/task/dimension/topology/text()");
}

template<typename ConfigT>
std::string dimension_geometry(ConfigT const& config)
{
   return config.query("/task/dimension/geometry/text()");
}

template<typename ConfigT>
std::string cell_topo(ConfigT const& config)
{
   return config.query("/task/celltopo/key/text()");
}

template<typename ConfigT>
std::string tool(ConfigT const& config)
{
   return config.query("/task/tool/key/text()");
}

} // end namespace query
} // end namespace viennamesh


int main(int argc, char *argv[])
{
   if(argc != 4)
   {
      std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile outputfile configfile" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);
   
   std::string::size_type pos = inputfile.rfind(".")+1;
   std::string input_extension = inputfile.substr(pos, inputfile.size());
   pos = outputfile.rfind(".")+1;   
   std::string output_extension = outputfile.substr(pos, outputfile.size());

   typedef viennautils::config<viennautils::tag::xml>::type    config_type;
   config_type config;
   std::ifstream configstream(argv[3]);
   config.read(configstream);
   configstream.close();

   if(input_extension == "vtu")
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }
   else
   if(input_extension == "pvd")
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }
   else
   if(input_extension == "bnd")
   {
      /*
      if(volume && netgen)
      if(volume && tetgen)      
      if(hull && 
      */
      return -1;   
   }
   else
   if(input_extension == "hin")
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }
   else
   if(input_extension == "gau32")
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }
   else
   if(input_extension == "gts")
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }
   else
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }

   return 0;
}



