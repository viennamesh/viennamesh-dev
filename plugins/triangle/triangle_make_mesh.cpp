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
#include "triangle_make_mesh.hpp"
#include "viennagrid/static_array.hpp"
#include "viennagrid/algorithm/extract_seed_points.hpp"
#include "viennamesh/sizing_function.hpp"

namespace viennamesh
{
  namespace triangle
  {
    SizingFunctionType triangle_sizing_function;

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

      viennagrid::point_t pt(2);
      pt[0] = (triorg[0] + tridest[0] + triapex[0]) / 3;
      pt[1] = (triorg[1] + tridest[1] + triapex[1]) / 3;




      viennagrid::static_array<viennagrid::point_t, 4> sample_points;
      sample_points[0] = viennagrid::make_point(triorg[0], triorg[1]);
      sample_points[1] = viennagrid::make_point(tridest[0], tridest[1]);
      sample_points[2] = viennagrid::make_point(triapex[0], triapex[1]);
      sample_points[3] = pt;


      SizingFunctionReturnType local_size;
      for (int i = 0; i != 4; ++i)
      {
        SizingFunctionReturnType current_size = triangle_sizing_function( sample_points[i] );
        if (current_size)
        {
          if (!local_size)
            local_size = current_size;
          else
          {
            if (current_size.get() < local_size.get() )
              local_size = current_size;
          }
        }
      }

//       if (local_size)
//         std::cout << "Requested size = " << local_size.get() << "   current triangle size = " << std::sqrt(maxlen) << std::endl;

      if (local_size && maxlen > local_size.get()*local_size.get())
        return 1;
      else
        return 0;
    }





    void make_mesh_impl(triangle::input_mesh const & input,
                        triangle::output_mesh & output,
                        PointContainerType const & hole_points,
                        SeedPointContainerType const & seed_points,
                        std::string options)
    {
      triangulateio tmp = input.triangle_mesh;


      if (!hole_points.empty())
      {
        tmp.numberofholes = hole_points.size();
        tmp.holelist = new REAL[2 * hole_points.size()];

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
        tmp.regionlist = new REAL[4 * seed_points.size()];

        for (std::size_t i = 0; i < seed_points.size(); ++i)
        {
          tmp.regionlist[4*i+0] = seed_points[i].first[0];
          tmp.regionlist[4*i+1] = seed_points[i].first[1];
          tmp.regionlist[4*i+2] = REAL(seed_points[i].second);
          tmp.regionlist[4*i+3] = 0;
        }

        options += 'A';
        info(1) << "Using seed points" << std::endl;
      }
      else
      {
        tmp.numberofregions = 0;
        tmp.regionlist = NULL;
      }

      char * options_buffer = new char[options.length()+1];
      std::strcpy(options_buffer, options.c_str());

      {
        StdCaptureHandle capture_handle;
        triangulate( options_buffer, &tmp, &(output.triangle_mesh), NULL);
      }

      delete[] tmp.regionlist;
      delete[] tmp.holelist;
      delete[] options_buffer;
    }



//     void extract_seed_points( triangle::input_mesh const & mesh,
//                               std::vector<viennagrid::point_t> const & hole_points,
//                               std::vector< std::pair<viennagrid::point_t, int> > & seed_points )
//     {
//       if (segmentation.segments.empty())
//         return;
//
//       LoggingStack stack( std::string("Extracting seed points from segments") );
//
//       info(5) << "Extracting seed points from segments" << std::endl;
//
//       std::string options = "zpQ";
//
//       int highest_segment_id = -1;
//       for (seed_point_2d_container::iterator spit = seed_points.begin(); spit != seed_points.end(); ++spit)
//         highest_segment_id = std::max( highest_segment_id, spit->second );
//       ++highest_segment_id;
//
//       for ( std::list<triangle::input_mesh>::const_iterator sit = segmentation.segments.begin(); sit != segmentation.segments.end(); ++sit)
//       {
//         triangle::output_mesh tmp_mesh;
//
//         LoggingStack stack( std::string("Segment ") + lexical_cast<std::string>(highest_segment_id) );
//         make_mesh_impl(*sit, tmp_mesh, hole_points, seed_point_2d_container(), options);
//
//         viennagrid::triangular_2d_mesh viennagrid_mesh;
//         viennamesh::triangle::convert( tmp_mesh, viennagrid_mesh );
//
//         std::vector<point_2d> local_seed_points;
//         viennagrid::extract_seed_points( viennagrid_mesh, local_seed_points );
//         for (unsigned int i = 0; i < local_seed_points.size(); ++i)
//         {
//           info(5) << "Found seed point: " << local_seed_points[i] << std::endl;
//           seed_points.push_back( std::make_pair(local_seed_points[i], highest_segment_id) );
//         }
//         highest_segment_id++;
//       }
//     }



    template<typename SizingFunctionRepresentationT>
    SizingFunctionType make_sizing_function(triangle::input_mesh const & mesh,
                                            PointContainerType const & hole_points,
                                            SeedPointContainerType const & seed_points,
                                            SizingFunctionRepresentationT const & sf,
                                            std::string const & base_path)
    {
      typedef viennagrid::mesh_t MeshType;
      MeshType simple_mesh;
//       typedef viennagrid::triangular_2d_segmentation SegmentationType;

//       typedef viennagrid::segmented_mesh<MeshType, SegmentationType> SegmentedMeshType;
//       viennamesh::result_of::parameter_handle<SegmentedMeshType>::type simple_mesh = viennamesh::make_parameter<SegmentedMeshType>();

      std::string options = "zpQ";
      triangle::output_mesh tmp_mesh;

      make_mesh_impl(mesh, tmp_mesh, hole_points, seed_points, options);
      viennamesh::convert( tmp_mesh, simple_mesh );

      return viennamesh::sizing_function::from_xml(sf, simple_mesh, base_path);
    }



