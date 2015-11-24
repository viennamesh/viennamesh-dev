#include "viennameshpp/plugin.hpp"
#include "netgen_mesh.hpp"
#include "netgen_make_mesh.hpp"
#include "netgen_csg_make_mesh.hpp"



viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<viennamesh::netgen::mesh>(context);

  viennamesh::register_conversion<viennagrid_mesh, viennamesh::netgen::mesh>(context);
  viennamesh::register_conversion<viennamesh::netgen::mesh, viennagrid_mesh>(context);

  viennamesh::register_algorithm<viennamesh::netgen::make_mesh>(context);
  viennamesh::register_algorithm<viennamesh::netgen::csg_make_mesh>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
