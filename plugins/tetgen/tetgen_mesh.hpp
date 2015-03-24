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
    typedef tetgenio mesh;

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



  viennamesh_error convert(viennagrid::mesh_t const & input, tetgen::mesh & output);
  viennamesh_error convert(tetgen::mesh const & input, viennagrid::mesh_t & output);

  template<>
  viennamesh_error internal_convert<viennagrid_mesh, tetgen::mesh>(viennagrid_mesh const & input, tetgen::mesh & output);
  template<>
  viennamesh_error internal_convert<tetgen::mesh, viennagrid_mesh>(tetgen::mesh const & input, viennagrid_mesh & output);

  namespace result_of
  {
    template<>
    struct data_information<tetgen::mesh>
    {
      static std::string type_name() { return "tetgen::mesh"; }
      static viennamesh_data_make_function make_function() { return viennamesh::generic_make<tetgen::mesh>; }
      static viennamesh_data_delete_function delete_function() { return viennamesh::generic_delete<tetgen::mesh>; }

    };
  }


}

#endif
