#include "viennamesh_plugin.hpp"

#include "detection_2d.hpp"
#include "extract_slice_2d.hpp"
#include "recombine_slice_2d.hpp"

#include "detection_3d.hpp"


namespace viennamesh
{
  void plugin_init( context_handle & context )
  {
    context.register_algorithm<viennamesh::symmetry_detection_2d>();
    context.register_algorithm<viennamesh::extract_symmetric_slice_2d>();
    context.register_algorithm<viennamesh::recombine_symmetric_slice_2d>();

    context.register_algorithm<viennamesh::symmetry_detection_3d>();
  }
}
