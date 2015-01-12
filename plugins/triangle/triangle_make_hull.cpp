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

#include "triangle_mesh.hpp"
#include "triangle_make_hull.hpp"


namespace viennamesh
{
  namespace triangle
  {
    make_hull::make_hull() {}

    std::string make_hull::name() { return "triangle_make_hull"; }

    bool make_hull::run(viennamesh::algorithm_handle &)
    {
      data_handle<triangle::input_mesh_3d> input_mesh = get_input<triangle::input_mesh_3d>("mesh");
      data_handle<triangle::output_mesh_3d> output_mesh = make_data<triangle::output_mesh_3d>();

      std::ostringstream options;
      options << "zpQ";

      output_mesh().cells.resize( input_mesh().cells.size() );
      output_mesh().vertex_points_3d = input_mesh().vertex_points_3d;
      output_mesh().is_segmented = input_mesh().is_segmented;

      for (unsigned int i = 0; i < input_mesh().cells.size(); ++i)
      {
        triangulateio tmp = input_mesh().cells[i].plc.triangle_mesh;
        REAL * tmp_holelist = NULL;

        std::vector<point_t> const & hole_points_2d = input_mesh().cells[i].hole_points_2d;

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

        char * buffer = new char[options.str().length()+1];
        std::strcpy(buffer, options.str().c_str());

        {
          StdCaptureHandle capture_handle;
          triangulate( buffer, &tmp, &output_mesh().cells[i].plc.triangle_mesh, NULL);
        }

        output_mesh().cells[i].vertex_ids = input_mesh().cells[i].vertex_ids;
        output_mesh().cells[i].segment_ids = input_mesh().cells[i].segment_ids;

        delete[] buffer;
        if (!hole_points_2d.empty())
          free(tmp_holelist);
      }

      set_output("mesh", output_mesh);

      return true;
    }



  }
}

