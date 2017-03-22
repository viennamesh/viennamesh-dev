#include "viennameshpp/plugin.hpp"
#include "cgal_mesh.hpp"
#include "cgal_mesh_simplification.hpp"
#include "cgal_automatic_mesh_simplification.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<viennamesh::cgal::polyhedron_surface_mesh>(context); //CGAL's triangulated surface mesh data type

  viennamesh::register_conversion<viennagrid_mesh, viennamesh::cgal::polyhedron_surface_mesh>(context); //viennagrid datastructure (DS) --> CGAL DS
  viennamesh::register_conversion<viennamesh::cgal::polyhedron_surface_mesh, viennagrid_mesh>(context); // CGAL DS --> viennagrid DS

  viennamesh::register_algorithm<viennamesh::cgal::cgal_mesh_simplification>(context); //mesh simplification with user defined policies and parameters
  viennamesh::register_algorithm<viennamesh::cgal::cgal_automatic_mesh_simplification>(context); //mesh simplifiaction with automatically chosen policies and parameters

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
