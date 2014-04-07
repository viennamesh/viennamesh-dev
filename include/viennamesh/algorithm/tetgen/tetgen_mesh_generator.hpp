#ifndef VIENNAMESH_ALGORITHM_TETGEN_MESH_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_MESH_GENERATOR_HPP


#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/tetgen/tetgen_mesh.hpp"


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

      static bool should_tetrahedron_be_refined(REAL*, REAL*, REAL*, REAL*, REAL*, REAL);

      void extract_seed_points( tetgen::input_segmentation const & segmentation, int num_hole_points, REAL * hole_points, seed_point_3d_container & seed_points );

      bool run_impl();

    private:
    };
  }

}



#endif
