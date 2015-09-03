#include "viennameshpp/plugin.hpp"
#include "poisson_reconstruct_surface.hpp"
#include "poisson_estimate_normals.hpp"
#include "scale_reconstruction.hpp"
#include "poisson_mesh.hpp"



viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{
  viennamesh::register_data_type<viennamesh::poisson::Vector>(context);

  viennamesh::register_algorithm<viennamesh::poisson::estimate_normals>(context);
  viennamesh::register_algorithm<viennamesh::poisson::scale_reconstruction>(context);
  viennamesh::register_algorithm<viennamesh::poisson::reconstruct_surface>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}
