#include "viennameshpp/plugin.hpp"

#include "remove_degenerate_cells.hpp"
#include "volumetric_resample.hpp"
#include "multi_material_marching_cubes.hpp"



viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::remove_degenerate_cells>(context);
  viennamesh::register_algorithm<viennamesh::volumetric_resample>(context);
  viennamesh::register_algorithm<viennamesh::multi_material_marching_cubes>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}



// namespace viennamesh
// {
//   void plugin_init( context_handle & context )
//   {
//     context.register_algorithm<viennamesh::remove_degenerate_cells>();
//     context.register_algorithm<viennamesh::volumetric_resample>();
//     context.register_algorithm<viennamesh::multi_material_marching_cubes>();
//   }
// }
