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

#include "viennameshpp/plugin.hpp"

namespace viennamesh
{
  class line_coarsening : public plugin_algorithm
  {
  public:
    line_coarsening();

    static std::string name();
    bool run(viennamesh::algorithm_handle &);
  };
}

#endif
