#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class hyperplane_clip : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Hyperplane Clip"; }

    template<typename MeshT, typename SegmentationT>
    bool generic_run( dynamic_point const & base_hyperplane_point, dynamic_point const & base_hyperplane_normal );
    bool run_impl();
  };
}

#endif
