#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_HULL_MESH_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_HULL_MESH_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace triangle
  {
    class hull_mesh_generator : public base_algorithm
    {
    public:
      string name() const { return "Triangle 1.6 hull mesher"; }
      bool run_impl();
    };
  }
}

#endif
