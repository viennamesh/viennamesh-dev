


#include <iostream>
#include <vector>

#include "viennautils/io/bnd.hpp"

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



int main(int argc, char *argv[])
{

   viennautils::io::bnd_reader my_bnd_reader;
   my_bnd_reader(argv[1]); 

   typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>      bnd_wrapper_type;
   bnd_wrapper_type                       wrapped_data(my_bnd_reader);      

   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        cervpt_hull_mesh_generator_type;
   cervpt_hull_mesh_generator_type        hull_mesher;      
   
   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::orienter>::type        orienter_adaptor_type;
   orienter_adaptor_type                  orienter;
   
   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::tetgen>::type        volume_mesh_generator_type;
   volume_mesh_generator_type      volume_mesher;            

   typedef volume_mesh_generator_type::result_type       result_type;

   result_type result = volume_mesher(orienter(hull_mesher(wrapped_data)));
   //result_type result = volume_mesher(wrapped_data);

   viennagrid::io::vtk_writer<result_type::value_type>  my_volume_vtk_writer;         
   my_volume_vtk_writer.writeDomain(*result, "volume_mesh.vtu");


   return 0;
}
