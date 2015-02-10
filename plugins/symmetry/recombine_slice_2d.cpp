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

#include "recombine_slice_2d.hpp"
#include "viennagrid/algorithm/distance.hpp"
#include "viennagrid/algorithm/centroid.hpp"


namespace viennamesh
{


  template<typename PointT>
  PointT reflect( PointT const & point, PointT const & centroid, PointT const & axis )
  {
    return centroid + 2 * axis * viennagrid::inner_prod(point-centroid, axis) - (point-centroid);
  }

  template<typename PointT>
  PointT rotate(PointT const & point, PointT const & centroid, double angle)
  {
    PointT vector = point-centroid;

    PointT result = viennagrid::make_point(0,0);
    result[0] = std::cos(angle) * vector[0] - std::sin(angle) * vector[1];
    result[1] = std::sin(angle) * vector[0] + std::cos(angle) * vector[1];

    return centroid + result;
  }


  recombine_symmetric_slice_2d::recombine_symmetric_slice_2d() {}
  std::string recombine_symmetric_slice_2d::name() { return "recombine_symmetric_slice_2d"; }

  bool recombine_symmetric_slice_2d::run(viennamesh::algorithm_handle &)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    double tol = 1e-6;

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    int rotational_frequency;
    bool rotational_frequency_used = false;
    double mirror_axis;
    bool mirror_axis_used = false;

    if (get_input<int>("rotational_frequency"))
    {
      rotational_frequency = get_input<int>("rotational_frequency")();
      rotational_frequency_used = true;
    }

    if (get_input<double>("mirror_axis"))
    {
      mirror_axis = get_input<double>("mirror_axis")();
      mirror_axis_used = true;
    }

    point_container_handle input_centroid = get_required_input<point_container_handle>("centroid");
    PointType centroid;
    convert( input_centroid(), centroid );

    std::cout << "rotational_frequency " << rotational_frequency << std::endl;
    std::cout << "mirror_axis " << mirror_axis << std::endl;
    std::cout << "centroid " << centroid << std::endl;

    mesh_handle output_mesh = make_data<mesh_handle>();
    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), tol );

    if (rotational_frequency_used && mirror_axis_used)
    {
      PointType mirror_vector = viennagrid::make_point( std::cos(mirror_axis), std::sin(mirror_axis) );
      for (auto triangle : viennagrid::cells( input_mesh() ))
      {
        copy_map(triangle);

        ElementType v0 = viennagrid::make_unique_vertex( output_mesh(), reflect(viennagrid::get_point(triangle, 0), centroid, mirror_vector), tol );
        ElementType v1 = viennagrid::make_unique_vertex( output_mesh(), reflect(viennagrid::get_point(triangle, 1), centroid, mirror_vector), tol );
        ElementType v2 = viennagrid::make_unique_vertex( output_mesh(), reflect(viennagrid::get_point(triangle, 2), centroid, mirror_vector), tol );

        viennagrid::make_triangle( output_mesh(), v0, v1, v2 );
      }

      std::vector<ElementType> slice_triangles;
      for (auto triangle : viennagrid::cells( output_mesh() ) )
        slice_triangles.push_back(triangle);

      for (auto triangle : slice_triangles)
      {
        for (int i = 1; i != rotational_frequency; ++i)
        {
          double angle = 2*M_PI*static_cast<double>(i)/static_cast<double>(rotational_frequency);

          ElementType v0 = viennagrid::make_unique_vertex( output_mesh(), rotate(viennagrid::get_point(triangle, 0), centroid, angle), tol );
          ElementType v1 = viennagrid::make_unique_vertex( output_mesh(), rotate(viennagrid::get_point(triangle, 1), centroid, angle), tol );
          ElementType v2 = viennagrid::make_unique_vertex( output_mesh(), rotate(viennagrid::get_point(triangle, 2), centroid, angle), tol );

          viennagrid::make_triangle( output_mesh(), v0, v1, v2 );
        }
      }
    }



    set_output( "mesh", output_mesh );

    return true;
  }

}
