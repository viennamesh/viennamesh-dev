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
#include "viennautils/file.hpp"
#include "viennautils/io/bnd.hpp"
#include "viennautils/io/hin.hpp"
#ifdef VIENNAMESH_HAVE_GTSIO
#include "viennautils/io/gts.hpp"
#endif

#include "viennagrid/domain.hpp"
#include "viennagrid/io/vtk_writer.hpp"
#include "viennagrid/io/vtk_reader.hpp"
#include "viennagrid/io/gau_reader.hpp"
#include "viennagrid/algorithm/cell_normals.hpp"

#include "viennamesh/generation/cervpt.hpp"
#include "viennamesh/generation/netgen.hpp"
#include "viennamesh/generation/tetgen.hpp"
#include "viennamesh/adaptation/orienter.hpp"
#include "viennamesh/adaptation/cell_normals.hpp"
#include "viennamesh/adaptation/hull_quality.hpp"
#include "viennamesh/classification/vgmodeler.hpp"
#include "viennamesh/wrapper.hpp"

#include <boost/any.hpp> // removeme
#include <boost/type_traits/remove_pointer.hpp>

//#define DUMP_HULL_MESH

template<typename WrappedDatastructureT>
void process(WrappedDatastructureT& data)
{
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        cervpt_hull_mesh_generator_type;
   cervpt_hull_mesh_generator_type     hull_mesher;      

   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::orienter>::type        orienter_adaptor_type;
   orienter_adaptor_type               orienter;

   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::cell_normals>::type    cell_normals_adaptor_type;
   cell_normals_adaptor_type           cell_normals;         
   
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::hull_quality>::type    hull_quality_adaptor_type;
   hull_quality_adaptor_type           hull_quality;                  

   typedef cervpt_hull_mesh_generator_type::result_type       hull_result_type;

   hull_result_type hull_mesh = cell_normals(orienter(hull_mesher(data)));
   hull_result_type adapted_hull_mesh = hull_quality(cell_normals(orienter(hull_mesher(data))));

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

   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::netgen>::type        netgen_volume_mesh_generator_type;
   netgen_volume_mesh_generator_type       volume_mesher;      

   typedef viennamesh::result_of::mesh_classifier<viennamesh::tag::vgmodeler>::type    mesh_classifier_type;
   mesh_classifier_type mesh_classifier;

   typedef netgen_volume_mesh_generator_type::result_type       volume_result_type;
   volume_result_type volume_mesh = mesh_classifier(volume_mesher(hull_mesh));
   volume_result_type adapted_volume_mesh = mesh_classifier(volume_mesher(adapted_hull_mesh));
   
   typedef volume_result_type::value_type                                               volume_domain_type;         
   
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

   if(input_extension == "bnd")
   {
      viennautils::io::bnd_reader my_bnd_reader;
      my_bnd_reader(inputfile); 

      typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>      bnd_wrapper_type;
      bnd_wrapper_type                    wrapped_data(my_bnd_reader);      

      process(wrapped_data);
   }
   else
   if(input_extension == "hin")
   {
      viennautils::io::hin_reader my_hin_reader;
      my_hin_reader(inputfile); 

      typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::hin_reader>      hin_wrapper_type;
      hin_wrapper_type                    wrapped_data(my_hin_reader);         

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



