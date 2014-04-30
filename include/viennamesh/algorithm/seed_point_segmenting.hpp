#ifndef VIENNAMESH_ALGORITHM_SEED_POINT_SEGMENTING_HPP
#define VIENNAMESH_ALGORITHM_SEED_POINT_SEGMENTING_HPP

/* =======================================================================
   Copyright (c) 2011-2014, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                     ViennaGrid - The Vienna Grid Library
                            -----------------

   License:      MIT (X11), see file LICENSE in the base directory
======================================================================= */

#include "viennamesh/core/algorithm.hpp"

namespace viennamesh
{
  class seed_point_segmenting : public base_algorithm
  {
  public:
    seed_point_segmenting();

    string name() const;
    string id() const;

    bool run_impl();

  private:
    dynamic_required_input_parameter_interface                    input_mesh;
    required_input_parameter_interface<seed_point_3d_container>   input_seed_points;

    output_parameter_interface                                    output_mesh;
  };
}



#endif
