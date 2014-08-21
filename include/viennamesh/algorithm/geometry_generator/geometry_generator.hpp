#ifndef VIENNAMESH_ALGORITHM_GEOMETRY_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_GEOMETRY_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class geometry_generator : public base_algorithm
  {
  public:
    geometry_generator();

    std::string name() const;
    std::string id() const;

    bool run_impl();

  private:
    required_input_parameter_interface<std::string>         filename;

    output_parameter_interface                              output_mesh;
    output_parameter_interface                              output_hole_points;
    output_parameter_interface                              output_seed_points;
  };
}

#endif
