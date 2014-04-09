#ifndef VIENNAMESH_ALGORITHM_EXTRACT_PLC_GEOMETRY_HPP
#define VIENNAMESH_ALGORITHM_EXTRACT_PLC_GEOMETRY_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class extract_plc : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Extract PLC geometry"; }

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();
  };
}

#endif
