#ifndef VIENNAMESH_ALGORITHM_LINE_MESH_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_LINE_MESH_GENERATOR_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class line_mesh_generator : public base_algorithm
  {
  public:
    string name() const { return "ViennaGrid 1D line mesher"; }

    template<typename GeometrySegmentationT>
    void extract_seed_points( GeometrySegmentationT const & segmentation, point_1d_container const & hole_points,
                              seed_point_1d_container & seed_points );

    template<typename GeometryT, typename GeometrySegmentationT, typename OutputMeshT, typename OutputSegmentationT>
    bool generic_run_impl();
    bool run_impl();
  };
}

#endif
