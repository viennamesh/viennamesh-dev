#include "viennamesh_plugin.hpp"

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

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    viennamesh::register_algorithm<viennamesh::affine_transform>(context);
    viennamesh::register_algorithm<viennamesh::extract_boundary>(context);
    viennamesh::register_algorithm<viennamesh::extract_plc_geometry>(context);
    viennamesh::register_algorithm<viennamesh::hyperplane_clip>(context);
    viennamesh::register_algorithm<viennamesh::laplace_smooth>(context);
    viennamesh::register_algorithm<viennamesh::hull_set_regions>(context);

    viennamesh::register_algorithm<viennamesh::merge_meshes>(context);
    viennamesh::register_algorithm<viennamesh::split_mesh>(context);

    viennamesh::register_algorithm<viennamesh::make_boundary_box>(context);

    viennamesh::register_algorithm<viennamesh::simplexify>(context);
    viennamesh::register_algorithm<viennamesh::line_coarsening>(context);
    viennamesh::register_algorithm<viennamesh::interpolate_quantities>(context);
  }
}
