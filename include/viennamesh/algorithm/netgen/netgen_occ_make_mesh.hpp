#ifndef VIENNAMESH_ALGORITHM_NETGEN_OCC_MAKE_MESH_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_OCC_MAKE_MESH_HPP

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

namespace viennamesh
{
  namespace netgen
  {
    class occ_make_mesh : public base_algorithm
    {
    public:
      occ_make_mesh();

      std::string name() const;
      std::string id() const;

      bool run_impl();

    private:
      required_input_parameter_interface<std::string>    filename;
      optional_input_parameter_interface<std::string>    filetype;
      optional_input_parameter_interface<double>         cell_size;
      default_input_parameter_interface<double>          curvature_safety_factor;
      default_input_parameter_interface<double>          segments_per_edge;
      default_input_parameter_interface<double, greater_check<double> >          grading;

      output_parameter_interface                         output_mesh;
    };
  }

}



#endif
