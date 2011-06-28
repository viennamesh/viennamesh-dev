/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

// *** system includes
#include <iostream>

// *** viennamesh includes
#include "viennamesh/common.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennamesh/generation.hpp"

template<typename WrappedDataT>
int meshing(WrappedDataT& wrapped_data)
{
   typedef typename viennamesh::result_of::mesh_generator<viennamesh::tag::triangle>::type        mesh_generator_type;
   mesh_generator_type  mesher;

   typedef typename mesh_generator_type::result_type       result_type;   

   result_type result = mesher(wrapped_data);

   viennagrid::io::vtk_writer<typename result_type::value_type>  vtk_writer;         
   vtk_writer.writeDomain(*result, "mesh.vtu");

   return 0;
}


int process_file(std::string filename) 
{
   std::cout << "processing file: " << filename << std::endl;

   std::string input_extension  = viennautils::file_extension(filename);

   if(input_extension == "sgf")
   {
      typedef viennagrid::domain<viennagrid::config::line_2d>     domain_type;
      domain_type domain;
   
      viennagrid::io::sgf_reader my_sgf_reader;
      my_sgf_reader(domain, filename);   

      typedef viennamesh::wrapper<viennamesh::tag::viennagrid, domain_type>      wrapper_type;
      wrapper_type                    wrapped_data(domain);      

      meshing(wrapped_data); 
   }
   else
   {
      std::cerr << "Error: Fileformat not supported of input file: " << filename << std::endl;
      return -1;
   }
   return 0;
}

int main(int argc, char *argv[])
{
   if(argc != 2)
   {
      std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
   std::string inputfile(argv[1]);
   
   process_file(inputfile);

  
   return 0;
}

