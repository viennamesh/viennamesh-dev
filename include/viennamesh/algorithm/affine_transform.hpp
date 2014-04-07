#ifndef VIENNAMESH_ALGORITHM_AFFINE_TRANSFORM_HPP
#define VIENNAMESH_ALGORITHM_AFFINE_TRANSFORM_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class affine_transform : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Affine Transform"; }

    template<typename MeshT, typename SegmentationT>
    bool generic_run( dynamic_point const & matrix, dynamic_point const & base_translate );
    bool run_impl();
  };
}

#endif
