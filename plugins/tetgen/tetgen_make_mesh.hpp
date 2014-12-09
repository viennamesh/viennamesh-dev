#ifndef VIENNAMESH_ALGORITHM_TETGEN_MAKE_MESH_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_MAKE_MESH_HPP

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

#include "viennamesh_plugin.hpp"

namespace viennamesh
{
  namespace tetgen
  {
    class make_mesh : public plugin_algorithm
    {
    public:
      make_mesh();

      static std::string name();
      bool run(viennamesh::algorithm_handle &);

//     private:
//       typedef viennagrid::segmented_mesh<tetgen::input_mesh, tetgen::input_segmentation> InputMeshType;
//       required_input_parameter_interface<InputMeshType>             input_mesh;
//       optional_input_parameter_interface<seed_point_3d_container>   input_seed_points;
//       optional_input_parameter_interface<point_3d_container>        input_hole_points;
//       dynamic_optional_input_parameter_interface                    sizing_function;
//       optional_input_parameter_interface<double>                    cell_size;
//       optional_input_parameter_interface<double>                    max_radius_edge_ratio;
//       optional_input_parameter_interface<double>                    min_dihedral_angle;
//       optional_input_parameter_interface<double>                    max_edge_ratio;
//       optional_input_parameter_interface<double>                    max_inscribed_radius_edge_ratio;
//       default_input_parameter_interface<bool>                       extract_segment_seed_points;
//       optional_input_parameter_interface<std::string>               option_string;
//
//       output_parameter_interface                                    output_mesh;
    };
  }

}



#endif
