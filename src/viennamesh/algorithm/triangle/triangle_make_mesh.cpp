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

#include "viennamesh/algorithm/triangle/triangle_mesh.hpp"
#include "viennamesh/algorithm/triangle/triangle_make_mesh.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"
#include "viennamesh/core/sizing_function.hpp"

namespace viennamesh
{
  namespace triangle
  {
    viennamesh::sizing_function_2d triangle_sizing_function;

    int should_triangle_be_refined_function(double * triorg, double * tridest, double * triapex, double)
    {
      REAL dxoa, dxda, dxod;
      REAL dyoa, dyda, dyod;
      REAL oalen, dalen, odlen;
      REAL maxlen;

      dxoa = triorg[0] - triapex[0];
      dyoa = triorg[1] - triapex[1];
      dxda = tridest[0] - triapex[0];
      dyda = tridest[1] - triapex[1];
      dxod = triorg[0] - tridest[0];
      dyod = triorg[1] - tridest[1];
      /* Find the squares of the lengths of the triangle's three edges. */
      oalen = dxoa * dxoa + dyoa * dyoa;
      dalen = dxda * dxda + dyda * dyda;
      odlen = dxod * dxod + dyod * dyod;
      /* Find the square of the length of the longest edge. */
      maxlen = (dalen > oalen) ? dalen : oalen;
      maxlen = (odlen > maxlen) ? odlen : maxlen;

      point_2d pt;
      pt[0] = (triorg[0] + tridest[0] + triapex[0]) / 3;
      pt[1] = (triorg[1] + tridest[1] + triapex[1]) / 3;

      viennagrid::static_array<double, 4> local_sizes;

      local_sizes[0] = triangle_sizing_function( point_2d(triorg[0], triorg[1]) );
      local_sizes[1] = triangle_sizing_function( point_2d(tridest[0], tridest[1]) );
      local_sizes[2] = triangle_sizing_function( point_2d(triapex[0], triapex[1]) );
      local_sizes[3] = triangle_sizing_function(pt);

      double local_size = -1;
      for (int i = 0; i < 4; ++i)
      {
        if (local_sizes[i] > 0)
        {
          if (local_size < 0)
            local_size = local_sizes[i];
          else
            local_size = std::min( local_size, local_sizes[i] );
        }
      }

      if (local_size > 0 && maxlen > local_size*local_size)
        return 1;
      else
        return 0;
    }





    void make_mesh_impl(triangle::input_mesh const & input,
                   triangle::output_mesh & output,
                   point_2d_container const & hole_points,
                   seed_point_2d_container const & seed_points,
                   std::string options)
    {
      triangulateio tmp = input.triangle_mesh;


      if (!hole_points.empty())
      {
        tmp.numberofholes = hole_points.size();
        tmp.holelist = new REAL[2*hole_points.size()];

        for (std::size_t i = 0; i < hole_points.size(); ++i)
        {
          tmp.holelist[2*i+0] = hole_points[i][0];
          tmp.holelist[2*i+1] = hole_points[i][1];
        }
      }
      else
      {
        tmp.numberofholes = 0;
        tmp.holelist = NULL;
      }


      if (!seed_points.empty())
      {
        tmp.numberofregions = seed_points.size();
        tmp.regionlist = new REAL[4*seed_points.size()];

        for (unsigned int i = 0; i < seed_points.size(); ++i)
        {
          tmp.regionlist[4*i+0] = seed_points[i].first[0];
          tmp.regionlist[4*i+1] = seed_points[i].first[1];
          tmp.regionlist[4*i+2] = REAL(seed_points[i].second);
          tmp.regionlist[4*i+3] = 0;
        }

        options += 'A';
      }
      else
      {
        tmp.numberofregions = 0;
        tmp.regionlist = NULL;
      }

      char * options_buffer = new char[options.length()];
      std::strcpy(options_buffer, options.c_str());

      {
        StdCaptureHandle capture_handle;
        triangulate( options_buffer, &tmp, &output.triangle_mesh, NULL);
      }

      delete[] tmp.regionlist;
      delete[] tmp.holelist;
      delete[] options_buffer;
    }



