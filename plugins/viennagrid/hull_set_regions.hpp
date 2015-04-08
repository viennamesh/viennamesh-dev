#ifndef VIENNAMESH_ALGORITHM_HULL_SET_REGIONS_HPP
#define VIENNAMESH_ALGORITHM_HULL_SET_REGIONS_HPP

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
  class hull_set_regions : public plugin_algorithm
  {
  public:
    hull_set_regions();

    static std::string name();
    bool run(viennamesh::algorithm_handle &);
  };
}

#endif
