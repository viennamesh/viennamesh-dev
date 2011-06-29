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


//
// mesh the data provided by the wrapped datastructure
//
template<typename WrappedDatastructureT>
void process(WrappedDatastructureT& data)
{
   // prepare a hull mesher
   //   generates a 2-simplex unstructured mesh embedded in a three-dimensional
   //   geometrical space
   //
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        cervpt_hull_mesh_generator_type;
   cervpt_hull_mesh_generator_type     hull_mesher;      

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
   
   // prepare a hull mesh adaptation tool which significantly improves the 
   // quality of the hull mesh. highly degenerate elements are removed, 
   // which results in a hull mesh with well formed elements. 
   // this is the perfect start mesh for an advancing front volume mesh
   //
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::hull_quality>::type    hull_quality_adaptor_type;
   hull_quality_adaptor_type           hull_quality;                  

   // derive the result type of the hull mesh
   //
   typedef cervpt_hull_mesh_generator_type::result_type       hull_result_type;

   // execute the functor chain: 
   //   1. generate a hull mesh of the input geometry (the inner most functor)
   //   2. impose the cell orientation
   //   3. compute the cell normals of the oriented hull mesh
   // store this non-quality adapted hull mesh result for later comparisons
   //
   hull_result_type hull_mesh = cell_normals(orienter(hull_mesher(data)));
   
   // additionally to the hull meshing, improve the quality of the mesh
   // and store the result in a different result, for comparison 
   //
   hull_result_type adapted_hull_mesh = hull_quality(hull_mesh);

   // optionally, one can dump the oriented hull mesh with the computed cell normals
   // the cell normals can be verified when utilizing paraview:
   //   apply modules: cell-center and glyph
#ifdef DUMP_HULL_MESH
         typedef hull_result_type::value_type                                                      hull_domain_type;
         typedef hull_domain_type::config_type                                                     hull_domain_configuration_type;
         typedef viennagrid::result_of::ncell_type<hull_domain_configuration_type, hull_domain_configuration_type::cell_tag::topology_level>::type     hull_cell_type;
         viennagrid::io::vtk_writer<hull_domain_type>  my_hull_vtk_writer;
         my_hull_vtk_writer.add_cell_data_normal(
            viennagrid::io::io_data_accessor_segment_based<
               hull_cell_type, viennagrid::seg_cell_normal_tag, viennagrid::seg_cell_normal_data::type
            >(viennagrid::seg_cell_normal_tag()), "cell_normals");
         my_hull_vtk_writer.writeDomain(*hull_mesh, "hull_mesh.vtu");
#endif

   // prepare a volume mesh generator
   //
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::netgen>::type        netgen_volume_mesh_generator_type;
   netgen_volume_mesh_generator_type       volume_mesher;      
   
   // and a mesh classification tool
   //   this tool investigates the quality of the mesh and provides us therefore 
   //   with feedback how 'good' a mesh is
   //
   typedef viennamesh::result_of::mesh_classifier<viennamesh::tag::vgmodeler>::type    mesh_classifier_type;
   mesh_classifier_type mesh_classifier;

   // setup the volume mesh result type
   //
   typedef netgen_volume_mesh_generator_type::result_type       volume_result_type;
   
   // execute the volume meshing functor chain and investigate the mesh quality
   // based on the non-quality improved hull mesh
   //
   volume_result_type volume_mesh = mesh_classifier(volume_mesher(hull_mesh));
   
   // do the same with the quality improved hull mesh
   //
   volume_result_type adapted_volume_mesh = mesh_classifier(volume_mesher(adapted_hull_mesh));
   
   typedef volume_result_type::value_type                                               volume_domain_type;         
   
   // write both volume meshes to vtk files to be investigated by paraview
   //
   viennagrid::io::vtk_writer<volume_domain_type>  my_volume_vtk_writer;         
   my_volume_vtk_writer.writeDomain(*volume_mesh, "volume_mesh.vtu");
   my_volume_vtk_writer.writeDomain(*adapted_volume_mesh, "adapted_volume_mesh.vtu");         
}

int main(int argc, char *argv[])
{
   if(argc != 2)
   {
      std::cerr << "## Parameter Error - usage: " << argv[0] << " inputfile.{hin,bnd}" << std::endl;
      std::cerr << "## shutting down .." << std::endl;
      return -1;
   }
   
  
   std::string inputfile(argv[1]);

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
      process(wrapped_data);
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
      process(wrapped_data);
   }
   else
   {
      std::cerr << "## input file format not supported: " << input_extension << std::endl;
      std::cerr << "## shutting down .. " << std::endl;
      return -1;   
   }

   return 0;
}



