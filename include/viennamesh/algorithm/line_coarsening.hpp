#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_COARSENING_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_COARSENING_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class line_coarsening : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Line Coarsing"; }

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();
  };
}

#endif
