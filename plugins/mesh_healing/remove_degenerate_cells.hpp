#ifndef VIENNAMESH_ALGORITHM_MESH_HEALING_REMOVE_DEGENERATE_CELLS_HPP
#define VIENNAMESH_ALGORITHM_MESH_HEALING_REMOVE_DEGENERATE_CELLS_HPP

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

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
  class remove_degenerate_cells : public plugin_algorithm
  {
  public:
    remove_degenerate_cells();

    static std::string name();
    bool run(viennamesh::algorithm_handle &);
  };
}

#endif
