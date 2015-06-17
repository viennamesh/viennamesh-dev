#include "viennameshpp/plugin.hpp"

#include "remove_degenerate_cells.hpp"
#include "volumetric_resample.hpp"
#include "multi_material_marching_cubes.hpp"

#include "merge_close_points.hpp"
#include "check_hull_topology.hpp"


viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::remove_degenerate_cells>(context);
  viennamesh::register_algorithm<viennamesh::volumetric_resample>(context);
  viennamesh::register_algorithm<viennamesh::multi_material_marching_cubes>(context);

  viennamesh::register_algorithm<viennamesh::merge_close_points>(context);
  viennamesh::register_algorithm<viennamesh::check_hull_topology>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}


