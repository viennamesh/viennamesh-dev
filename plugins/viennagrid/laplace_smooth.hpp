#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_LAPLACE_SMOOTH_HPP

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
  class laplace_smooth : public plugin_algorithm
  {
  public:
    laplace_smooth();

    static std::string name();
    bool run(viennamesh::algorithm_handle &);
  };
}

#endif