    make_mesh::make_mesh() {}

    std::string make_mesh::name() { return "triangle_make_mesh"; }


    bool make_mesh::run(viennamesh::algorithm_handle &)
    {
      data_handle<viennamesh_string> option_string = get_input<viennamesh_string>("option_string");
      data_handle<double> min_angle = get_input<double>("min_angle");
      data_handle<double> cell_size = get_input<double>("cell_size");
      data_handle<bool> delaunay = get_input<bool>("delaunay");
      data_handle<viennamesh_string> algorithm_type = get_input<viennamesh_string>("algorithm_type");
      data_handle<bool> no_points_on_boundary = get_input<bool>("no_points_on_boundary");

      data_handle<triangle::input_mesh> input_mesh = get_required_input<triangle::input_mesh>("mesh");
      point_container_handle input_hole_points = get_input<point_container_handle>("hole_points");
      seed_point_container_handle input_seed_points = get_input<seed_point_container_handle>("seed_points");

      data_handle<triangle::output_mesh> output_mesh = make_data<triangle::output_mesh>();


      std::ostringstream options;

      if (option_string.valid())
        options << option_string();
      else
        options << "zp";

      if (min_angle.valid())
        options << "q" << min_angle() / M_PI * 180.0;

      if (cell_size.valid())
        options << "a" << cell_size();

      if ( !delaunay.valid() || (delaunay.valid() && delaunay()) )
        options << "D";

      if ( no_points_on_boundary.valid() && no_points_on_boundary() )
        options << "Y";

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


      triangulateio const & im = input_mesh().triangle_mesh;

      PointContainerType hole_points;
//       exttract hole points from input interface
      if (input_hole_points.valid())
      {
        convert(input_hole_points(), hole_points);
        info(5) << "Found hole " << hole_points.size() << " points" << std::endl;
      }
      // hole points from mesh
      for (int i = 0; i < im.numberofholes; ++i)
        hole_points.push_back( viennagrid::make_point(im.holelist[2*i+0], im.holelist[2*i+1]) );


      SeedPointContainerType seed_points;
//       seed points from input interface
      if (input_seed_points.valid())
      {
        convert(input_seed_points(), seed_points);
        info(5) << "Found seed " << seed_points.size() << " points" << std::endl;
      }
      // seed points from mesh
      for (int i = 0; i < im.numberofregions; ++i)
      {
        seed_points.push_back(
          std::make_pair(
            viennagrid::make_point(im.regionlist[4*i+0], im.regionlist[4*i+1]),
            static_cast<int>(im.regionlist[4*i+2])+0.5
          )
        );
      }
      // seed points from segmentation
//       if (extract_segment_seed_points())
//       {
//         extract_seed_points( input_mesh().segmentation, hole_points, seed_points );
//       }





      data_handle<viennamesh_string> sizing_function = get_input<viennamesh_string>("sizing_function");
      if (sizing_function)
      {
        info(5) << "Using user-defined XML string sizing function" << std::endl;
        info(5) << sizing_function() << std::endl;
        triangle_sizing_function = make_sizing_function(
                                    input_mesh(), hole_points, seed_points,
                                    sizing_function(), base_path());
        options << "u";
        should_triangle_be_refined = should_triangle_be_refined_function;
      }





//       abstract_data_handle sizing_function = get_input<>("sizing_function");
//       if (sizing_function.valid())
//       {
//         if (sizing_function.get<sizing_function_2d>())
//         {
//           info(5) << "Using user-defined sizing function" << std::endl;
//           triangle_sizing_function = sizing_function.get<sizing_function_2d>()();
//           options << "u";
//           should_triangle_be_refined = should_triangle_be_refined_function;
//         }
//         else
//         if (sizing_function.get<pugi::xml_node>())
//         {
//           info(5) << "Using user-defined XML sizing function" << std::endl;
//           triangle_sizing_function = make_sizing_function(
//                                       input_mesh().mesh, hole_points, seed_points,
//                                       sizing_function.get<pugi::xml_node>()(), base_path());
//           options << "u";
//           should_triangle_be_refined = should_triangle_be_refined_function;
//         }
//         else if (sizing_function.get<std::string>())
//         {
//           info(5) << "Using user-defined XML string sizing function" << std::endl;
//           info(5) << sizing_function.get<std::string>()() << std::endl;
//           triangle_sizing_function = make_sizing_function(
//                                       input_mesh().mesh, hole_points, seed_points,
//                                       sizing_function.get<std::string>()(), base_path());
//           options << "u";
//           should_triangle_be_refined = should_triangle_be_refined_function;
//         }
//         else
//           warning(5) << "Type of sizing function not supported" << std::endl;
//       }

//       make_mesh_impl( input_mesh(), output_mesh(), hole_points(), seed_points(), options.str() );


      info(1) << "Making mesh with option string " << options.str() << std::endl;
      make_mesh_impl( input_mesh(), output_mesh(), hole_points, seed_points, options.str() );




      set_output("mesh", output_mesh);

      return true;
    }



  }
}
