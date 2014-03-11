#include "viennamesh/core/algorithm_factory.hpp"


#include "viennamesh/algorithm/io.hpp"
#include "viennamesh/algorithm/triangle.hpp"
#include "viennamesh/algorithm/tetgen.hpp"
#include "viennamesh/algorithm/netgen.hpp"
#include "viennamesh/algorithm/viennagrid.hpp"


namespace viennamesh
{

  algorithm_factory_t::algorithm_factory_t()
  {
    register_algorithm<viennamesh::io::mesh_reader>("mesh_reader");
    register_algorithm<viennamesh::io::string_reader>("string_reader");
    register_algorithm<viennamesh::io::mesh_writer>("mesh_writer");

    register_algorithm<viennamesh::triangle::algorithm>("generate_triangle");
    register_algorithm<viennamesh::tetgen::algorithm>("generate_tetgen");
    register_algorithm<viennamesh::netgen::csg_mesher>("netgen_csg");

    register_algorithm<viennamesh::affine_transform::algorithm>("affine_transform");
    register_algorithm<viennamesh::extract_hull::algorithm>("extract_hull");
    register_algorithm<viennamesh::hyperplane_clip::algorithm>("hyperplane_clip");

    register_algorithm<viennamesh::map_segments::algorithm>("map_segments");
    register_algorithm<viennamesh::merge_meshes::algorithm>("merge_meshes");
    register_algorithm<viennamesh::line_coarsening::algorithm>("line_coarsening");
  }


  algorithm_factory_t & algorithm_factory()
  {
    static algorithm_factory_t factory_;
    return factory_;
  }
}
