#ifndef VIENNAMESH_ALGORITHM_TETGEN_GENERATOR_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_GENERATOR_HPP

#include <cstring>
#include "viennamesh/core/algorithm.hpp"
#include "viennamesh/algorithm/tetgen/mesh.hpp"
#include "viennamesh/algorithm/viennagrid/extract_seed_points.hpp"


namespace viennamesh
{
  namespace tetgen
  {
    class algorithm : public base_algorithm
    {
    public:

      string name() const { return "Tetgen 1.5 mesher"; }

      void extract_seed_points( tetgen::input_segmentation const & segmentation, int num_hole_points, REAL * hole_points,
                                seed_point_3d_container & seed_points )
      {
        if (segmentation.segments.empty())
          return;

        LoggingStack stack( string("Extracting seed points from segments") );

        info(5) << "Extracting seed points from segments" << std::endl;

        string options = "zpQ";

        int highest_segment_id = -1;
        for (seed_point_3d_container::iterator spit = seed_points.begin(); spit != seed_points.end(); ++spit)
          highest_segment_id = std::max( highest_segment_id, spit->second );
        ++highest_segment_id;

        for ( std::list<tetgen::input_mesh>::const_iterator sit = segmentation.segments.begin(); sit != segmentation.segments.end(); ++sit)
        {
          LoggingStack stack( string("Segment ") + lexical_cast<string>(highest_segment_id) );

          tetgenio & tmp = (tetgenio&)(*sit);
          tetgen::output_mesh tmp_mesh;

          if (hole_points)
          {
            tmp.numberofholes = num_hole_points;
            tmp.holelist = hole_points;
          }

          char * buffer = new char[options.length()+1];
          std::strcpy(buffer, options.c_str());

//           viennautils::StdCapture capture;
//           capture.start();

          tetrahedralize(buffer, &tmp, &tmp_mesh);

//           capture.finish();
//           info(5) << capture.get() << std::endl;


          viennagrid::tetrahedral_3d_mesh viennagrid_mesh;
          viennamesh::tetgen::convert( tmp_mesh, viennagrid_mesh );


          unsigned int i = seed_points.size();
          viennamesh::extract_seed_points::extract_seed_points( viennagrid_mesh, seed_points, highest_segment_id++ );
          for (; i < seed_points.size(); ++i)
            info(5) << "Found seed point: " << seed_points[i].first << std::endl;

          tmp.holelist = NULL;
          tmp.numberofholes = 0;
          delete[] buffer;
        }
      }


