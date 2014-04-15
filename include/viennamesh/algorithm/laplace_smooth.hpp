#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class laplace_smooth : public base_algorithm
  {
  public:
    laplace_smooth();

    string name() const;
    string id() const;

    template<typename MeshT>
    bool generic_run();
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface  input_mesh;
    default_input_parameter_interface<double>   lambda;
    default_input_parameter_interface<double>   max_distance;

    output_parameter_interface                  output_mesh;
  };
}

#endif
