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
#include "viennamesh/adaptation.hpp"
#include "viennamesh/io.hpp"

template<typename DomainT>
int meshing(DomainT& domain)
{
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::orienter>::type        orienter_adaptor_type;
   orienter_adaptor_type                  orienter;
   
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::cell_normals>::type    cell_normals_adaptor_type;
   cell_normals_adaptor_type           cell_normals;               
   
   typedef orienter_adaptor_type::result_type  domain_result_type;

   domain_result_type domainres = cell_normals(orienter(domain));

   viennamesh::io::domainwriter(domainres, "output"); 

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

      typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        hull_mesh_generator_type;
      hull_mesh_generator_type        hull_mesher;       
      
      typedef hull_mesh_generator_type::result_type   domain_result_type;
      
      domain_result_type domainsp = hull_mesher(wrapped_data);
      
      meshing(*domainsp);
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


