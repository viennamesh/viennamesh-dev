/* =============================================================================
   Copyright (c) 2011, Institute for Microelectronics, TU Wien
   http://www.iue.tuwien.ac.at
                             -----------------
                 ViennaMesh - The Vienna Mesh Generation Framework
                             -----------------

   authors:    Josef Weinbub                         weinbub@iue.tuwien.ac.at


   license:    see file LICENSE in the base directory
============================================================================= */

#include<iostream>

#include "viennautils/file.hpp"

#include "viennagrid/io/gau_reader.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennamesh/generator.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennamesh/transfer/viennagrid.hpp"

int main(int argc, char *argv[])
{
   if(argc != 3)
   {
      std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile outputfile" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);

   std::string input_extension  = viennautils::file_extension(inputfile);
   std::string output_extension = viennautils::file_extension(outputfile);
   
   if(input_extension != "gau32")
   {
      std::cerr << "## Error::Inputfile is not a gau32 file .." << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }

//   if(output_extension != "pvd")
//   {
//      std::cerr << "## Error::Inputfile is not a gau32 file .." << std::endl;
//      std::cerr << "## shutting down .." << std::endl;
//      return -1;
//   }

   // prepare an input domain 
   // this domain will hold the input mesh


   typedef viennagrid::domain<viennagrid::config::triangular_3d>                  domain_type;
   domain_type domain;
   
   viennagrid::io::importGAU(domain, inputfile);      

   typedef viennamesh::wrapper<viennamesh::tag::viennagrid, domain_type>     vgrid_wrapper_type;
   vgrid_wrapper_type data_in(domain);      
   
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::tetgen, vgrid_wrapper_type>::type   mesh_generator_type;
   mesh_generator_type mesher(data_in);      

   mesher();         

   typedef viennagrid::domain<viennagrid::config::tetrahedral_3d> domain_out_type;
   domain_out_type domain_out;      
   
   typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
   transfer_type  transfer;
   transfer(mesher, domain_out);      
   
   viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
   my_vtk_writer.writeDomain(domain_out, outputfile);

   return 0;
}
