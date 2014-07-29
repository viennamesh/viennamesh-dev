#ifndef VIENNAMESH_ALGORITHM_HEAL_MESH_HPP
#define VIENNAMESH_ALGORITHM_HEAL_MESH_HPP

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
  class heal_mesh : public base_algorithm
  {
  public:
    heal_mesh();

    std::string name() const;
    std::string id() const;

    bool run_impl();

  private:
    dynamic_required_input_parameter_interface    input_mesh;
    default_input_parameter_interface<double>     tolerance;
    default_input_parameter_interface<int>        max_heal_iteration_count;

    output_parameter_interface                    output_mesh;
  };
}

#endif
