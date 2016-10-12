#ifndef VIENNAMESH_ALGORITHM_CGAL_MESH_SIMPLIFICATION_HPP
#define VIENNAMESH_ALGORITHM_CGAL_MESH_SIMPLIFICATION_HPP

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
  namespace cgal
  {
    class cgal_mesh_simplification : public plugin_algorithm
    {
    public:
      cgal_mesh_simplification();

      static std::string name();
      bool run(viennamesh::algorithm_handle &);
    };
  }

}



#endif
