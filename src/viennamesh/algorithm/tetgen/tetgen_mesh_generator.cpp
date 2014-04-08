#ifdef VIENNAMESH_WITH_TETGEN

#include "viennamesh/algorithm/tetgen/tetgen_mesh.hpp"
#include "viennamesh/algorithm/tetgen/tetgen_mesh_generator.hpp"

#include <cstring>
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"


namespace viennamesh
{
  namespace tetgen
  {
    viennamesh::sizing_function_3d mesh_generator::sizing_function;
    bool mesh_generator::using_sizing_function;

    double mesh_generator::max_edge_ratio;
    bool mesh_generator::using_max_edge_ratio;

    double mesh_generator::max_inscribed_radius_edge_ratio;
    bool mesh_generator::using_max_inscribed_radius_edge_ratio;

    bool mesh_generator::should_tetrahedron_be_refined(double * tet_p0, double * tet_p1, double * tet_p2, double * tet_p3, double * , double)
    {
      point_3d p0( tet_p0[0], tet_p0[1], tet_p0[2]);
      point_3d p1( tet_p1[0], tet_p1[1], tet_p1[2]);
      point_3d p2( tet_p2[0], tet_p2[1], tet_p2[2]);
      point_3d p3( tet_p3[0], tet_p3[1], tet_p3[2]);

      double d01 = viennagrid::distance(p0, p1);
      double d02 = viennagrid::distance(p0, p2);
      double d03 = viennagrid::distance(p0, p3);
      double d12 = viennagrid::distance(p1, p2);
      double d13 = viennagrid::distance(p1, p3);
      double d23 = viennagrid::distance(p2, p3);

      double max_len = std::max(std::max(std::max(d01, d02), std::max(d03, d12)), std::max(d13, d23));

      if (using_max_edge_ratio)
      {
        double min_len = std::min(std::min(std::min(d01, d02), std::min(d03, d12)), std::min(d13, d23));

        if (min_len / max_len < max_edge_ratio)
          return true;
      }


      if (using_max_inscribed_radius_edge_ratio)
      {
        // http://saketsaurabh.in/blog/2009/11/radius-of-a-sphere-inscribed-in-a-general-tetrahedron/
        double volume = viennagrid::spanned_volume( p0, p1, p2, p3 );
        double surface = viennagrid::spanned_volume( p0, p1, p2 ) + viennagrid::spanned_volume( p0, p1, p3 ) + viennagrid::spanned_volume( p0, p2, p3 ) + viennagrid::spanned_volume( p1, p2, p3 );
        double inscribed_sphere_radius = volume / (3.0 * surface);

        if (inscribed_sphere_radius / max_len < max_inscribed_radius_edge_ratio)
          return true;
      }



      if (using_sizing_function)
      {
        point_3d center = (p0+p1+p2+p3)/4.0;

        typedef viennagrid::static_array<double, 5> ContainerType;
        ContainerType local_sizes;

        local_sizes[0] = sizing_function( p0 );
        local_sizes[1] = sizing_function( p1 );
        local_sizes[2] = sizing_function( p2 );
        local_sizes[3] = sizing_function( p3 );
        local_sizes[4] = sizing_function(center);

        double local_size = -1;
        for (ContainerType::iterator it = local_sizes.begin(); it != local_sizes.end(); ++it)
        {
          if (*it > 0)
          {
            if (local_size < 0)
              local_size = *it;
            else
              local_size = std::min( local_size, *it );
          }
        }

        return max_len > local_size;
      }

      return false;
    }




    void extract_seed_points( tetgen::input_segmentation const & segmentation, int num_hole_points, REAL * hole_points, seed_point_3d_container & seed_points )
    {
      if (segmentation.segments.empty())
        return;

      LoggingStack stack( string("Extracting seed points from segments") );

      info(5) << "Extracting seed points from segments" << std::endl;

      tetgenbehavior options;
      options.zeroindex = 1;
      options.verbose = 0;
      options.quiet = 1;
      options.plc = 1;

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

        tetrahedralize(&options, &tmp, &tmp_mesh);

        viennagrid::tetrahedral_3d_mesh viennagrid_mesh;
        viennamesh::tetgen::convert( tmp_mesh, viennagrid_mesh );

        unsigned int i = seed_points.size();
        viennagrid::extract_seed_points( viennagrid_mesh, seed_points, highest_segment_id++ );
        for (; i < seed_points.size(); ++i)
          info(5) << "Found seed point: " << seed_points[i].first << std::endl;

        tmp.holelist = NULL;
        tmp.numberofholes = 0;
      }
    }


