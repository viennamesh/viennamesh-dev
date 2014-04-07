#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_PROJECT_MESH_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_PROJECT_MESH_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class project_mesh : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Project"; }

    template<typename InputMeshT, typename InputSegmentationT, typename OutputMeshT, typename OutputSegmentationT>
    bool generic_run( int target_dimension );
    bool run_impl();
  };
}

#endif
