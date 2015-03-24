#include "viennamesh_plugin.hpp"
#include "triangle_mesh.hpp"
#include "triangle_make_mesh.hpp"
#include "triangle_make_hull.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_data_type<triangle_mesh>();

    context.register_conversion<viennagrid_mesh, triangle_mesh>();
    context.register_conversion<triangle_mesh, viennagrid_mesh>();

    context.register_algorithm<viennamesh::triangle::make_mesh>();
    context.register_algorithm<viennamesh::triangle::make_hull>();
  }
}
