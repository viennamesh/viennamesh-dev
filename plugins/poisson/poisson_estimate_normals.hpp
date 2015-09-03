#ifndef VIENNAMESH_ALGORITHM_POISSON_ESTIMATE_NORMALS_HPP
#define VIENNAMESH_ALGORITHM_POISSON_ESTIMATE_NORMALS_HPP

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
  namespace poisson
  {
    class estimate_normals : public plugin_algorithm
    {
    public:
      estimate_normals();

      static std::string name();
      bool run(viennamesh::algorithm_handle &);
    };
  }

}



#endif
