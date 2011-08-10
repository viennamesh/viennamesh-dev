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
#include "viennamesh/classification.hpp"
#include "viennamesh/io.hpp"

//
// generate high-quality 3d meshes
//
template<typename WrappedDatastructureT>
void process_3d(WrappedDatastructureT& data, std::string const& outputfile, int checks = 0)
{
   // prepare a hull mesher
   //   generates a 2-simplex unstructured mesh embedded in a three-dimensional
   //   geometrical space
   //
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        hull_mesh_generator_type;
   hull_mesh_generator_type            hull_mesher;      

   // prepare a mesh topology verification adaptor
   //   this mesh adaptation tool tests the validity of a mesh based on topology tests
   //
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::topo_check>::type       topo_checker_type;
   topo_checker_type                   topo_checker;
   
   // prepare a mesh geometry verification adaptor
   //   this mesh adaptation tool tests the validity of a mesh based on geometry tests
   //
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::geom_check>::type       geom_checker_type;
   geom_checker_type                   geom_checker;   

   // prepare a orientation adaptor
   //   this mesh adaptation tool orients the hull mesh elements in counter-clockwise 
   //   orientation. this is required for advancing front volume mesh generators
   //
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::orienter>::type        orienter_adaptor_type;
   orienter_adaptor_type               orienter;

   // compute the cell normals of the hull mesh
   //   when this functor is applied after the orienter functor is, 
   //   all cell normals should point outwards
   // 
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::cell_normals>::type    cell_normals_adaptor_type;
   cell_normals_adaptor_type           cell_normals;         
   
   // prepare a volume mesh generator
   //
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::netgen>::type        volume_mesh_generator_type;
   volume_mesh_generator_type       volume_mesher;      

   // and a mesh classification tool
   //   this tool investigates the quality of the mesh and provides us therefore 
   //   with feedback how 'good' a mesh is
   //
   typedef viennamesh::result_of::mesh_classifier<viennamesh::tag::vgmodeler>::type    mesh_classifier_type;
   mesh_classifier_type mesh_classifier;

   typedef hull_mesh_generator_type::result_type      hull_domainsp_type;
   typedef volume_mesh_generator_type::result_type    vol_domainsp_type;

   // execute the functor chain: 
   //   1. generate a hull mesh of the input geometry (the inner most functor)
   std::cout << "   hull meshing .. " << std::endl;
   hull_domainsp_type   hullmesh = hull_mesher(data);
   
   if(checks == 1)
   {
      std::cout << "   hull checking topology .. " << std::endl;
      topo_checker(hullmesh);

      std::cout << "   hull checking geometry .. " << std::endl;   
      geom_checker(hullmesh);
   }

   //   2. impose consistent ccw cell orientation
   std::cout << "   hull orienting .. " << std::endl;   
   hull_domainsp_type   oriented = orienter(hullmesh);

   //   3. compute the cell normals of the oriented hull mesh
   std::cout << "   hull normals .. " << std::endl;   
   hull_domainsp_type   normals  = cell_normals(oriented);

   //   4. do the volume meshing
   std::cout << "   volume meshing .. " << std::endl;   
   vol_domainsp_type    volume   = volume_mesher(normals);

   //   5. investigate mesh quality
   std::cout << "   volume classifying .. " << std::endl;   
   mesh_classifier(volume);

   // write paraview/vtk output
   std::cout << "   writing domain .. " << std::endl;   
   viennamesh::io::domainwriter(volume, outputfile);
}

int main(int argc, char *argv[])
{
   if(argc != 4)
   {
      std::cerr << "## Parameter Error - usage: " << argv[0] << " inputfile.{hin,bnd,gau32,inp,sgf} outputfile checks[0|1]" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
  
   std::string inputfile(argv[1]);
   std::string outputfile(argv[2]);
   int checks(atoi(argv[3]));
   std::cout << "## " << argv[0] << " processing file: " << inputfile << std::endl;
   
   std::string input_extension  = viennautils::file_extension(inputfile);

   //
   // process different file types
   //
   if(input_extension == "bnd")
   {
      // the reader object
      //
      viennautils::io::bnd_reader my_bnd_reader;
      my_bnd_reader(inputfile); 

      // the reader datastructure is wrapped to offer a specific interface
      //
      typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>      bnd_wrapper_type;
      bnd_wrapper_type                    wrapped_data(my_bnd_reader);      

      // mesh this data
      //
      if(my_bnd_reader.dim_geom() == 3)
         process_3d(wrapped_data, outputfile, checks);
      else
      {
         std::cerr << "## only 3d BND meshes supported .. " << std::endl;
         exit(-1);
      }


   }
   else
   if(input_extension == "hin")
   {
      // the reader object
      //   
      viennautils::io::hin_reader my_hin_reader;
      my_hin_reader(inputfile); 

      // the reader datastructure is wrapped to offer a specific interface
      //
      typedef viennamesh::wrapper<viennamesh::tag::hin, viennautils::io::hin_reader>      hin_wrapper_type;
      hin_wrapper_type                    wrapped_data(my_hin_reader);         

      // mesh this data
      //
      process_3d(wrapped_data, outputfile, checks);
   }
   else
   if(input_extension == "gau32")
   {
      typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
      domain_type domain;
      
      viennagrid::io::importGAU(domain, inputfile);

      // the reader datastructure is wrapped to offer a specific interface
      //
      typedef viennamesh::wrapper<viennamesh::tag::viennagrid, domain_type>      viennagrid_wrapper_type;
      viennagrid_wrapper_type                    wrapped_data(domain);         

      // mesh this data
      //
      process_3d(wrapped_data, outputfile, checks);
   }   
   else
   if(input_extension == "inp")
   {
      typedef viennagrid::domain<viennagrid::config::triangular_3d>     domain_type;
      domain_type domain;
      
      viennagrid::io::importINP(domain, inputfile, true);

      viennamesh::io::domainwriter(domain, "hullinp");

      // the reader datastructure is wrapped to offer a specific interface
      //
      typedef viennamesh::wrapper<viennamesh::tag::viennagrid, domain_type>      viennagrid_wrapper_type;
      viennagrid_wrapper_type                    wrapped_data(domain);         

      // mesh this data
      //
      process_3d(wrapped_data, outputfile, checks);
   }   
   else
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }

   return 0;
}



