#ifdef VIENNAMESH_WITH_TETGEN

#include "viennamesh/algorithm/tetgen/tetgen_mesh.hpp"
#include "viennamesh/algorithm/tetgen/tetgen_make_mesh.hpp"

#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"
#include "viennamesh/core/sizing_function.hpp"


namespace viennamesh
{
  namespace tetgen
  {
    viennamesh::sizing_function_3d tetgen_sizing_function;
    bool using_sizing_function;

    double max_edge_ratio;
    bool using_max_edge_ratio;

    double max_inscribed_radius_edge_ratio;
    bool using_max_inscribed_radius_edge_ratio;

    bool should_tetrahedron_be_refined_function(double * tet_p0, double * tet_p1, double * tet_p2, double * tet_p3, double * , double)
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

        local_sizes[0] = tetgen_sizing_function( p0 );
        local_sizes[1] = tetgen_sizing_function( p1 );
        local_sizes[2] = tetgen_sizing_function( p2 );
        local_sizes[3] = tetgen_sizing_function( p3 );
        local_sizes[4] = tetgen_sizing_function(center);

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






    void make_mesh_impl(tetgen::input_mesh const & input,
                   tetgen::output_mesh & output,
                   point_3d_container const & hole_points,
                   seed_point_3d_container const & seed_points,
                   tetgenbehavior options)
    {
      tetgenio & tmp = (tetgenio&)(input);

      int old_numberofregions = tmp.numberofregions;
      REAL * old_regionlist = tmp.regionlist;

      tmp.numberofregions = 0;
      tmp.regionlist = NULL;

      int old_numberofholes = tmp.numberofholes;
      REAL * old_holelist = tmp.holelist;

      tmp.numberofholes = 0;
      tmp.holelist = NULL;

      if (!hole_points.empty())
      {
        tmp.numberofholes = hole_points.size();
        tmp.holelist = new REAL[3 * hole_points.size()];

        for (std::size_t i = 0; i < hole_points.size(); ++i)
        {
          tmp.holelist[3*i+0] = hole_points[i][0];
          tmp.holelist[3*i+1] = hole_points[i][1];
          tmp.holelist[3*i+2] = hole_points[i][2];
        }
      }

      if (!seed_points.empty())
      {
        tmp.numberofregions = seed_points.size();
        tmp.regionlist = new REAL[5 * seed_points.size()];

        for (std::size_t i = 0; i < seed_points.size(); ++i)
        {
          tmp.regionlist[5*i+0] = seed_points[i].first[0];
          tmp.regionlist[5*i+1] = seed_points[i].first[1];
          tmp.regionlist[5*i+2] = seed_points[i].first[2];
          tmp.regionlist[5*i+3] = REAL(seed_points[i].second);
          tmp.regionlist[5*i+4] = 0;
        }

        options.regionattrib = 1;
        info(1) << "Using seed points" << std::endl;
      }

      std_capture().start();
      options.init();
      tetrahedralize(&options, &tmp, &output);
      std_capture().finish();

      delete[] tmp.regionlist;
      delete[] tmp.holelist;

      tmp.numberofregions = old_numberofregions;
      tmp.regionlist = old_regionlist;

      tmp.numberofholes = old_numberofholes;
      tmp.holelist = old_holelist;
    }





    void extract_seed_points( tetgen::input_segmentation const & segmentation,
                              point_3d_container const & hole_points,
                              seed_point_3d_container & seed_points )
    {
      if (segmentation.segments.empty())
        return;

      LoggingStack stack( std::string("Extracting seed points from segments") );

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
        tetgen::output_mesh tmp_mesh;

        LoggingStack stack( std::string("Segment ") + lexical_cast<std::string>(highest_segment_id) );
        make_mesh_impl(*sit, tmp_mesh, hole_points, seed_point_3d_container(), options);

        viennagrid::tetrahedral_3d_mesh viennagrid_mesh;
        viennamesh::tetgen::convert( tmp_mesh, viennagrid_mesh );

        std::vector<point_3d> local_seed_points;
        viennagrid::extract_seed_points( viennagrid_mesh, local_seed_points );
        for (unsigned int i = 0; i < local_seed_points.size(); ++i)
        {
          info(5) << "Found seed point: " << local_seed_points[i] << std::endl;
          seed_points.push_back( std::make_pair(local_seed_points[i], highest_segment_id) );
        }
        highest_segment_id++;
      }
    }

