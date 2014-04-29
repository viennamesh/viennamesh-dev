#ifndef VIENNAMESH_ALGORITHM_NETGEN_OCC_MAKE_MESH_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_OCC_MAKE_MESH_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  namespace netgen
  {
    class occ_make_mesh : public base_algorithm
    {
    public:
      occ_make_mesh();

      string name() const;
      string id() const;

      bool run_impl();

    private:
      required_input_parameter_interface<string>         filename;
      optional_input_parameter_interface<string>         filetype;
      optional_input_parameter_interface<double>         cell_size;
      default_input_parameter_interface<double>          curvature_safety_factor;
      default_input_parameter_interface<double>          segments_per_edge;
      default_input_parameter_interface<double>          grading;

      output_parameter_interface                         output_mesh;
    };
  }

}



#endif