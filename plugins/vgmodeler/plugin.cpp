#include "viennameshpp/plugin.hpp"

#include "vgmodeler_adapt_hull.hpp"


viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::vgmodeler::adapt_hull>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}

