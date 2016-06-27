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

#include <set>
#include <map>
#include <iterator>

#include "extract_slice_2d.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/centroid.hpp"


namespace viennamesh
{



  extract_symmetric_slice_2d::extract_symmetric_slice_2d() {}
  std::string extract_symmetric_slice_2d::name() { return "extract_symmetric_slice_2d"; }

  bool extract_symmetric_slice_2d::run(viennamesh::algorithm_handle &)
  {
    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");
    int geometric_dimension = viennagrid::geometric_dimension( input_mesh() );

    if (geometric_dimension != 2)
      return false;

    double tol = 1e-6;

    typedef viennagrid::mesh                                                MeshType;
    typedef viennagrid::result_of::point<MeshType>::type                    PointType;
    typedef viennagrid::result_of::element<MeshType>::type                  ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type      ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementIteratorType;



    point_handle input_centroid = get_required_input<point_handle>("centroid");
    PointType centroid = input_centroid();
//     convert( input_centroid(), centroid );

    std::vector<double> mirror_axis = get_input<double>("mirror_axis").get_vector();
    std::vector<int> rotational_frequencies = get_input<int>("rotational_frequencies").get_vector();

    info(1) << "Got global mirror axis: " << std::endl;
    for (std::size_t i = 0; i != mirror_axis.size(); ++i)
      info(1) << "  " << mirror_axis[i] << std::endl;

    info(1) << "Got global rotational frequencies: " << std::endl;
    for (std::size_t i = 0; i != rotational_frequencies.size(); ++i)
      info(1) << "  " << rotational_frequencies[i] << std::endl;

    double max_distance = -1.0;
    ConstElementRangeType vertices(input_mesh(), 0);
    for (ConstElementIteratorType vit = vertices.begin(); vit != vertices.end(); ++vit)
      max_distance = std::max(max_distance, viennagrid::distance(*vit, centroid));


    mesh_handle output_mesh = make_data<mesh_handle>();
    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), tol );


    if (rotational_frequencies.empty() && mirror_axis.empty())
      return false;

    if (rotational_frequencies.empty())
    {
      info(1) << "Only using mirror symmetry, no rotational symmetry" << std::endl;
      double used_axis = mirror_axis.front();
      set_output( "mirror_axis", used_axis );
      set_output( "centroid", centroid );

      PointType vector = viennagrid::make_point( std::cos(used_axis), std::sin(used_axis) );
      PointType normal = vector;
      std::swap(normal[0], normal[1]);
      normal[0] = -normal[0];

      PointType mirror_axis_start = centroid - vector*max_distance*1.2;
      PointType mirror_axis_end = centroid + vector*max_distance*1.2;

      info(1) << "Using mirror axis: " << mirror_axis_start << " - " << mirror_axis_end << std::endl;
      std::map<double, ElementType> vertices_on_mirror_axis;

      ConstElementRangeType lines(input_mesh(),1);
      for (ConstElementIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        std::pair<PointType, PointType> cp = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(*lit,0), viennagrid::get_point(*lit,1),
          mirror_axis_start, mirror_axis_end
        );

        if ( viennagrid::detail::is_equal(tol, cp.first, cp.second) )
        {

          if (viennagrid::detail::is_equal(tol, viennagrid::get_point(*lit,0), cp.first) ||
              viennagrid::detail::is_equal(tol, viennagrid::get_point(*lit,1), cp.first))
          {
            // one point of the line is on the mirror axis
            if (viennagrid::inner_prod(viennagrid::centroid(*lit)-centroid, normal) > 0)
              copy_map(*lit);

            if (viennagrid::detail::is_equal(tol, viennagrid::get_point(*lit,0), cp.first))
              vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(*lit)[0]);
            else
              vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(*lit)[1]);
          }
          else
          {
            ElementType new_vertex = viennagrid::make_unique_vertex( output_mesh(), cp.first, tol );
            vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = new_vertex;

            if (viennagrid::inner_prod(viennagrid::get_point(*lit,0)-centroid, normal) > 0)
              viennagrid::make_line( output_mesh(), new_vertex, viennagrid::vertices(*lit)[0] );
            else
              viennagrid::make_line( output_mesh(), new_vertex, viennagrid::vertices(*lit)[1] );
          }
        }
        else
        {
          if (viennagrid::inner_prod(viennagrid::centroid(*lit)-centroid, normal) > 0)
            copy_map(*lit);
        }
      }

      if (!vertices_on_mirror_axis.empty())
      {
        std::map<double, ElementType>::iterator it = vertices_on_mirror_axis.begin();
        std::map<double, ElementType>::iterator it_prev = it;
        ++it;

        while (it != vertices_on_mirror_axis.end())
        {
          viennagrid::make_line( output_mesh(), it->second, it_prev->second );
          ++it;
          ++it_prev;
        }
      }
    }
    else
    {
      if (mirror_axis.empty())
        info(1) << "Only using rotational symmetry" << std::endl;
      else
        info(1) << "Using mirror symmetry and rotational symmetry" << std::endl;

//       int mirror_axis_index = 0;
//       data_handle<int> mirror_axis_index_input = get_input<int>("mirror_axis_index");
//       if (mirror_axis_index_input.valid())
//         mirror_axis_index = mirror_axis_index_input();

      double start_angle = 0.0;

      data_handle<double> start_angle_input = get_input<double>("start_angle");
      if (start_angle_input.valid())
        start_angle = start_angle_input();

      if (!mirror_axis.empty())
        start_angle = mirror_axis.front();


      int rotational_frequency = rotational_frequencies.back();
      double rotational_angle = M_PI/rotational_frequency;
      if (mirror_axis.empty())
        rotational_angle *= 2.0;


      set_output( "rotational_frequency", rotational_frequency);

      if (!mirror_axis.empty())
        set_output( "mirror_axis", start_angle );

      set_output( "centroid", centroid );

      PointType vector_0 = viennagrid::make_point( std::cos(start_angle), std::sin(start_angle) );
      PointType vector_1 = viennagrid::make_point( std::cos(start_angle + rotational_angle), std::sin(start_angle + rotational_angle) );

      PointType normal_0 = vector_0;
      std::swap(normal_0[0], normal_0[1]);
      normal_0[0] = -normal_0[0];

      PointType normal_1 = vector_1;
      std::swap(normal_1[0], normal_1[1]);
      normal_1[0] = -normal_1[0];



      PointType axis_0_end = centroid + vector_0*max_distance*1.2;
      PointType axis_1_end = centroid + vector_1*max_distance*1.2;

      info(1) << "Using axis 0: " << centroid << " - " << axis_0_end << std::endl;
      info(1) << "Using axis 1: " << centroid << " - " << axis_1_end << std::endl;


      std::map<double, ElementType> vertices_on_mirror_axis;
      std::map<double, ElementType> vertices_on_angle_axis;

      ElementType centrod_vertex = viennagrid::make_vertex( output_mesh(), centroid );
      vertices_on_mirror_axis[0] = centrod_vertex;
      vertices_on_angle_axis[0] = centrod_vertex;


      ConstElementRangeType lines(input_mesh(),1);
      for (ConstElementIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        std::pair<PointType, PointType> cp_0 = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(*lit,0), viennagrid::get_point(*lit,1),
          centroid, axis_0_end
        );

        std::pair<PointType, PointType> cp_1 = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(*lit,0), viennagrid::get_point(*lit,1),
          centroid, axis_1_end
        );

        if ( viennagrid::detail::is_equal(tol, cp_0.first, cp_0.second) &&
             viennagrid::detail::is_equal(tol, cp_1.first, cp_1.second) )
        {
          ElementType v_0 = viennagrid::make_unique_vertex( output_mesh(), cp_0.first, tol );
          vertices_on_mirror_axis[ viennagrid::inner_prod(cp_0.first-centroid, vector_0) ] = v_0;

          ElementType v_1 = viennagrid::make_unique_vertex( output_mesh(), cp_1.first, tol );
          vertices_on_angle_axis[ viennagrid::inner_prod(cp_1.first-centroid, vector_1) ] = v_1;

          viennagrid::make_line(  output_mesh(), v_0, v_1 );
        }
        else
        {
          if ( viennagrid::detail::is_equal(tol, cp_0.first, cp_0.second) )
          {
            ElementType v_0 = viennagrid::make_unique_vertex( output_mesh(), cp_0.first, tol );
            vertices_on_mirror_axis[ viennagrid::inner_prod(cp_0.first-centroid, vector_0) ] = v_0;

            if (viennagrid::inner_prod( viennagrid::get_point(*lit,0)-centroid, normal_0 ) > 0)
              viennagrid::make_line( output_mesh(), v_0, copy_map(viennagrid::vertices(*lit)[0]) );
            else
              viennagrid::make_line( output_mesh(), v_0, copy_map(viennagrid::vertices(*lit)[1]) );
          }
          else if ( viennagrid::detail::is_equal(tol, cp_1.first, cp_1.second) )
          {
            ElementType v_1 = viennagrid::make_unique_vertex( output_mesh(), cp_1.first, tol );
            vertices_on_angle_axis[ viennagrid::inner_prod(cp_1.first-centroid, vector_1) ] = v_1;

            if (viennagrid::inner_prod( viennagrid::get_point(*lit,0)-centroid, normal_1 ) < 0)
              viennagrid::make_line( output_mesh(), v_1, copy_map(viennagrid::vertices(*lit)[0]) );
            else
              viennagrid::make_line( output_mesh(), v_1, copy_map(viennagrid::vertices(*lit)[1]) );
          }
          else
          {
            if ( (viennagrid::inner_prod( viennagrid::centroid(*lit)-centroid, normal_0 ) > 0) &&
                 (viennagrid::inner_prod( viennagrid::centroid(*lit)-centroid, normal_1 ) < 0) )
            {
              copy_map(*lit);
            }

          }
        }


      }


      if (!vertices_on_mirror_axis.empty())
      {
        std::map<double, ElementType>::iterator it = vertices_on_mirror_axis.begin();
        std::map<double, ElementType>::iterator it_prev = it;
        ++it;

        while (it != vertices_on_mirror_axis.end())
        {
          viennagrid::make_line( output_mesh(), it->second, it_prev->second );
          ++it;
          ++it_prev;
        }
      }

      if (!vertices_on_angle_axis.empty())
      {
        std::map<double, ElementType>::iterator it = vertices_on_angle_axis.begin();
        std::map<double, ElementType>::iterator it_prev = it;
        ++it;

        while (it != vertices_on_angle_axis.end())
        {
          viennagrid::make_line( output_mesh(), it->second, it_prev->second );
          ++it;
          ++it_prev;
        }
      }
    }


