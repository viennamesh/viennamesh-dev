#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_MAP_SEGMENTS_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_MAP_SEGMENTS_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class map_segments : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid Map Segments"; }

    template<typename MeshT, typename SegmentationT>
    bool generic_run( std::map<int, int> const & segment_mapping );
    bool run_impl();
  };
}

#endif
