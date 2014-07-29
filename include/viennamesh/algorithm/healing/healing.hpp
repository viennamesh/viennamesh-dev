#ifndef VIENNAMESH_ALGORITHM_HEALING_HPP
#define VIENNAMESH_ALGORITHM_HEALING_HPP

#include "viennagrid/mesh/mesh_operations.hpp"

namespace viennamesh
{
  template<typename MeshT, typename HealFunctorT>
  void iteratively_heal(MeshT const & input_mesh, MeshT & output_mesh, HealFunctorT heal_functor)
  {
    MeshT tmp;
    bool good = heal_functor(input_mesh, output_mesh);

    MeshT * src = &output_mesh;
    MeshT * dst = &tmp;

    while (!good)
    {
      viennagrid::clear(*dst);
      good = heal_functor(*src, *dst);
      std::swap(src, dst);
    }

    if (dst != &output_mesh)
      viennagrid::copy(tmp, output_mesh);
  }
}

#endif
