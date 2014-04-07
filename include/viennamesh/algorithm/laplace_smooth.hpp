#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class laplace_smooth : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Laplace Smoothing"; }

    template<typename MeshT>
    bool generic_run();
    bool run_impl();
  };
}

#endif
