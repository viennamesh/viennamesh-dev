#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_EXTRACT_SLICE_3D_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_EXTRACT_SLICE_3D_HPP

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
  class extract_symmetric_slice_3d : public plugin_algorithm
  {
  public:
    extract_symmetric_slice_3d();

    static std::string name();
    bool run(viennamesh::algorithm_handle &);

  private:

    mesh_handle cut(viennagrid::const_mesh const & mesh,
                    viennagrid::point const & N,
                    double tolerance);
  };
}

#endif
