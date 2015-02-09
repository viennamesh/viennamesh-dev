#include "viennamesh_plugin.hpp"

#include "remove_degenerate_cells.hpp"
#include "volumetric_resample.hpp"


namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    viennamesh::register_algorithm<viennamesh::remove_degenerate_cells>(context);

    viennamesh::register_algorithm<viennamesh::volumetric_resample>(context);
  }
}
