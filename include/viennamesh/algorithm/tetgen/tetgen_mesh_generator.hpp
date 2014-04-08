#ifndef VIENNAMESH_ALGORITHM_TETGEN_MESH_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_MESH_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace tetgen
  {
    class mesh_generator : public base_algorithm
    {
    public:
      string name() const { return "Tetgen 1.5 mesher"; }
      bool run_impl();
    };
  }

}



#endif
