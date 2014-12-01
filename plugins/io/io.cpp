#include "viennamesh/plugin.hpp"
#include "mesh_reader.hpp"
#include "mesh_writer.hpp"

int viennamesh_plugin_init(viennamesh_context ctx_)
{
  viennamesh::context_handle context(ctx_);

  viennamesh::register_algorithm<viennamesh::mesh_reader>(context);
  viennamesh::register_algorithm<viennamesh::mesh_writer>(context);

  return VIENNAMESH_SUCCESS;
}
