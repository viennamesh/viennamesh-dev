#include "viennameshpp/plugin.hpp"
#include "cgal_simplify_mesh.hpp"
#include "cgal_mesh.hpp"



viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<viennamesh::cgal::mesh>(context);

  viennamesh::register_conversion<viennagrid_mesh, viennamesh::cgal::mesh>(context);
  viennamesh::register_conversion<viennamesh::cgal::mesh, viennagrid_mesh>(context);

  viennamesh::register_algorithm<viennamesh::cgal::simplify_mesh>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
