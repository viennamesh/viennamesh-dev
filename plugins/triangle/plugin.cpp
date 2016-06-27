#include "viennameshpp/plugin.hpp"
#include "triangle_mesh.hpp"
#include "triangle_make_mesh.hpp"
#include "triangle_make_hull.hpp"
#include "triangle_convert.hpp"



viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<triangle_mesh>(context);

  viennamesh::register_conversion<viennagrid_mesh, triangle_mesh>(context);
  viennamesh::register_conversion<triangle_mesh, viennagrid_mesh>(context);

  viennamesh::register_algorithm<viennamesh::triangle::make_mesh>(context);
  viennamesh::register_algorithm<viennamesh::triangle::make_hull>(context);
  viennamesh::register_algorithm<viennamesh::triangle_convert>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
