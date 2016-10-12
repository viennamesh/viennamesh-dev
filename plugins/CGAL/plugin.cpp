#include "viennameshpp/plugin.hpp"
#include "cgal_mesh.hpp"
#include "cgal_mesh_simplification.hpp"


viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<viennamesh::cgal::polyhedron_surface_mesh>(context);
  
  viennamesh::register_conversion<viennagrid_mesh, viennamesh::cgal::polyhedron_surface_mesh>(context);
  viennamesh::register_conversion<viennamesh::cgal::polyhedron_surface_mesh, viennagrid_mesh>(context);
  
  viennamesh::register_algorithm<viennamesh::cgal::cgal_mesh_simplification>(context);
  
  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
