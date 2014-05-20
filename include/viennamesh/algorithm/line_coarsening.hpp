#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_COARSENING_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LINE_COARSENING_HPP

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
  class line_coarsening : public base_algorithm
  {
  public:
    line_coarsening();

    std::string name() const;
    std::string id() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface      input_mesh;
    default_input_parameter_interface<double>       angle;

    output_parameter_interface                      output_mesh;
  };
}

#endif
