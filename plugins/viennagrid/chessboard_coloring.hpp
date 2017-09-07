#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_CHESSBOARD_COLORING_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_CHESSBOARD_COLORING_HPP

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
  class chessboard_coloring : public plugin_algorithm
  {
  public:
    chessboard_coloring();

    static std::string name();
    bool run(viennamesh::algorithm_handle &);
  };
}

#endif