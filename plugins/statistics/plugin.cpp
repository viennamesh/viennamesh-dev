#include "viennameshpp/plugin.hpp"

#include "make_statistic.hpp"
#include "mesh_information.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::make_statistic>(context);
  viennamesh::register_algorithm<viennamesh::mesh_information>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}


