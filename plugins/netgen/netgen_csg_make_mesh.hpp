#ifndef VIENNAMESH_ALGORITHM_NETGEN_CSG_MAKE_MESH_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_CSG_MAKE_MESH_HPP

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


#include "external/libsrc/csg/csg.hpp"
namespace netgen
{
  extern CSGeometry * ParseCSG (istream & istr);
}


namespace viennamesh
{

  namespace netgen
  {
    class csg_make_mesh : public plugin_algorithm
    {
    public:
      csg_make_mesh();

      static std::string name();
      bool run(viennamesh::algorithm_handle &);
    };
  }

}



#endif
