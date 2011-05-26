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

#include "viennautils/io/bnd.hpp"
#include "viennautils/io/hin.hpp"
#include "viennautils/file.hpp"

#include "viennagrid/domain.hpp"

#include "viennamesh/wrapper/hin.hpp"
#include "viennamesh/wrapper/bnd.hpp"
#include "viennamesh/generation/cervpt.hpp"
#include "viennamesh/generation/tetgen.hpp"

template<typename WrappedDataT>
int meshing(WrappedDataT& wrapped_data)
{
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        hull_mesh_generator_type;
   hull_mesh_generator_type      hull_mesher;       

   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::tetgen>::type        volume_mesh_generator_type;
   volume_mesh_generator_type    volume_mesher;      

   typedef volume_mesh_generator_type::result_type       volume_result_type;   
   
   std::cout << "testing TetGen directly on wrapped input data .. " << std::endl;
   volume_result_type result1 = volume_mesher(wrapped_data);

   std::cout << "testing TetGen on pre-processed domain .. " << std::endl;   
   volume_result_type result2 = volume_mesher(hull_mesher(wrapped_data));

   return 0;
}


int process_file(std::string filename) 
{
   std::cout << "processing file: " << filename << std::endl;

   std::string input_extension  = viennautils::file_extension(filename);

   if(input_extension == "bnd")
   {
      viennautils::io::bnd_reader my_bnd_reader;
      my_bnd_reader(filename); 

      typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>      bnd_wrapper_type;
      bnd_wrapper_type                    wrapped_data(my_bnd_reader);      

      meshing(wrapped_data);
   }
   else
   if(input_extension == "hin")
   {
      viennautils::io::hin_reader my_hin_reader;
      my_hin_reader(filename); 

      typedef viennamesh::wrapper<viennamesh::tag::hin, viennautils::io::hin_reader>      hin_wrapper_type;
      hin_wrapper_type                    wrapped_data(my_hin_reader);      

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

