#include "viennameshpp/plugin.hpp"

#include "affine_transform.hpp"
#include "extract_boundary.hpp"
#include "extract_plc_geometry.hpp"
#include "hyperplane_clip.hpp"
#include "laplace_smooth.hpp"
#include "hull_set_regions.hpp"
#include "merge_meshes.hpp"
#include "split_mesh.hpp"
#include "make_boundary_box.hpp"
#include "simplexify.hpp"
#include "line_coarsening.hpp"
#include "interpolate_quantities.hpp"
#include "map_regions.hpp"
#include "refine_plc_lines.hpp"
#include "center_mesh.hpp"
#include "mark_hull_regions.hpp"
#include "scale_quantities.hpp"
#include "stretch_middle.hpp"
#include "douglas_peucker_line_smoothing.hpp"
#include "uniform_refine.hpp"
#include "change_cell_region.hpp"


viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::affine_transform>(context);
  viennamesh::register_algorithm<viennamesh::extract_boundary>(context);
  viennamesh::register_algorithm<viennamesh::extract_plc_geometry>(context);
  viennamesh::register_algorithm<viennamesh::hyperplane_clip>(context);
  viennamesh::register_algorithm<viennamesh::laplace_smooth>(context);
  viennamesh::register_algorithm<viennamesh::hull_set_regions>(context);

  viennamesh::register_algorithm<viennamesh::merge_meshes>(context);
  viennamesh::register_algorithm<viennamesh::split_mesh>(context);
  viennamesh::register_algorithm<viennamesh::map_regions>(context);

  viennamesh::register_algorithm<viennamesh::make_boundary_box>(context);

  viennamesh::register_algorithm<viennamesh::simplexify>(context);
  viennamesh::register_algorithm<viennamesh::line_coarsening>(context);
  viennamesh::register_algorithm<viennamesh::interpolate_quantities>(context);
  viennamesh::register_algorithm<viennamesh::scale_quantities>(context);
  viennamesh::register_algorithm<viennamesh::refine_plc_lines>(context);
  viennamesh::register_algorithm<viennamesh::center_mesh>(context);

  viennamesh::register_algorithm<viennamesh::mark_hull_regions>(context);
  viennamesh::register_algorithm<viennamesh::stretch_middle>(context);
  viennamesh::register_algorithm<viennamesh::douglas_peucker_line_smoothing>(context);

  viennamesh::register_algorithm<viennamesh::uniform_refine>(context);
  viennamesh::register_algorithm<viennamesh::change_cell_region>(context);

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
//     context.register_algorithm<viennamesh::affine_transform>();
//     context.register_algorithm<viennamesh::extract_boundary>();
//     context.register_algorithm<viennamesh::extract_plc_geometry>();
//     context.register_algorithm<viennamesh::hyperplane_clip>();
//     context.register_algorithm<viennamesh::laplace_smooth>();
//     context.register_algorithm<viennamesh::hull_set_regions>();
//
//     context.register_algorithm<viennamesh::merge_meshes>();
//     context.register_algorithm<viennamesh::split_mesh>();
//     context.register_algorithm<viennamesh::map_regions>();
//
//     context.register_algorithm<viennamesh::make_boundary_box>();
//
//     context.register_algorithm<viennamesh::simplexify>();
//     context.register_algorithm<viennamesh::line_coarsening>();
//     context.register_algorithm<viennamesh::interpolate_quantities>();
//     context.register_algorithm<viennamesh::refine_plc_lines>();
//     context.register_algorithm<viennamesh::center_mesh>();
//   }
// }
