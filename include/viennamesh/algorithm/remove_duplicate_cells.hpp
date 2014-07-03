#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_REMOVE_DUPLICATE_CELLS_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_REMOVE_DUPLICATE_CELLS_HPP

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
  class remove_duplicate_cells : public base_algorithm
  {
  public:
    remove_duplicate_cells();

    std::string name() const;
    std::string id() const;

    template<typename MeshT, typename SegmentationT>
    bool generic_run();
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface                                  input_mesh;

    output_parameter_interface                                                  output_mesh;
  };
}


#endif
