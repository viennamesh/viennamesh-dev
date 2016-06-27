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

#include "geometry.hpp"

namespace viennamesh
{
//   template<typename PointT>
//   PointT rotate(PointT const & point, PointT const & centroid, double angle)
//   {
//     PointT vector = point-centroid;
//
//     PointT result = viennagrid::make_point(0,0);
//     result[0] = std::cos(angle) * vector[0] - std::sin(angle) * vector[1];
//     result[1] = std::sin(angle) * vector[0] + std::cos(angle) * vector[1];
//
//     return centroid + result;
//   }


  recombine_symmetric_slice_2d::recombine_symmetric_slice_2d() {}
  std::string recombine_symmetric_slice_2d::name() { return "recombine_symmetric_slice_2d"; }

  bool recombine_symmetric_slice_2d::run(viennamesh::algorithm_handle &)
  {
    typedef viennagrid::mesh                                                MeshType;
    typedef viennagrid::result_of::point<MeshType>::type                    PointType;
    typedef viennagrid::result_of::element<MeshType>::type                  ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type      ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type    ConstElementIteratorType;

    double tol = 1e-6;

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    int rotational_frequency = 0;
    bool rotational_frequency_used = false;
    double mirror_axis = 0.0;
    bool mirror_axis_used = false;

    if (get_input<int>("rotational_frequency").valid())
    {
      rotational_frequency = get_input<int>("rotational_frequency")();
      rotational_frequency_used = true;
    }

    if (get_input<double>("mirror_axis").valid())
    {
      mirror_axis = get_input<double>("mirror_axis")();
      mirror_axis_used = true;
    }

    point_handle input_centroid = get_required_input<point_handle>("centroid");
    PointType centroid = input_centroid();

    if (rotational_frequency_used)
      info(1) << "rotational_frequency " << rotational_frequency << std::endl;
    if (mirror_axis_used)
      info(1) << "mirror_axis " << mirror_axis << std::endl;
    info(1) << "centroid " << centroid << std::endl;

    mesh_handle output_mesh = make_data<mesh_handle>();
    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), tol );

    ConstElementRangeType triangles(input_mesh(), 2);
    for (ConstElementIteratorType tit = triangles.begin(); tit != triangles.end(); ++tit)
    {
      copy_map(*tit);

      if (mirror_axis_used)
      {
        PointType mirror_vector = viennagrid::make_point( std::cos(mirror_axis), std::sin(mirror_axis) );

        ElementType v0 = viennagrid::make_unique_vertex( output_mesh(), reflect(viennagrid::get_point(*tit, 0), centroid, mirror_vector), tol );
        ElementType v1 = viennagrid::make_unique_vertex( output_mesh(), reflect(viennagrid::get_point(*tit, 1), centroid, mirror_vector), tol );
        ElementType v2 = viennagrid::make_unique_vertex( output_mesh(), reflect(viennagrid::get_point(*tit, 2), centroid, mirror_vector), tol );

        viennagrid::make_triangle( output_mesh(), v0, v1, v2 );
      }
    }

    std::vector<ElementType> slice_triangles;
    ConstElementRangeType output_triangles(input_mesh(), 2);
    for (ConstElementIteratorType tit = output_triangles.begin(); tit != output_triangles.end(); ++tit)
      slice_triangles.push_back(*tit);

    for (std::vector<ElementType>::iterator tit = slice_triangles.begin(); tit != slice_triangles.end(); ++tit)
    {
      for (int i = 1; i != rotational_frequency; ++i)
      {
        double angle = 2*M_PI*static_cast<double>(i)/static_cast<double>(rotational_frequency);

        ElementType v0 = viennagrid::make_unique_vertex( output_mesh(), rotate(viennagrid::get_point(*tit, 0), centroid, angle), tol );
        ElementType v1 = viennagrid::make_unique_vertex( output_mesh(), rotate(viennagrid::get_point(*tit, 1), centroid, angle), tol );
        ElementType v2 = viennagrid::make_unique_vertex( output_mesh(), rotate(viennagrid::get_point(*tit, 2), centroid, angle), tol );

        viennagrid::make_triangle( output_mesh(), v0, v1, v2 );
      }
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
