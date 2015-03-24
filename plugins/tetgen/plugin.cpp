#include "viennamesh_plugin.hpp"
#include "tetgen_mesh.hpp"
#include "tetgen_make_mesh.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_data_type<tetgen::mesh>();

    context.register_conversion<viennagrid_mesh, tetgen::mesh>();
    context.register_conversion<tetgen::mesh, viennagrid_mesh>();

    context.register_algorithm<viennamesh::tetgen::make_mesh>();
  }
}
