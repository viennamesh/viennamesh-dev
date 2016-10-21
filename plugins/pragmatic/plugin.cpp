#include "viennameshpp/plugin.hpp"

#include "pragmatic_coarsen.hpp"
#include "pragmatic_refine.hpp"
#include "pragmatic_smooth.hpp"
#include "pragmatic_swapping.hpp"
#include "pragmatic_metis_partitioning.hpp"
#include "pragmatic_refine_multi_region.hpp"

viennamesh_error viennamesh_plugin_init(viennamesh_context context)
{  
  viennamesh::register_algorithm<viennamesh::pragmatic_coarsen>(context);
  viennamesh::register_algorithm<viennamesh::pragmatic_refine>(context);
  viennamesh::register_algorithm<viennamesh::pragmatic_smooth>(context);
  viennamesh::register_algorithm<viennamesh::pragmatic_swapping>(context);

  viennamesh::register_algorithm<viennamesh::pragmatic_metis_partitioner>(context);
  viennamesh::register_algorithm<viennamesh::pragmatic_refine_multi_region>(context);

  return VIENNAMESH_SUCCESS;
}

int viennamesh_version()
{
  return VIENNAMESH_VERSION;
}


