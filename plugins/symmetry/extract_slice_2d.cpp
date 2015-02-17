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

    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;



    point_container_handle input_centroid = get_required_input<point_container_handle>("centroid");
    PointType centroid;
    convert( input_centroid(), centroid );

    std::vector<double> mirror_axis = get_input_vector<double>("mirror_axis");
    std::vector<int> rotational_frequencies = get_input_vector<int>("rotational_frequencies");

    info(1) << "Got global mirror axis: " << std::endl;
    for (std::size_t i = 0; i != mirror_axis.size(); ++i)
      info(1) << "  " << mirror_axis[i] << std::endl;

    info(1) << "Got global rotational frequencies: " << std::endl;
    for (std::size_t i = 0; i != rotational_frequencies.size(); ++i)
      info(1) << "  " << rotational_frequencies[i] << std::endl;

    double max_distance = -1.0;
    for (auto vertex : viennagrid::vertices(input_mesh()))
      max_distance = std::max(max_distance, viennagrid::distance(vertex, centroid));


    mesh_handle output_mesh = make_data<mesh_handle>();
    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), tol );



    if (rotational_frequencies.empty() && !mirror_axis.empty())
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

      for (auto line : viennagrid::elements(input_mesh(),1))
      {
        std::pair<PointType, PointType> cp = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(line,0), viennagrid::get_point(line,1),
          mirror_axis_start, mirror_axis_end
        );

        if ( viennagrid::detail::is_equal(tol, cp.first, cp.second) )
        {

          if (viennagrid::detail::is_equal(tol, viennagrid::get_point(line,0), cp.first) ||
              viennagrid::detail::is_equal(tol, viennagrid::get_point(line,1), cp.first))
          {
            // one point of the line is on the mirror axis
            if (viennagrid::inner_prod(viennagrid::centroid(line)-centroid, normal) > 0)
              copy_map(line);

            if (viennagrid::detail::is_equal(tol, viennagrid::get_point(line,0), cp.first))
              vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(line)[0]);
            else
              vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(line)[1]);
          }
          else
          {
            ElementType new_vertex = viennagrid::make_unique_vertex( output_mesh(), cp.first, tol );
            vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = new_vertex;

            if (viennagrid::inner_prod(viennagrid::get_point(line,0)-centroid, normal) > 0)
              viennagrid::make_line( output_mesh(), new_vertex, viennagrid::vertices(line)[0] );
            else
              viennagrid::make_line( output_mesh(), new_vertex, viennagrid::vertices(line)[1] );
          }
        }
        else
        {
          if (viennagrid::inner_prod(viennagrid::centroid(line)-centroid, normal) > 0)
            copy_map(line);
        }
      }


      std::cout << "Vertices on mirror line: " << vertices_on_mirror_axis.size() << std::endl;

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



    if (!rotational_frequencies.empty() && !mirror_axis.empty())
    {
      info(1) << "Only using mirror symmetry and rotational symmetry" << std::endl;

      int mirror_axis_index = 0;
      data_handle<int> mirror_axis_index_input = get_input<int>("mirror_axis_index");
      if (mirror_axis_index_input)
        mirror_axis_index = mirror_axis_index_input();

      double used_axis = mirror_axis.front();
      int rotational_frequency = rotational_frequencies.back();

      set_output( "rotational_frequency", rotational_frequency);
      set_output( "mirror_axis", used_axis );
      set_output( "centroid", centroid );

      PointType vector_mirror = viennagrid::make_point( std::cos(used_axis), std::sin(used_axis) );
      PointType vector_angle = viennagrid::make_point( std::cos(used_axis + M_PI/rotational_frequency), std::sin(used_axis + M_PI/rotational_frequency) );

      PointType normal_mirror = vector_mirror;
      std::swap(normal_mirror[0], normal_mirror[1]);
      normal_mirror[0] = -normal_mirror[0];

      PointType normal_angle = vector_angle;
      std::swap(normal_angle[0], normal_angle[1]);
      normal_angle[0] = -normal_angle[0];



      PointType mirror_axis_end = centroid + vector_mirror*max_distance*1.2;
      PointType angle_axis_end = centroid + vector_angle*max_distance*1.2;

      info(1) << "Using mirror axis: " << centroid << " - " << mirror_axis_end << std::endl;
      info(1) << "Using angle axis: " << centroid << " - " << angle_axis_end << std::endl;


      std::map<double, ElementType> vertices_on_mirror_axis;
      std::map<double, ElementType> vertices_on_angle_axis;

      ElementType centrod_vertex = viennagrid::make_vertex( output_mesh(), centroid );
      vertices_on_mirror_axis[0] = centrod_vertex;
      vertices_on_angle_axis[0] = centrod_vertex;


      for (auto line : viennagrid::elements(input_mesh(),1))
      {
        std::cout << line << std::endl;

        std::pair<PointType, PointType> cp_mirror = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(line,0), viennagrid::get_point(line,1),
          centroid, mirror_axis_end
        );

        std::pair<PointType, PointType> cp_angle = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(line,0), viennagrid::get_point(line,1),
          centroid, angle_axis_end
        );

        if ( viennagrid::detail::is_equal(tol, cp_mirror.first, cp_mirror.second) &&
             viennagrid::detail::is_equal(tol, cp_angle.first, cp_angle.second) )
        {
          std::cout << "  Line crosses mirror and angle axis at " << cp_mirror.first << " and " << cp_angle.first << std::endl;

          ElementType v_mirror = viennagrid::make_unique_vertex( output_mesh(), cp_mirror.first, tol );
          vertices_on_mirror_axis[ viennagrid::inner_prod(cp_mirror.first-centroid, vector_mirror) ] = v_mirror;

          ElementType v_angle = viennagrid::make_unique_vertex( output_mesh(), cp_angle.first, tol );
          vertices_on_angle_axis[ viennagrid::inner_prod(cp_angle.first-centroid, vector_angle) ] = v_angle;

          viennagrid::make_line(  output_mesh(), v_mirror, v_angle );
        }
        else
        {
          if ( viennagrid::detail::is_equal(tol, cp_mirror.first, cp_mirror.second) )
          {
            std::cout << "  Line crosses mirror axis at " << cp_mirror.first << std::endl;

            ElementType v_mirror = viennagrid::make_unique_vertex( output_mesh(), cp_mirror.first, tol );
            vertices_on_mirror_axis[ viennagrid::inner_prod(cp_mirror.first-centroid, vector_mirror) ] = v_mirror;

            if (viennagrid::inner_prod( viennagrid::get_point(line,0)-centroid, normal_mirror ) > 0)
              viennagrid::make_line( output_mesh(), v_mirror, copy_map(viennagrid::vertices(line)[0]) );
            else
              viennagrid::make_line( output_mesh(), v_mirror, copy_map(viennagrid::vertices(line)[1]) );
          }
          else if ( viennagrid::detail::is_equal(tol, cp_angle.first, cp_angle.second) )
          {
            std::cout << "  Line crosses angle axis at " << cp_angle.first << std::endl;

            ElementType v_angle = viennagrid::make_unique_vertex( output_mesh(), cp_angle.first, tol );
            vertices_on_angle_axis[ viennagrid::inner_prod(cp_angle.first-centroid, vector_angle) ] = v_angle;

            if (viennagrid::inner_prod( viennagrid::get_point(line,0)-centroid, normal_angle ) < 0)
              viennagrid::make_line( output_mesh(), v_angle, copy_map(viennagrid::vertices(line)[0]) );
            else
              viennagrid::make_line( output_mesh(), v_angle, copy_map(viennagrid::vertices(line)[1]) );
          }
          else
          {
            std::cout << "  Line crosses no axis" << std::endl;
            std::cout << viennagrid::inner_prod( viennagrid::centroid(line)-centroid, normal_mirror ) << " " << viennagrid::inner_prod( viennagrid::centroid(line)-centroid, normal_angle ) << std::endl;

            if ( (viennagrid::inner_prod( viennagrid::centroid(line)-centroid, normal_mirror ) > 0) &&
                 (viennagrid::inner_prod( viennagrid::centroid(line)-centroid, normal_angle ) < 0) )
            {
              copy_map(line);
            }

          }
        }


      }


      std::cout << "Number of vertices on mirror axis: " << vertices_on_mirror_axis.size() << std::endl;
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

      std::cout << "Number of vertices on angle axis: " << vertices_on_angle_axis.size() << std::endl;
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


    std::cout << "Number of vertices: " << viennagrid::vertices( output_mesh() ).size() << std::endl;
    std::cout << "Number of lines: " << viennagrid::cells( output_mesh() ).size() << std::endl;

    set_output( "mesh", output_mesh );

    return true;
  }

}
