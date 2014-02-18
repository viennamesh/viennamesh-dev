#ifndef VIENNAMESH_ALGORITHM_TETGEN_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_GENERATOR_HPP


#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/tetgen/mesh.hpp"


namespace viennamesh
{
  namespace tetgen
  {
    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "Tetgen 1.5 mesher"; }

      static sizing_function_3d sizing_function;
      static bool should_tetrahedron_be_refined(REAL*, REAL*, REAL*, REAL*, REAL*, REAL);

      void extract_seed_points( tetgen::input_segmentation const & segmentation, int num_hole_points, REAL * hole_points, seed_point_3d_container & seed_points );

      bool run_impl();

    private:
    };
  }

}



#endif
