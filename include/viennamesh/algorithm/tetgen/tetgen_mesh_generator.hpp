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

      static sizing_function_3d sizing_function;
      static bool using_sizing_function;

      static double max_edge_ratio;
      static bool using_max_edge_ratio;

      static double max_inscribed_radius_edge_ratio;
      static bool using_max_inscribed_radius_edge_ratio;

      static bool should_tetrahedron_be_refined(double*, double*, double*, double*, double*, double);

      bool run_impl();

    private:
    };
  }

}



#endif
