#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MESH_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MESH_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace triangle
  {
    class mesh_generator : public base_algorithm
    {
    public:

      string name() const { return "Triangle 1.6 mesher"; }

      static sizing_function_2d sizing_function;
      static int should_triangle_be_refined(double * triorg, double * tridest, double * triapex, double);

      bool run_impl();
    };
  }
}

#endif
