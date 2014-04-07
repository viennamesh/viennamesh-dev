#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HULL_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_EXTRACT_HULL_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class extract_hull : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Extract Hull"; }

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();
  };
}

#endif
