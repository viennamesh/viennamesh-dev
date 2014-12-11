#include "viennamesh_plugin.hpp"

#include "affine_transform.hpp"
#include "extract_boundary.hpp"
#include "extract_plc_geometry.hpp"
#include "hyperplane_clip.hpp"
#include "laplace_smooth.hpp"

namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    viennamesh::register_algorithm<viennamesh::affine_transform>(context);
    viennamesh::register_algorithm<viennamesh::extract_boundary>(context);
    viennamesh::register_algorithm<viennamesh::extract_plc_geometry>(context);
    viennamesh::register_algorithm<viennamesh::hyperplane_clip>(context);
    viennamesh::register_algorithm<viennamesh::laplace_smooth>(context);
  }
}
