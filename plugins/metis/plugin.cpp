#include "viennamesh_plugin.hpp"

#include "mesh_partitioning.hpp"


namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    viennamesh::register_algorithm<viennamesh::metis_mesh_partitioning>(context);
  }
}