    template<typename SizingFunctionRepresentationT>
    sizing_function_3d make_sizing_function(tetgen::input_mesh const & mesh,
                                            point_3d_container const & hole_points,
                                            seed_point_3d_container const & seed_points,
                                            SizingFunctionRepresentationT const & sf,
                                            std::string const & base_path)
    {
      typedef viennagrid::tetrahedral_3d_mesh MeshType;
      typedef viennagrid::tetrahedral_3d_segmentation SegmentationType;

      typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
      viennamesh::result_of::parameter_handle<SegmentedMeshType>::type simple_mesh = viennamesh::make_parameter<SegmentedMeshType>();

      tetgenbehavior options;
      options.zeroindex = 1;
      options.verbose = 0;
      options.quiet = 1;
      options.plc = 1;
      tetgen::output_mesh tmp_mesh;

      make_mesh_impl(mesh, tmp_mesh, hole_points, seed_points, options);
      viennamesh::tetgen::convert( tmp_mesh, simple_mesh() );

      return viennamesh::sizing_function::from_xml(sf, simple_mesh, base_path);
    }



    make_mesh::make_mesh() :
      input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented tetgen mesh supported")),
      input_seed_points(*this, parameter_information("seed_points","seed_point_3d_container","A container of seed points with corresonding segment names")),
      input_hole_points(*this, parameter_information("hole_points","point_3d_container","A container of hole points")),
      sizing_function(*this, parameter_information("sizing_function","sizing_function_3d|string|pugi::xml_node","The sizing function, different representation are supported: arbitrary sizing_function_3d or the ViennaMesh sizing function framework in string or pugi::xml_node representation")),
      cell_size(*this, parameter_information("cell_size","double","The desired maximum size of tetrahedrons, all tetrahedrons will be at most this size")),
      max_radius_edge_ratio(*this, parameter_information("max_radius_edge_ratio","double","Desired maximum ratio of the circumradius and longest edge")),
      min_dihedral_angle(*this, parameter_information("min_dihedral_angle","double","Desired minimum dihedral angle")),
      max_edge_ratio(*this, parameter_information("max_edge_ratio","double","Desired maximum ratio of the shortest edge and the longest edge")),
      max_inscribed_radius_edge_ratio(*this, parameter_information("max_inscribed_radius_edge_ratio","double","Desired maximum ratio of the inscribed sphere radius and the longest edge")),
      extract_segment_seed_points(*this, parameter_information("extract_segment_seed_points","bool","Should seed points be extracted from the input segmentation?"), true),
      option_string(*this, parameter_information("option_string","string","The tetgen options string")),
      output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, segmented tetrahedral 3d")) {}

    std::string make_mesh::name() const { return "Tetgen 1.5 mesher"; }
    std::string make_mesh::id() const { return "tetgen_make_mesh"; }


