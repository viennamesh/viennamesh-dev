#ifndef VIENNAMESH_ALGORITHM_EXTRACT_PLC_GEOMETRY_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_PLC_GEOMETRY_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class extract_plc : public base_algorithm
  {
  public:
    extract_plc();

    string name() const;
    string id() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface    input_mesh;

    output_parameter_interface                    output_mesh;
  };
}

#endif
