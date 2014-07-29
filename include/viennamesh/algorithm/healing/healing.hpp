#ifndef VIENNAMESH_ALGORITHM_HEALING_HPP
#define VIENNAMESH_ALGORITHM_HEALING_HPP

#include "viennagrid/mesh/mesh_operations.hpp"

namespace viennamesh
{
  template<typename MeshT, typename HealFunctorT>
  bool iteratively_heal(MeshT const & input_mesh, MeshT & output_mesh,
                        std::size_t max_iterations,
                        HealFunctorT heal_functor)
  {
    MeshT tmp;
    bool good;

    {
      LoggingStack stack( std::string("Healing iteration 1") );
      good = heal_functor(input_mesh, output_mesh);
    }
    std::size_t iteration_count = 1;

    MeshT * src = &output_mesh;
    MeshT * dst = &tmp;

    while (!good && iteration_count <= max_iterations)
    {
      ++iteration_count;
      LoggingStack stack( std::string("Healing iteration ") + lexical_cast<std::string>(iteration_count) );
      viennagrid::clear(*dst);
      good = heal_functor(*src, *dst);
      std::swap(src, dst);
    }

    if (good && dst != &output_mesh)
      viennagrid::copy(tmp, output_mesh);

    return good;
  }
}

#endif
