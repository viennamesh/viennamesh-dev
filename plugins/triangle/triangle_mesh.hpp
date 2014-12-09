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

#include "viennamesh_plugin.hpp"

extern "C"
{
  #include "triangle/triangle_interface.h"
}

namespace viennamesh
{

  namespace triangle
  {
    struct base_mesh_t
    {
      base_mesh_t()
      {
        triangle_mesh.pointlist = NULL;
        triangle_mesh.pointattributelist = NULL;
        triangle_mesh.pointmarkerlist = NULL;
        triangle_mesh.numberofpoints = 0;
        triangle_mesh.numberofpointattributes = 0;

        triangle_mesh.trianglelist = NULL;
        triangle_mesh.triangleattributelist = NULL;
        triangle_mesh.trianglearealist = NULL;
        triangle_mesh.neighborlist = NULL;
        triangle_mesh.numberoftriangles = 0;
        triangle_mesh.numberofcorners = 0;
        triangle_mesh.numberoftriangleattributes = 0;

        triangle_mesh.segmentlist = NULL;
        triangle_mesh.segmentmarkerlist = NULL;
        triangle_mesh.numberofsegments = 0;

        triangle_mesh.holelist = NULL;
        triangle_mesh.numberofholes = 0;

        triangle_mesh.regionlist = NULL;
        triangle_mesh.numberofregions = 0;

        triangle_mesh.edgelist = NULL;
        triangle_mesh.edgemarkerlist = NULL;
        triangle_mesh.normlist = NULL;
        triangle_mesh.numberofedges = 0;
      }

      ~base_mesh_t()
      {
        if (triangle_mesh.pointlist) free(triangle_mesh.pointlist);
        if (triangle_mesh.pointattributelist) free(triangle_mesh.pointattributelist);
        if (triangle_mesh.pointmarkerlist) free(triangle_mesh.pointmarkerlist);

        if (triangle_mesh.trianglelist) free(triangle_mesh.trianglelist);
        if (triangle_mesh.triangleattributelist) free(triangle_mesh.triangleattributelist);
        if (triangle_mesh.trianglearealist) free(triangle_mesh.trianglearealist);
        if (triangle_mesh.neighborlist) free(triangle_mesh.neighborlist);

        if (triangle_mesh.segmentlist) free(triangle_mesh.segmentlist);
        if (triangle_mesh.segmentmarkerlist) free(triangle_mesh.segmentmarkerlist);

        if (triangle_mesh.edgelist) free(triangle_mesh.edgelist);
        if (triangle_mesh.edgemarkerlist) free(triangle_mesh.edgemarkerlist);
        if (triangle_mesh.normlist) free(triangle_mesh.normlist);
      }

      void init_points(int num_points)
      {
        if (triangle_mesh.pointlist) free(triangle_mesh.pointlist);
        triangle_mesh.pointlist = (REAL*)malloc( sizeof(REAL) * 2 * num_points);
        triangle_mesh.numberofpoints = num_points;
      }

      void init_segments(int num_segments)
      {
        if (triangle_mesh.segmentlist) free(triangle_mesh.segmentlist);
        triangle_mesh.segmentlist = (int*)malloc( sizeof(int) * 2 * num_segments);
        triangle_mesh.numberofsegments = num_segments;
      }

      triangulateio triangle_mesh;
    };

    class input_mesh_t : public base_mesh_t
    {
    public:
      ~input_mesh_t()
      {
        if (triangle_mesh.holelist) free(triangle_mesh.holelist);
        if (triangle_mesh.regionlist) free(triangle_mesh.regionlist);
      }
    };

    class output_mesh_t : public base_mesh_t
    {};



    typedef input_mesh_t input_mesh;
    typedef output_mesh_t output_mesh;

  }

  int convert_to_triangle(viennamesh_data input_, viennamesh_data output_);
  int convert_from_triangle(viennamesh_data input_, viennamesh_data output_);

  namespace result_of
  {
    template<>
    struct data_information<triangle::input_mesh>
    {
      static std::string type_name() { return "triangle::input_mesh"; }
      static std::string local_binary_format() { return viennamesh::local_binary_format(); }
    };

    template<>
    struct data_information<triangle::output_mesh>
    {
      static std::string type_name() { return "triangle::output_mesh"; }
      static std::string local_binary_format() { return viennamesh::local_binary_format(); }
    };
  }

}

#endif
