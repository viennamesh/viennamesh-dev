#include "viennameshpp/plugin.hpp"

#include "mesh_partitioning.hpp"


viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::metis_mesh_partitioning>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
