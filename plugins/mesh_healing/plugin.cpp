#include "viennamesh_plugin.hpp"

#include "remove_degenerate_cells.hpp"
#include "volumetric_resample.hpp"
#include "multi_material_marching_cubes.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_algorithm<viennamesh::remove_degenerate_cells>();
    context.register_algorithm<viennamesh::volumetric_resample>();
    context.register_algorithm<viennamesh::multi_material_marching_cubes>();
  }
}