    bool make_mesh::run_impl()
    {
      typedef tetgen::output_mesh OutputMeshType;
      output_parameter_proxy<OutputMeshType> omp(output_mesh);


      tetgenbehavior options;

      if (option_string.valid())
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

      using_sizing_function = false;
      using_max_edge_ratio = false;
      using_max_inscribed_radius_edge_ratio = false;


      tetgenio & tmp = (tetgenio&)(input_mesh().mesh);

      if (max_radius_edge_ratio.valid())
      {
        options.quality = 1;
        options.minratio = max_radius_edge_ratio();
        info(1) << "Using global maximum radius edge ratio: " << max_radius_edge_ratio() << std::endl;
      }

      if (min_dihedral_angle.valid())
      {
        options.quality = 1;
        options.mindihedral = min_dihedral_angle();
        info(1) << "Using global minimum dihedral angle: " << min_dihedral_angle() << std::endl;
      }

      if (max_edge_ratio.valid())
      {
        viennamesh::tetgen::max_edge_ratio = max_edge_ratio();
        using_max_edge_ratio = true;
        options.use_refinement_callback = 1;
        tmp.tetunsuitable = should_tetrahedron_be_refined_function;
        info(1) << "Using global max edge ratio: " << max_edge_ratio() << std::endl;
      }

      if (max_inscribed_radius_edge_ratio.valid())
      {
        viennamesh::tetgen::max_inscribed_radius_edge_ratio = max_inscribed_radius_edge_ratio();
        using_max_inscribed_radius_edge_ratio = true;
        options.use_refinement_callback = 1;
        tmp.tetunsuitable = should_tetrahedron_be_refined_function;
        info(1) << "Using global max inscribed radius edge ratio: " << max_inscribed_radius_edge_ratio() << std::endl;
      }



      tetgenio const & im = (tetgenio&)(input_mesh().mesh);

      point_3d_container hole_points;
      // exttract hole points from input interface
      if (input_hole_points.valid() && !input_hole_points().empty())
      {
        info(5) << "Found hole points" << std::endl;
        hole_points = input_hole_points();
      }
      // hole points from mesh
      for (int i = 0; i < im.numberofholes; ++i)
        hole_points.push_back( point_3d(im.holelist[3*i+0], im.holelist[3*i+1], im.holelist[3*i+2]) );


      seed_point_3d_container seed_points;
      // seed points from input interface
      if (input_seed_points.valid() && !input_seed_points().empty())
      {
        info(5) << "Found seed points" << std::endl;
        seed_points = input_seed_points();
      }
      // seed points from mesh
      for (int i = 0; i < im.numberofregions; ++i)
        seed_points.push_back( std::make_pair(point_3d(im.regionlist[5*i+0], im.regionlist[5*i+1], im.regionlist[5*i+2]),
                                              static_cast<int>(im.regionlist[5*i+3])+0.5) );
      // seed points from segmentation
      if (extract_segment_seed_points())
      {
        extract_seed_points( input_mesh().segmentation, hole_points, seed_points );
      }




      if (sizing_function.valid())
      {
        if (sizing_function.get<sizing_function_3d>())
        {
          info(5) << "Using user-defined sizing function" << std::endl;
          tetgen_sizing_function = sizing_function.get<sizing_function_3d>()();
          using_sizing_function = true;
          options.use_refinement_callback = 1;
          tmp.tetunsuitable = should_tetrahedron_be_refined_function;
        }
        else if (sizing_function.get<pugi::xml_node>())
        {
          info(5) << "Using user-defined XML sizing function" << std::endl;
          tetgen_sizing_function = make_sizing_function(
                                      input_mesh().mesh, hole_points, seed_points,
                                      sizing_function.get<pugi::xml_node>()(), base_path());
          using_sizing_function = true;
          options.use_refinement_callback = 1;
          tmp.tetunsuitable = should_tetrahedron_be_refined_function;
        }
        else if (sizing_function.get<std::string>())
        {
          info(5) << "Using user-defined XML string sizing function" << std::endl;
          info(5) << sizing_function.get<std::string>()() << std::endl;
          tetgen_sizing_function = make_sizing_function(
                                      input_mesh().mesh, hole_points, seed_points,
                                      sizing_function.get<std::string>()(), base_path());
          using_sizing_function = true;
          options.use_refinement_callback = 1;
          tmp.tetunsuitable = should_tetrahedron_be_refined_function;
        }
        else
          warning(5) << "Type of sizing function not supported" << std::endl;


      }
      else if (cell_size.valid())
      {
        options.fixedvolume = 1;
        options.maxvolume = cell_size();
        info(1) << "Using global maximum cell size: " << cell_size() << std::endl;
      }


      make_mesh_impl( input_mesh().mesh, omp(), hole_points, seed_points, options );


      if (sizing_function.valid())
        tmp.tetunsuitable = NULL;

      return true;
    }
  }
}

#endif
