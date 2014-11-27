#ifndef VIENNAMESH_ALGORITHM_REMOVE_OVERLAPPING_SURFACES_HPP
#define VIENNAMESH_ALGORITHM_REMOVE_OVERLAPPING_SURFACES_HPP

/* ============================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaMesh - The Vienna Meshing Framework
                            -----------------

                    http://viennamesh.sourceforge.net/

   License:         MIT (X11), see file LICENSE in the base directory
=============================================================================== */

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class remove_overlapping_surfaces : public base_algorithm
  {
  public:
    remove_overlapping_surfaces();

    std::string name() const;
    std::string id() const;

    bool run_impl();

  private:
    dynamic_required_input_parameter_interface    input_mesh;
    default_input_parameter_interface<double>     normal_difference_tolerance;
    default_input_parameter_interface<double>     max_distance;
    default_input_parameter_interface<int>        distance_point_count;

    output_parameter_interface                    output_mesh;
    output_parameter_interface                    output_segmented_mesh;
  };
}

#endif
