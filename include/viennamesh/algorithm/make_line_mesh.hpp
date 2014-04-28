#ifndef VIENNAMESH_ALGORITHM_MAKE_LINE_MESH_HPP
#define VIENNAMESH_ALGORITHM_MAKE_LINE_MESH_HPP

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class make_line_mesh : public base_algorithm
  {
  public:
    make_line_mesh();

    string name() const;
    string id() const;

    template<typename GeometrySegmentationT>
    void extract_seed_points( GeometrySegmentationT const & segmentation, point_1d_container const & hole_points,
                              seed_point_1d_container & seed_points );

    template<typename GeometryT, typename GeometrySegmentationT, typename OutputMeshT, typename OutputSegmentationT>
    bool generic_run_impl();
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface                    input_mesh;
    optional_input_parameter_interface<seed_point_1d_container>   input_seed_points;
    optional_input_parameter_interface<point_1d_container>        input_hole_points;
    default_input_parameter_interface<double>                     cell_size;
    default_input_parameter_interface<bool>                       use_different_segment_ids_for_unknown_segments;
    default_input_parameter_interface<bool>                       extract_segment_seed_points;
    default_input_parameter_interface<double>                     relative_min_geometry_point_distance;
    optional_input_parameter_interface<double>                    absolute_min_geometry_point_distance;

    output_parameter_interface                                    output_mesh;
  };
}

#endif