//     info(1) << "Number of vertices: " << viennagrid::vertices( output_mesh() ).size() << std::endl;
//     info(1) << "Number of lines: " << viennagrid::cells( output_mesh() ).size() << std::endl;


    data_handle<double> line_size = get_input<double>("line_size");
    if (line_size.valid())
    {
      mesh_handle tmp = make_data<mesh_handle>();

      ConstElementRangeType lines(input_mesh(),1);
      for (ConstElementIteratorType lit = lines.begin(); lit != lines.end(); ++lit)
      {
        PointType start = viennagrid::get_point(*lit, 0);
        PointType end = viennagrid::get_point(*lit, 1);

        double old_line_size = viennagrid::distance(start, end);

        int line_count = old_line_size / line_size() + 0.5;
        line_count = std::max(line_count, 1);

        std::cout << "old_line_size = " << old_line_size << std::endl;
        std::cout << "line_count = " << line_count << std::endl;

        PointType step = (end-start) / line_count;
        for (int i = 0; i != line_count; ++i)
        {
          PointType s = start + i*step;
          PointType e = start + (i+1)*step;

          ElementType vs = viennagrid::make_unique_vertex( tmp(), s, 1e-8 );
          ElementType ve = viennagrid::make_unique_vertex( tmp(), e, 1e-8 );

          viennagrid::make_line( tmp(), vs, ve );
        }
      }

      set_output( "mesh", tmp );
    }
    else
      set_output( "mesh", output_mesh );

    return true;
  }

}
