#ifdef VIENNAMESH_WITH_TRIANGLE

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

#include "viennamesh/algorithm/triangle/triangle_3d_mesh.hpp"
#include "viennamesh/algorithm/triangle/triangle_make_hull.hpp"


namespace viennamesh
{
  namespace triangle
  {
    make_hull::make_hull() :
      input_mesh(*this, parameter_information("mesh","mesh","The input mesh, triangle input_mesh_3d supported")),
      output_mesh(*this, parameter_information("mesh","mesh","The output mesh, triangle output_mesh_3d")) {}

    std::string make_hull::name() const { return "Triangle 1.6 hull mesher"; }
    std::string make_hull::id() const { return "triangle_make_hull"; }


    bool make_hull::run_impl()
    {
      typedef triangle::output_mesh_3d OutputMeshType;
      output_parameter_proxy<OutputMeshType> omp(output_mesh);

      std::ostringstream options;
      options << "zpQ";

      omp().cells.resize( input_mesh().cells.size() );
      omp().vertex_points_3d = input_mesh().vertex_points_3d;
      omp().is_segmented = input_mesh().is_segmented;

      for (unsigned int i = 0; i < input_mesh().cells.size(); ++i)
      {
        triangulateio tmp = input_mesh().cells[i].plc.triangle_mesh;
        REAL * tmp_holelist = NULL;

        std::vector<point_2d> const & hole_points_2d = input_mesh().cells[i].hole_points_2d;

        if (!hole_points_2d.empty())
        {
          tmp_holelist = (REAL*)malloc( 2*sizeof(REAL)*(tmp.numberofholes+hole_points_2d.size()) );
          memcpy( tmp_holelist, tmp.holelist, 2*sizeof(REAL)*tmp.numberofholes );

          for (std::size_t i = 0; i < hole_points_2d.size(); ++i)
          {
            tmp_holelist[2*(tmp.numberofholes+i)+0] = hole_points_2d[i][0];
            tmp_holelist[2*(tmp.numberofholes+i)+1] = hole_points_2d[i][1];
          }

          tmp.numberofholes += hole_points_2d.size();
          tmp.holelist = tmp_holelist;
        }

        char * buffer = new char[options.str().length()];
        std::strcpy(buffer, options.str().c_str());

        std_capture().start();
        triangulate( buffer, &tmp, &omp().cells[i].plc.triangle_mesh, NULL);
        std_capture().finish();

        omp().cells[i].vertex_ids = input_mesh().cells[i].vertex_ids;
        omp().cells[i].segment_ids = input_mesh().cells[i].segment_ids;

        delete[] buffer;
        if (!hole_points_2d.empty())
          free(tmp_holelist);
      }

      return true;
    }



  }
}


#endif
