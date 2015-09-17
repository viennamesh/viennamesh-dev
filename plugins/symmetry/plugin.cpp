#include "viennameshpp/plugin.hpp"

#include "detection_2d.hpp"
#include "extract_slice_2d.hpp"
#include "recombine_slice_2d.hpp"

#include "detection_3d.hpp"
#include "extract_slice_3d.hpp"
#include "extract_plc_slice_3d.hpp"
#include "merge_slice_interface_3d.hpp"

#include "recombine_slice.hpp"




viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_algorithm<viennamesh::symmetry_detection_2d>(context);
  viennamesh::register_algorithm<viennamesh::extract_symmetric_slice_2d>(context);
  viennamesh::register_algorithm<viennamesh::recombine_symmetric_slice_2d>(context);

  viennamesh::register_algorithm<viennamesh::symmetry_detection_3d>(context);
  viennamesh::register_algorithm<viennamesh::extract_symmetric_slice_3d>(context);
  viennamesh::register_algorithm<viennamesh::extract_symmetric_plc_slice_3d>(context);
  viennamesh::register_algorithm<viennamesh::merge_symmetry_slice_interface_3d>(context);


  viennamesh::register_algorithm<viennamesh::recombine_symmetric_slice>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}


