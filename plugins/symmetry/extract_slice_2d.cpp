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

    if (rotational_frequencies.empty() && !mirror_axis.empty())
    {
      info(1) << "Only using mirror symmetry, no rotational symmetry" << std::endl;
      double used_axis = mirror_axis.front();

      PointType vector = viennagrid::make_point( std::cos(used_axis), std::sin(used_axis) );
      PointType normal = vector;
      std::swap(normal[0], normal[1]);
      normal[0] = -normal[0];

      PointType mirror_axis_start = centroid - vector*max_distance*1.2;
      PointType mirror_axis_end = centroid + vector*max_distance*1.2;

      info(1) << "Using mirror axis: " << mirror_axis_start << " - " << mirror_axis_end << std::endl;
      mesh_handle output_mesh = make_data<mesh_handle>();
      viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh() );

//       std::vector<ElementType> vertices_on_mirror_axis;
      std::map<double, ElementType> vertices_on_mirror_axis;

//       for (auto vertex : viennagrid::vertices(input_mesh()))
//       {
//         std::pair<PointType, PointType> cp = viennagrid::detail::closest_points_point_line(
//           viennagrid::get_point(vertex),
//           mirror_axis_start, mirror_axis_end
//         );
//
//         if ( viennagrid::detail::is_equal(1e-8, cp.first, cp.second) )
//           vertices_on_mirror_axis.push_back( copy_map(vertex) );
//       }

      for (auto line : viennagrid::elements(input_mesh(),1))
      {
        std::pair<PointType, PointType> cp = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(line,0), viennagrid::get_point(line,1),
          mirror_axis_start, mirror_axis_end
        );

        if ( viennagrid::detail::is_equal(1e-8, cp.first, cp.second) )
        {

          if (viennagrid::detail::is_equal(1e-8, viennagrid::get_point(line,0), cp.first) ||
              viennagrid::detail::is_equal(1e-8, viennagrid::get_point(line,1), cp.first))
          {
            // one point of the line is on the mirror axis
            if (viennagrid::inner_prod(viennagrid::centroid(line)-centroid, normal) > 0)
              copy_map(line);

            if (viennagrid::detail::is_equal(1e-8, viennagrid::get_point(line,0), cp.first))
              vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(line)[0]);
            else
              vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(line)[1]);
          }
          else
          {
            ElementType new_vertex = viennagrid::make_unique_vertex( output_mesh(), cp.first, 1e-6 );
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


      set_output( "mesh", output_mesh );
    }



    if (!rotational_frequencies.empty() && !mirror_axis.empty())
    {
      info(1) << "Only using mirror symmetry and rotational symmetry" << std::endl;

      double used_axis = mirror_axis.front();
      int rotational_frequency = rotational_frequencies.back();

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




      for (auto line : viennagrid::elements(input_mesh(),1))
      {
        std::pair<PointType, PointType> cp_mirror = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(line,0), viennagrid::get_point(line,1),
          centroid, mirror_axis_end
        );

        std::pair<PointType, PointType> cp_angle = viennagrid::detail::closest_points_line_line(
          viennagrid::get_point(line,0), viennagrid::get_point(line,1),
          centroid, mirror_axis_end
        );

//         if ( viennagrid::detail::is_equal(1e-8, cp.first, cp.second) )
//         {
//
//           if (viennagrid::detail::is_equal(1e-8, viennagrid::get_point(line,0), cp.first) ||
//               viennagrid::detail::is_equal(1e-8, viennagrid::get_point(line,1), cp.first))
//           {
//             // one point of the line is on the mirror axis
//             if (viennagrid::inner_prod(viennagrid::centroid(line)-centroid, normal) > 0)
//               copy_map(line);
//
//             if (viennagrid::detail::is_equal(1e-8, viennagrid::get_point(line,0), cp.first))
//               vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(line)[0]);
//             else
//               vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = copy_map( viennagrid::vertices(line)[1]);
//           }
//           else
//           {
//             ElementType new_vertex = viennagrid::make_unique_vertex( output_mesh(), cp.first, 1e-6 );
//             vertices_on_mirror_axis[ viennagrid::inner_prod(cp.first-centroid, vector) ] = new_vertex;
//
//             if (viennagrid::inner_prod(viennagrid::get_point(line,0)-centroid, normal) > 0)
//               viennagrid::make_line( output_mesh(), new_vertex, viennagrid::vertices(line)[0] );
//             else
//               viennagrid::make_line( output_mesh(), new_vertex, viennagrid::vertices(line)[1] );
//           }
//         }
//         else
//         {
//           if (viennagrid::inner_prod(viennagrid::centroid(line)-centroid, normal) > 0)
//             copy_map(line);
//         }
      }


    }




//     if (mirror_axis.empty() || )


//     std::sort( mirror_axis.begin(), mirror_axis.end() );
//     std::sort( rotational_frequencies.begin(), rotational_frequencies.end() );
//
//     int highest_rotational_frequency = rotational_frequencies.back();
//     double rotation_angle = 2*M_PI / static_cast<double>(highest_rotational_frequency);
//     double mirror_axis = mirror_axis.front();


    return true;
  }

}
