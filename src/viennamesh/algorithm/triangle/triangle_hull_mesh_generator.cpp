#ifdef VIENNAMESH_WITH_TRIANGLE

#include "viennamesh/algorithm/triangle/triangle_3d_mesh.hpp"
#include "viennamesh/algorithm/triangle/triangle_hull_mesh_generator.hpp"


namespace viennamesh
{
  namespace triangle
  {
    bool hull_mesh_generator::run_impl()
    {
      typedef triangle::input_mesh_3d InputMeshType;
      typedef triangle::output_mesh_3d OutputMeshType;

      viennamesh::result_of::const_parameter_handle<InputMeshType>::type input_mesh = get_required_input<InputMeshType>("default");
      output_parameter_proxy<OutputMeshType> output_mesh = output_proxy<OutputMeshType>("default");

      bool use_logger = true;
      copy_input( "use_logger", use_logger );

      std::ostringstream options;

      const_string_parameter_handle option_string = get_input<string>("option_string");
      if (option_string)
        options << option_string();
      else
        options << "zpQ";

      output_mesh().cells.resize( input_mesh().cells.size() );
      output_mesh().vertex_points_3d = input_mesh().vertex_points_3d;
      output_mesh().is_segmented = input_mesh().is_segmented;

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

        if (use_logger)
          std_capture().start();

        triangulate( buffer, &tmp, &output_mesh().cells[i].plc.triangle_mesh, NULL);
        output_mesh().cells[i].vertex_ids = input_mesh().cells[i].vertex_ids;
        output_mesh().cells[i].segment_ids = input_mesh().cells[i].segment_ids;

        if (use_logger)
          std_capture().start();

        delete[] buffer;
        if (!hole_points_2d.empty())
          free(tmp_holelist);
      }

      return true;
    }



  }
}


#endif
