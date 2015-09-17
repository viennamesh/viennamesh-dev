#ifndef VIENNAMESH_ALGORITHM_TRIANGLE_MESH_HPP
#define VIENNAMESH_ALGORITHM_TRIANGLE_MESH_HPP

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
#include "viennagrid/algorithm/plane_to_2d_projector.hpp"


extern "C"
{
  #include "triangle_interface.h"
}



namespace viennamesh
{
  namespace triangle
  {
    void init_points(triangulateio & mesh, int num_points);
    void init_segments(triangulateio & mesh, int num_segments);
  }


  inline viennamesh_error make_triangle_mesh(viennamesh_data * data)
  { return make_viennamesh_data<triangle_mesh>(data, triangle_make_mesh); }
  inline viennamesh_error delete_triangle_mesh(viennamesh_data data)
  { return delete_viennamesh_data<triangle_mesh>(data, triangle_delete_mesh); }





  viennamesh_error convert(viennagrid::mesh const & input, triangulateio & output);
  viennamesh_error convert(triangulateio const & input, viennagrid::mesh & output);

  template<>
  viennamesh_error internal_convert<viennagrid_mesh, triangle_mesh>(viennagrid_mesh const & input, triangle_mesh & output);
  template<>
  viennamesh_error internal_convert<triangle_mesh, viennagrid_mesh>(triangle_mesh const & input, viennagrid_mesh & output);



  namespace result_of
  {
    template<>
    struct data_information<triangle_mesh>
    {
      static std::string type_name() { return "triangle_mesh"; }
      static viennamesh_data_make_function make_function() { return make_triangle_mesh; }
      static viennamesh_data_delete_function delete_function() { return delete_triangle_mesh; }
    };
  }

}

#endif
