#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_MAP_SEGMENTS_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_MAP_SEGMENTS_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class map_segments : public base_algorithm
  {
  public:
    map_segments();

    std::string name() const;
    std::string id() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run( std::map<std::string, std::string> const & segment_mapping );
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface                                  input_mesh;
    required_input_parameter_interface< std::map<std::string, std::string> >    segment_mapping;

    output_parameter_interface                                                  output_mesh;
  };
}

#endif
