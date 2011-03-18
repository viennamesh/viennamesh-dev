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

#include "viennautils/io/gau.hpp"
#include "viennautils/dumptype.hpp"

#include "viennagrid/forwards.h"
#include "viennagrid/domain.hpp"
#include "viennagrid/io/vtk_writer.hpp"

#include "viennamesh/interfaces/netgen.hpp"
#include "viennamesh/generator.hpp"
#include "viennamesh/wrapper/gsse01.hpp"
#include "viennamesh/classifier.hpp"
#include "viennamesh/transfer/viennagrid.hpp"

int main(int argc, char * argv[])
{
   if(argc != 4)
   {
      std::cerr << "## Error::Parameter - usage: " << argv[0] << " inputfile.{gts} outputfile.{vtu} dimension" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);
   int dim(atoi(argv[3]));
   
   std::string::size_type pos = inputfile.rfind(".")+1;
   std::string input_extension = inputfile.substr(pos, inputfile.size());
   pos = outputfile.rfind(".")+1;   
   std::string output_extension = outputfile.substr(pos, outputfile.size());
   
   if(input_extension == "gau32") // read an old gsse v01 hull mesh
   {
      typedef gsse::detail_topology::unstructured<2>                                unstructured_topology_32t; 
      typedef gsse::get_domain<unstructured_topology_32t, double, double,3>::type   domain_32t;
      domain_32t domain;
      domain.read_file(inputfile, false);
      
      typedef viennamesh::wrapper<viennamesh::tag::gsse01, domain_32t>     gsse01_wrapper_type;;
      gsse01_wrapper_type data_in(domain);      
      
      typedef viennamesh::result_of::mesh_generator<viennamesh::tag::netgen, gsse01_wrapper_type>::type   mesh_generator_type;
      mesh_generator_type mesher(data_in);      
       
      mesher( boost::fusion::make_map<viennamesh::tag::criteria, viennamesh::tag::size>(viennamesh::tag::conforming_delaunay(), 1.0) );         

      typedef viennagrid::domain<viennagrid::config::tetrahedral_3d> domain_out_type;
      domain_out_type domain_out;      
      
      typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
      transfer_type  transfer;
      transfer(mesher, domain_out);

      typedef viennamesh::result_of::mesh_classifier<viennamesh::tag::vgmodeler>::type  mesh_classifier_type;
      mesh_classifier_type mesh_classifier;
      mesh_classifier(mesher);
      
      if(output_extension == "vtk")
      {
         std::cout << "# viennamesh::writing vtk files .. " << std::endl;
         viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
         my_vtk_writer.writeDomain(domain_out, outputfile);
      }
      else
      {
         std::cerr << "## Error: output fileformat not supported: " << output_extension << std::endl;
         std::cerr << "## shutting down .." << std::endl;     
         return -1;         
      }              
   }
   else
   {
      std::cerr << "## Error: input fileformat not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .." << std::endl;     
      return -1;
   }
   
   return 0;
}



