#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_MERGE_MESHES_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_MERGE_MESHES_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class merge_meshes : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Merge Meshes"; }

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();
  };
}

#endif
