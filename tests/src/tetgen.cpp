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


#include "viennautils/io.hpp"
#include "viennautils/dumptype.hpp"
#include "viennautils/file.hpp"

#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/domain.hpp"

#include "viennamesh/interfaces/tetgen.hpp"
#include "viennamesh/generator.hpp"
#include "viennamesh/wrapper.hpp"
#include "viennamesh/classifier.hpp"
#include "viennamesh/transfer/viennagrid.hpp"

int main(int argc, char * argv[])
{
   std::string inputfile("../input/cu_lowk_leti_right_oriented.gau32");
   std::string outputfile("output_tetgen.vtk");
   
   if(!viennautils::file_exists(inputfile))
   {
      std::cerr << "ViennaMesh::Test::Tetgen: inputfile does not exist" << std::endl;
      std::cerr << "   file: " << inputfile << std::endl;
      return -1;
   }
   
   std::string::size_type pos = inputfile.rfind(".")+1;
   std::string input_extension = inputfile.substr(pos, inputfile.size());
   pos = outputfile.rfind(".")+1;   
   std::string output_extension = outputfile.substr(pos, outputfile.size());
   

   if(input_extension == "gau32") // read an old gsse v01 hull mesh
   {
      // read input file
      //
      typedef gsse::detail_topology::unstructured<2>                                unstructured_topology_32t; 
      typedef gsse::get_domain<unstructured_topology_32t, double, double,3>::type   domain_32t;
      domain_32t domain;
      domain.read_file(inputfile, false);
      
      // create a wrapper for the gsse-01 domain
      //
      typedef viennamesh::wrapper<viennamesh::tag::gsse01, domain_32t>     gsse01_wrapper_type;;
      gsse01_wrapper_type data_in(domain);      
      
      // create a vgmodeler volume mesher and pass the domain wrapper to the objects constructor
      typedef viennamesh::result_of::mesh_generator<viennamesh::tag::tetgen, gsse01_wrapper_type>::type   mesh_generator_type;
      mesh_generator_type mesher(data_in);      
       
      // start meshing
      //   note: meshing parameters will be set here .. 
      //
      mesher( );         

      // create a viennagrid output domain
      //   note: this domain will hold the output volume mesh
      //
      typedef viennagrid::domain<viennagrid::config::tetrahedral_3d> domain_out_type;
      domain_out_type domain_out;      
      
      // transfer the mesh result to the output viennagrid domain
      //
      typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
      transfer_type  transfer;
      transfer(mesher, domain_out);

      // use the viennagrid domains vtk writer to write the multi-segment output files
      //   notes: produces a master file (*.pvd) and *.vtu files for each segment
      //          to view the mesh, start paraview and load the *.pvd file.
      //
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



