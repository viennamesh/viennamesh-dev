#include "viennamesh_plugin.hpp"
#include "mesh_reader.hpp"
#include "mesh_writer.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_algorithm<viennamesh::mesh_reader>();
    context.register_algorithm<viennamesh::mesh_writer>();
  }
}
