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
    viennagrid::copy(input_mesh, output_mesh);


    MeshT * src = &output_mesh;
    MeshT * dst = &tmp;

    std::size_t iteration_count = 1;
    std::size_t bads;

    while ( (bads = heal_functor(*src)) > 0 && iteration_count <= max_iterations)
    {
      LoggingStack stack( std::string("Healing iteration ") + lexical_cast<std::string>(iteration_count) );
      info(1) << "Healing " << bads << " bads" << std::endl;
      ++iteration_count;
      heal_functor(*src, *dst);
      std::swap(src, dst);
    }

    if (src != &output_mesh)
      viennagrid::copy(tmp, output_mesh);

    return iteration_count <= max_iterations || heal_functor(*src);
  }
}

#endif