    bool mesh_generator::run_impl()
    {
      typedef viennagrid::segmented_mesh< tetgen::input_mesh, tetgen::input_segmentation > InputMeshType;
      typedef tetgen::output_mesh OutputMeshType;

      viennamesh::result_of::const_parameter_handle<InputMeshType>::type input_mesh = get_required_input<InputMeshType>("default");
      output_parameter_proxy<OutputMeshType> output_mesh = output_proxy<OutputMeshType>("default");

      bool use_logger = true;
      copy_input( "use_logger", use_logger );


      tetgenbehavior options;

      const_string_parameter_handle option_string = get_input<string>("option_string");
      if (option_string)
      {
        options.parse_commandline( const_cast<char*>(option_string().c_str()) );
      }
      else
      {
        options.zeroindex = 1;
        options.verbose = 1;

        options.vertexperblock = 1000000;
        options.tetrahedraperblock = 1000000;
        options.shellfaceperblock = 1000000;

        options.plc = 1;
      }

      const_double_parameter_handle cell_size = get_input<double>("cell_size");

//       const_parameter_handle sf = get_input("sizing_function");

      viennamesh::result_of::const_parameter_handle<sizing_function_3d>::type sf = get_input<sizing_function_3d>("sizing_function");

      const_double_parameter_handle max_radius_edge_ratio = get_input<double>("max_radius_edge_ratio");
      const_double_parameter_handle min_dihedral_angle = get_input<double>("min_dihedral_angle");
      const_double_parameter_handle max_edge_ratio = get_input<double>("max_edge_ratio");
      const_double_parameter_handle max_inscribed_radius_edge_ratio = get_input<double>("max_inscribed_radius_edge_ratio");

      using_sizing_function = false;
      using_max_edge_ratio = false;
      using_max_inscribed_radius_edge_ratio = false;


      tetgenio & tmp = (tetgenio&)input_mesh().mesh;

//       typedef viennagrid::segmented_mesh<viennagrid::thin_tetrahedral_3d_mesh, viennagrid::thin_tetrahedral_3d_mesh> OutputSegmentedMeshType;
//       viennamesh::result_of::parameter_handle<OutputSegmentedMeshType>::type simple_mesh = viennamesh::make_parameter<OutputSegmentedMeshType>();

      if (sf)
      {
//         if (sf->is_type<sizing_function_3d>())
//           sizing_function = sf();
//         else if (sf->is_type<std::string>())
//         {
//           const_string_parameter_handle sf_code = dynamic_handle_cast<const std::string>(sf);
//
//
//           viennamesh::algorithm_handle simple_mesher( new viennamesh::tetgen::algorithm() );
//           simple_mesher->set_output( "default", simple_mesh() );
//           simple_mesher->set_input( "default", input_mesh() );
//           simple_mesher->run();
//
//           pugi::xml_document xml_element_size;
//           xml_element_size.load( sf_code() );
//
//           function = viennamesh::sizing_function::from_xml(xml_element_size.first_child(), simple_mesh().mesh, simple_mesh().segmentation);
//
//           sizing_function = viennamesh::bind(viennamesh::sizing_function::get<viennamesh::sizing_function::base_sizing_function_3d>, function, _1);
//         }

        sizing_function = sf();
        using_sizing_function = true;
        options.use_refinement_callback = 1;
        tmp.tetunsuitable = should_tetrahedron_be_refined;

        info(1) << "Using sizing function" << std::endl;
      }
      else if (cell_size)
      {
        options.fixedvolume = 1;
        options.maxvolume = cell_size();
        info(1) << "Using global maximum cell size: " << cell_size() << std::endl;
      }

      if (max_radius_edge_ratio)
      {
        options.quality = 1;
        options.minratio = max_radius_edge_ratio();
        info(1) << "Using global maximum radius edge ratio: " << max_radius_edge_ratio() << std::endl;
      }

      if (min_dihedral_angle)
      {
        options.quality = 1;
        options.mindihedral = min_dihedral_angle();
        info(1) << "Using global minimum dihedral angle: " << min_dihedral_angle() << std::endl;
      }

      if (max_edge_ratio)
      {
        this->max_edge_ratio = max_edge_ratio();
        using_max_edge_ratio = true;
        options.use_refinement_callback = 1;
        tmp.tetunsuitable = should_tetrahedron_be_refined;
        info(1) << "Using global max edge ratio: " << max_edge_ratio() << std::endl;
      }

      if (max_inscribed_radius_edge_ratio)
      {
        this->max_inscribed_radius_edge_ratio = max_inscribed_radius_edge_ratio();
        using_max_inscribed_radius_edge_ratio = true;
        options.use_refinement_callback = 1;
        tmp.tetunsuitable = should_tetrahedron_be_refined;
        info(1) << "Using global max inscribed radius edge ratio: " << max_inscribed_radius_edge_ratio() << std::endl;
      }


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

        options.regionattrib = 1;
        info(1) << "Using seed points" << std::endl;
      }

//       info(1) << "Tetgen option string: \"" <<  options.str() << "\"" << std::endl;

      if (use_logger)
        std_capture().start();

      options.init();
      tetrahedralize(&options, &tmp, &output_mesh());

      if (use_logger)
        std_capture().finish();

      delete[] tmp.regionlist;
      delete[] tmp.holelist;

      tmp.numberofregions = old_numberofregions;
      tmp.regionlist = old_regionlist;

      tmp.numberofholes = old_numberofholes;
      tmp.holelist = old_holelist;

      if (sf)
        tmp.tetunsuitable = NULL;

      return true;
    }
  }
}

#endif