    void extract_seed_points( triangle::input_segmentation const & segmentation,
                              point_2d_container const & hole_points,
                              seed_point_2d_container & seed_points )
    {
      if (segmentation.segments.empty())
        return;

      LoggingStack stack( std::string("Extracting seed points from segments") );

      info(5) << "Extracting seed points from segments" << std::endl;

      std::string options = "zpQ";

      int highest_segment_id = -1;
      for (seed_point_2d_container::iterator spit = seed_points.begin(); spit != seed_points.end(); ++spit)
        highest_segment_id = std::max( highest_segment_id, spit->second );
      ++highest_segment_id;

      for ( std::list<triangle::input_mesh>::const_iterator sit = segmentation.segments.begin(); sit != segmentation.segments.end(); ++sit)
      {
        triangle::output_mesh tmp_mesh;

        LoggingStack stack( std::string("Segment ") + lexical_cast<std::string>(highest_segment_id) );
        make_mesh_impl(*sit, tmp_mesh, hole_points, seed_point_2d_container(), options);

        viennagrid::triangular_2d_mesh viennagrid_mesh;
        viennamesh::triangle::convert( tmp_mesh, viennagrid_mesh );

        std::vector<point_2d> local_seed_points;
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
    sizing_function_2d make_sizing_function(triangle::input_mesh const & mesh,
                                            point_2d_container const & hole_points,
                                            seed_point_2d_container const & seed_points,
                                            SizingFunctionRepresentationT const & sf,
                                            std::string const & base_path)
    {
      typedef viennagrid::triangular_2d_mesh MeshType;
      typedef viennagrid::triangular_2d_segmentation SegmentationType;

      typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
      viennamesh::result_of::parameter_handle<SegmentedMeshType>::type simple_mesh = viennamesh::make_parameter<SegmentedMeshType>();

      std::string options = "zpQ";
      triangle::output_mesh tmp_mesh;

      make_mesh_impl(mesh, tmp_mesh, hole_points, seed_points, options);
      viennamesh::triangle::convert( tmp_mesh, simple_mesh() );

      return viennamesh::sizing_function::from_xml(sf, simple_mesh, base_path);
    }



    make_mesh::make_mesh() :
      input_mesh(*this, parameter_information("mesh","mesh","The input mesh, segmented triangle input_mesh supported")),
      input_seed_points(*this, parameter_information("seed_points","seed_point_2d_container","A container of seed points with corresonding segment names")),
      input_hole_points(*this, parameter_information("hole_points","point_2d_container","A container of hole points")),
      sizing_function(*this, parameter_information("sizing_function","sizing_function_2d|string|pugi::xml_node","The sizing function, different representation are supported: arbitrary sizing_function_2d or the ViennaMesh sizing function framework in string or pugi::xml_node representation")),
      cell_size(*this, parameter_information("cell_size","double","The desired maximum size of tetrahedrons, all tetrahedrons will be at most this size")),
      min_angle(*this, parameter_information("min_angle","double","Desired minimum angle")),
      delaunay(*this, parameter_information("delaunay","bool","Determines if the output mesh should be delaunay"), true),
      algorithm_type(*this, parameter_information("algorithm_type","string","The meshing algorithm, supported algorithms: incremental_delaunay, sweepline, devide_and_conquer")),
      extract_segment_seed_points(*this, parameter_information("extract_segment_seed_points","bool","Should seed points be extracted from the input segmentation?"), true),
      option_string(*this, parameter_information("option_string","string","The triangle options string")),
      output_mesh(*this, parameter_information("mesh", "mesh", "The output mesh, segmented triangle 2d")) {}

    std::string make_mesh::name() const { return "Triangle 1.6 mesher"; }
    std::string make_mesh::id() const { return "triangle_make_mesh"; }


    bool make_mesh::run_impl()
    {
      typedef triangle::output_mesh OutputMeshType;
      output_parameter_proxy<OutputMeshType> omp(output_mesh);

      std::ostringstream options;

      if (option_string.valid())
        options << option_string();
      else
        options << "zp";

      if (min_angle.valid())
        options << "q" << min_angle() / M_PI * 180.0;

      if (cell_size.valid())
        options << "a" << cell_size();

      if (delaunay())
        options << "D";

      if (algorithm_type.valid())
      {
        if (algorithm_type() == "incremental_delaunay")
          options << "i";
        else if (algorithm_type() == "sweepline")
          options << "F";
        else if (algorithm_type() == "devide_and_conquer")
        {}
        else
        {
          warning(5) << "Algorithm not recognized: '" << algorithm_type() << "' supported algorithms:" << std::endl;
          warning(5) << "  'incremental_delaunay'" << std::endl;
          warning(5) << "  'sweepline'" << std::endl;
          warning(5) << "  'devide_and_conquer'" << std::endl;
        }
      }


      triangulateio const & im = input_mesh().mesh.triangle_mesh;

      point_2d_container hole_points;
      // exttract hole points from input interface
      if (input_hole_points.valid() && !input_hole_points().empty())
      {
        info(5) << "Found hole points" << std::endl;
        hole_points = input_hole_points();
      }
      // hole points from mesh
      for (int i = 0; i < im.numberofholes; ++i)
        hole_points.push_back( point_2d(im.holelist[2*i+0], im.holelist[2*i+1]) );


      seed_point_2d_container seed_points;
      // seed points from input interface
      if (input_seed_points.valid() && !input_seed_points().empty())
      {
        info(5) << "Found input seed points" << std::endl;
        seed_points = input_seed_points();
      }
      // seed points from mesh
      for (int i = 0; i < im.numberofregions; ++i)
        seed_points.push_back( std::make_pair(point_2d(im.regionlist[4*i+0], im.regionlist[4*i+1]),
                                              static_cast<int>(im.regionlist[4*i+2])+0.5) );
      // seed points from segmentation
      if (extract_segment_seed_points())
      {
        extract_seed_points( input_mesh().segmentation, hole_points, seed_points );
      }


      if (sizing_function.valid())
      {
        if (sizing_function.get<sizing_function_2d>())
        {
          info(5) << "Using user-defined sizing function" << std::endl;
          triangle_sizing_function = sizing_function.get<sizing_function_2d>()();
          options << "u";
          should_triangle_be_refined = should_triangle_be_refined_function;
        }
        else if (sizing_function.get<pugi::xml_node>())
        {
          info(5) << "Using user-defined XML sizing function" << std::endl;
          triangle_sizing_function = make_sizing_function(
                                      input_mesh().mesh, hole_points, seed_points,
                                      sizing_function.get<pugi::xml_node>()(), base_path());
          options << "u";
          should_triangle_be_refined = should_triangle_be_refined_function;
        }
        else if (sizing_function.get<std::string>())
        {
          info(5) << "Using user-defined XML string sizing function" << std::endl;
          info(5) << sizing_function.get<std::string>()() << std::endl;
          triangle_sizing_function = make_sizing_function(
                                      input_mesh().mesh, hole_points, seed_points,
                                      sizing_function.get<std::string>()(), base_path());
          options << "u";
          should_triangle_be_refined = should_triangle_be_refined_function;
        }
        else
          warning(5) << "Type of sizing function not supported" << std::endl;
      }

      make_mesh_impl( input_mesh().mesh, omp(), hole_points, seed_points, options.str() );

      return true;
    }



  }
}

#endif
