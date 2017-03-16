#ifndef VIENNAMESH_ALGORITHM_VTK_SIMPLIFY_MESH_HPP
#define VIENNAMESH_ALGORITHM_VTK_SIMPLIFY_MESH_HPP

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
    namespace vtk
    {
        class simplify_mesh :
                public plugin_algorithm
        {
            public:
                simplify_mesh();

                static std::string name() { return "vtk_simplify_mesh"; }
                bool run(viennamesh::algorithm_handle&);
        };
    }
}


#endif
