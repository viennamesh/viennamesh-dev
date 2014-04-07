#include "viennamesh/core/algorithm_factory.hpp"


#include "viennamesh/algorithm/all.hpp"
// #include "viennamesh/algorithm/triangle.hpp"
// #include "viennamesh/algorithm/tetgen.hpp"
// #include "viennamesh/algorithm/netgen.hpp"
// #include "viennamesh/algorithm/viennagrid.hpp"


namespace viennamesh
{

  algorithm_factory_t::algorithm_factory_t()
  {
    register_algorithm<viennamesh::io::mesh_reader>("mesh_reader");
    register_algorithm<viennamesh::io::string_reader>("string_reader");
    register_algorithm<viennamesh::io::mesh_writer>("mesh_writer");

#ifdef WITH_TRIANGLE
    register_algorithm<viennamesh::triangle::mesh_generator>("triangle_mesh_generator");
#endif

#ifdef WITH_TETGEN
    register_algorithm<viennamesh::tetgen::mesh_generator>("tetgen_mesh_generator");
#endif

#ifdef WITH_NETGEN
    register_algorithm<viennamesh::netgen::csg_mesh_generator>("netgen_csg_mesh_generator");
#ifdef NETGEN_WITH_OPENCASCADE
    register_algorithm<viennamesh::netgen::occ_mesh_generator>("netgen_occ_mesh_generator");
#endif
#endif


    register_algorithm<viennamesh::line_mesh_generator>("line_mesh_generator");

    register_algorithm<viennamesh::affine_transform>("affine_transform");
    register_algorithm<viennamesh::extract_hull>("extract_hull");
    register_algorithm<viennamesh::hyperplane_clip>("hyperplane_clip");

    register_algorithm<viennamesh::map_segments>("map_segments");
    register_algorithm<viennamesh::merge_meshes>("merge_meshes");
    register_algorithm<viennamesh::line_coarsening>("line_coarsening");
  }


  algorithm_factory_t & algorithm_factory()
  {
    static algorithm_factory_t factory_;
    return factory_;
  }
}
