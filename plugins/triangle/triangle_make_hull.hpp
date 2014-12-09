#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MAKE_HULL_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MAKE_HULL_HPP

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

#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/triangle/triangle_3d_mesh.hpp"

namespace viennamesh
{
  namespace triangle
  {
    class make_hull : public base_algorithm
    {
    public:
      make_hull();

      std::string name() const;
      std::string id() const;

      bool run_impl();

    private:
      typedef triangle::input_mesh_3d InputMeshType;
      required_input_parameter_interface<InputMeshType>             input_mesh;

      output_parameter_interface output_mesh;
    };
  }
}

#endif
