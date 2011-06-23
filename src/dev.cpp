


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
#include "viennamesh/generation/triangle.hpp"
//#include "viennamesh/adaptation/orienter.hpp"
//#include "viennamesh/adaptation/cell_normals.hpp"
//#include "viennamesh/adaptation/hull_quality.hpp"
//#include "viennamesh/classification/vgmodeler.hpp"
#include "viennamesh/wrapper.hpp"



int main(int argc, char *argv[])
{

   viennautils::io::bnd_reader my_bnd_reader;
   my_bnd_reader(argv[1]); 

   typedef viennamesh::wrapper<viennamesh::tag::bnd, viennautils::io::bnd_reader>      bnd_wrapper_type;
   bnd_wrapper_type                       wrapped_data(my_bnd_reader);      

   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::cervpt>::type        cervpt_hull_mesh_generator_type;
   cervpt_hull_mesh_generator_type        hull_mesher;      
   typedef cervpt_hull_mesh_generator_type::result_type       result_type;


//   typedef viennamesh::result_of::mesh_adaptor<viennamesh::tag::orienter>::type        orienter_adaptor_type;
//   orienter_adaptor_type                  orienter;
//   
//   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::tetgen>::type        mesh_generator_type;
//   mesh_generator_type      mesher;            

//   typedef viennamesh::result_of::mesh_generator<viennamesh::tag::triangle>::type        mesh_generator_type;
//   mesh_generator_type      mesher;            


//   typedef mesh_generator_type::result_type       result_type;

//   //result_type result = mesher(orienter(hull_mesher(wrapped_data)));
   result_type result = hull_mesher(wrapped_data);

   viennagrid::io::vtk_writer<result_type::value_type>  my_vtk_writer;         
   my_vtk_writer.writeDomain(*result, "output.vtu");


   return 0;
}
