#ifndef VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_MESH_HPP

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

#include <list>
#include "viennamesh_plugin.hpp"

#ifndef TETLIBRARY
  #define TETLIBRARY
#endif
#include "tetgen.h"


namespace viennamesh
{
  namespace tetgen
  {

    class input_mesh : public tetgenio
    {};

    class output_mesh : public tetgenio
    {};

    template<typename PointContainerT>
    void set_hole_points( tetgenio::facet & facet, PointContainerT const & hole_points )
    {
      facet.numberofholes = hole_points.size();
      if (facet.numberofholes > 0)
      {
        facet.holelist = new REAL[ 3 * facet.numberofholes ];
        for (int hole_point_index = 0; hole_point_index != facet.numberofholes; ++hole_point_index)
        {
          facet.holelist[3*hole_point_index+0] = hole_points[hole_point_index][0];
          facet.holelist[3*hole_point_index+1] = hole_points[hole_point_index][1];
          facet.holelist[3*hole_point_index+2] = hole_points[hole_point_index][2];
        }
      }
      else
        facet.holelist = 0;
    }

    inline void set_hole_points( tetgenio::facet & facet, viennagrid::null_type const & )
    {
      facet.holelist = 0;
      facet.numberofholes = 0;
    }
  }





  int convert_to_tetgen(viennamesh_data input_, viennamesh_data output_);
  int convert_from_tetgen(viennamesh_data input_, viennamesh_data output_);


  namespace result_of
  {
    template<>
    struct data_information<tetgen::input_mesh>
    {
      static std::string type_name() { return "tetgen::input_mesh"; }
      static std::string local_binary_format() { return viennamesh::local_binary_format(); }
    };

    template<>
    struct data_information<tetgen::output_mesh>
    {
      static std::string type_name() { return "tetgen::output_mesh"; }
      static std::string local_binary_format() { return viennamesh::local_binary_format(); }
    };
  }


}

#endif
