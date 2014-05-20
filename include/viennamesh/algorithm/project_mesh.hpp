#ifndef VIENNAMESH_ALGORITHM_VIENNAGRID_PROJECT_MESH_HPP
#define VIENNAMESH_ALGORITHM_VIENNAGRID_PROJECT_MESH_HPP

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
  class project_mesh : public base_algorithm
  {
  public:
    project_mesh();

    string name() const;
    string id() const;

    template<typename InputMeshT, typename InputSegmentationT, typename OutputMeshT, typename OutputSegmentationT>
    bool generic_run( int target_dimension );
    bool run_impl();

  private:
    dynamic_required_input_parameter_interface        input_mesh;
    required_input_parameter_interface<int>           target_dimension;

    output_parameter_interface                        output_mesh;
  };
}

#endif
