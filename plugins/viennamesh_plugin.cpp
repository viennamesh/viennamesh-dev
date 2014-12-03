#include "viennamesh_plugin.hpp"

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}

int viennamesh_plugin_init(viennamesh_context ctx_)
{
  viennamesh::context_handle context(ctx_);
  viennamesh::plugin_init(context);

  return VIENNAMESH_SUCCESS;
}
