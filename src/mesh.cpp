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

#include "viennamesh/wrapper/viennagrid.hpp"
#include "viennamesh/interfaces/tetgen.hpp"
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

   if(output_extension != "pvd")
   {
      std::cerr << "## Error::Inputfile is not a gau32 file .." << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }

   // prepare an input domain 
   // a domain is basically a datastructure which holds the mesh elements.
   // additionally it provides traversal and access mechanisms for the stored mesh
   //
   typedef viennagrid::domain<viennagrid::config::triangular_3d>        domain_type;
   domain_type domain;
   
   // read the input file
   //
   viennagrid::io::importGAU(domain, inputfile);      
   
   //
   // ***** play with the domain ****
   //
   
   // some meta-love to derive the corresponding types from the given domain type
   typedef domain_type::segment_type                                             segment_type;
   typedef viennagrid::result_of::const_ncell_container<segment_type, 0>::type   vertex_cont_type;
   typedef viennagrid::result_of::iterator<vertex_cont_type>::type               vertex_iter_type;   
   
   // travese the segments of the domain
   for(std::size_t si = 0; si < domain.segment_size(); ++si)
   {
      // extract the current segment object
      segment_type const & seg = domain.segment(si);
   
      // extract the segment specific vertices
      vertex_cont_type vertices = viennagrid::ncells<0>(seg);
      
      // traverse the vertices
      for(vertex_iter_type vit = vertices.begin(); vit != vertices.end(); vit++)
      {
         // access the geometry information of the current vertex
         // note: commented due to much output ;-)
         // std::cout << vit->getPoint() << std::endl;
      }
   }


   //
   // ***** volume meshing ****
   //

   // use a wrapper for the domain
   // this wrapper approach allows to provide a unified mesh datastructure interface 
   // for viennamesh. this way we can utilize viennamesh for different input mesh 
   // datastructures.
   //
   typedef viennamesh::wrapper<viennamesh::tag::viennagrid, domain_type>     vgrid_wrapper_type;
   vgrid_wrapper_type data_in(domain);      
   
   // set up a mesh generator 
   // in this case we use a specific generator: TetGen
   //
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::tetgen, vgrid_wrapper_type>::type   mesh_generator_type;
   mesh_generator_type mesher(data_in);      

   // execute the meshing process with default parameters
   //
   mesher();         

   // prepare a output domain which will hold the volume mesh result
   // note the difference towards the input domain. we now require a volume mesh, 
   // hence a tetrahedral 3d configuration is required.
   //
   typedef viennagrid::domain<viennagrid::config::tetrahedral_3d> domain_out_type;
   domain_out_type domain_out;      
   
   // utilize a transfer function which specifially transfers the mesh from the 
   // mesh generator object to a viennagrid domain
   //
   typedef viennamesh::transfer<viennamesh::tag::viennagrid>      transfer_type;
   transfer_type  transfer;
   transfer(mesher, domain_out);      
   
   // write the viennagrid mesh (aka the output/result volume mesh) based 
   // on the vtk file format
   //
   viennagrid::io::Vtk_writer<domain_out_type> my_vtk_writer;
   my_vtk_writer.writeDomain(domain_out, outputfile);

   return 0;
}


