#include "viennamesh_plugin.hpp"
#include "tdr_reader.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_algorithm<viennamesh::tdr_reader>();
  }
}
