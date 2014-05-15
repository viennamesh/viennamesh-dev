#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_DIRECTIONAL_CLIP_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class hyperplane_clip : public base_algorithm
  {
  public:
    hyperplane_clip();

    std::string name() const;
    std::string id() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run( dynamic_point const & base_hyperplane_point, dynamic_point const & base_hyperplane_normal );
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface            input_mesh;
    required_input_parameter_interface<dynamic_point>     hyperplane_point;
    required_input_parameter_interface<dynamic_point>     hyperplane_normal;

    output_parameter_interface                            output_mesh;
  };
}

#endif
