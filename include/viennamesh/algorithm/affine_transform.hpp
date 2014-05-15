#ifndef VIENNAMESH_ALGORITHM_AFFINE_TRANSFORM_HPP
#define VIENNAMESH_ALGORITHM_AFFINE_TRANSFORM_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class affine_transform : public base_algorithm
  {
  public:
    affine_transform();

    std::string name() const;
    std::string id() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run( dynamic_point const & matrix, dynamic_point const & base_translate );
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface               input_mesh;
    required_input_parameter_interface<dynamic_point>        matrix;
    optional_input_parameter_interface<dynamic_point>        translate;

    output_parameter_interface                               output_mesh;
  };
}

#endif