      bool run_impl()
      {
        typedef viennagrid::segmented_mesh< tetgen::input_mesh, tetgen::input_segmentation > InputMeshType;
        typedef tetgen::output_mesh OutputMeshType;

        viennamesh::result_of::const_parameter_handle<InputMeshType>::type input_mesh = get_required_input<InputMeshType>("default");
        output_parameter_proxy<OutputMeshType> output_mesh = output_proxy<OutputMeshType>("default");

        bool use_logger = true;
        copy_input( "use_logger", use_logger );


        std::ostringstream options;
//         options << "zpVqO4/7x10000000";

        const_string_parameter_handle option_string = get_input<string>("option_string");
        if (option_string)
          options << option_string();
        else
          options << "zpV";

        const_double_parameter_handle cell_size = get_input<double>("cell_size");
        if (cell_size)
          options << "a" << cell_size();

        const_double_parameter_handle max_radius_edge_ratio = get_input<double>("max_radius_edge_ratio");
        const_double_parameter_handle min_dihedral_angle = get_input<double>("min_dihedral_angle");

        if (max_radius_edge_ratio && min_dihedral_angle)
          options << "q" << max_radius_edge_ratio() << "/" << min_dihedral_angle() / M_PI * 180.0;
        else if (max_radius_edge_ratio)
          options << "q" << max_radius_edge_ratio();
        else if (min_dihedral_angle)
          options << "q/" << min_dihedral_angle() / M_PI * 180.0;


        tetgenio & tmp = (tetgenio&)input_mesh().mesh;

        int old_numberofregions = tmp.numberofregions;
        REAL * old_regionlist = tmp.regionlist;

        tmp.numberofregions = 0;
        tmp.regionlist = NULL;

        int old_numberofholes = tmp.numberofholes;
        REAL * old_holelist = tmp.holelist;

        tmp.numberofholes = 0;
        tmp.holelist = NULL;

        seed_point_3d_container seed_points;

        typedef viennamesh::result_of::const_parameter_handle<seed_point_3d_container>::type ConstSeedPointContainerHandle;
        ConstSeedPointContainerHandle seed_points_handle = get_input<seed_point_3d_container>("seed_points");
        if (seed_points_handle && !seed_points_handle().empty())
        {
          info(5) << "Found seed points" << std::endl;
          seed_points = seed_points_handle();
        }


        point_3d_container hole_points;

        typedef viennamesh::result_of::const_parameter_handle<point_3d_container>::type ConstPointContainerHandle;
        ConstPointContainerHandle hole_points_handle = get_input<point_3d_container>("hole_points");
        if (hole_points_handle && !hole_points_handle().empty())
        {
          info(5) << "Found hole points" << std::endl;
          hole_points = hole_points_handle();
        }

        if (!hole_points.empty())
        {
          REAL * tmp_holelist = new REAL[3 * (hole_points.size() + tmp.numberofholes)];
          memcpy( tmp_holelist, tmp.holelist, sizeof(REAL)*3*tmp.numberofholes );

          for (std::size_t i = 0; i < hole_points.size(); ++i)
          {
            tmp_holelist[3*(tmp.numberofholes+i)+0] = hole_points[i][0];
            tmp_holelist[3*(tmp.numberofholes+i)+1] = hole_points[i][1];
            tmp_holelist[3*(tmp.numberofholes+i)+2] = hole_points[i][2];
          }

          tmp.numberofholes += hole_points.size();
          tmp.holelist = tmp_holelist;
        }




        bool extract_segment_seed_points = true;
        copy_input( "extract_segment_seed_points", extract_segment_seed_points );
        if (extract_segment_seed_points)
        {
          extract_seed_points( input_mesh().segmentation, tmp.numberofholes, tmp.holelist, seed_points );
        }

        if (!seed_points.empty())
        {
          REAL * tmp_regionlist = new REAL[5 * (seed_points.size() + tmp.numberofregions)];
          memcpy( tmp_regionlist, tmp.regionlist, sizeof(REAL)*5*tmp.numberofregions );

          for (std::size_t i = 0; i < seed_points.size(); ++i)
          {
            tmp_regionlist[5*(tmp.numberofregions+i)+0] = seed_points[i].first[0];
            tmp_regionlist[5*(tmp.numberofregions+i)+1] = seed_points[i].first[1];
            tmp_regionlist[5*(tmp.numberofregions+i)+2] = seed_points[i].first[2];
            tmp_regionlist[5*(tmp.numberofregions+i)+3] = REAL(seed_points[i].second);
            tmp_regionlist[5*(tmp.numberofregions+i)+4] = 0;
          }

          tmp.numberofregions += seed_points.size();
          tmp.regionlist = tmp_regionlist;

          if (options.str().find('A') == string::npos)
            options << "A";
        }

        info(1) << "Tetgen option string: \"" <<  options.str() << "\"" << std::endl;

        char * buffer = new char[options.str().length()+1];
        std::strcpy(buffer, options.str().c_str());


        if (use_logger)
          std_capture().start();

        tetrahedralize(buffer, &tmp, &output_mesh());

        if (use_logger)
          std_capture().finish();

        delete[] buffer;
        delete[] tmp.regionlist;
        delete[] tmp.holelist;

        tmp.numberofregions = old_numberofregions;
        tmp.regionlist = old_regionlist;

        tmp.numberofholes = old_numberofholes;
        tmp.holelist = old_holelist;

        return true;
      }

    private:
    };
  }

}



#endif
