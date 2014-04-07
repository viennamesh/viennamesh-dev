#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MESH_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MESH_GENERATOR_HPP

#include <cstring>
#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/triangle/triangle_mesh.hpp"

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

      void extract_seed_points( triangle::input_segmentation const & segmentation, int num_hole_points, REAL * hole_points, seed_point_2d_container & seed_points );

      bool run_impl();

    private:
    };

  }


}

#endif
