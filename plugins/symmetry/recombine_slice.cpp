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

#include "recombine_slice.hpp"
#include "viennagridpp/algorithm/distance.hpp"
#include "viennagridpp/algorithm/centroid.hpp"


namespace viennamesh
{


  template<typename PointT>
  PointT reflect(PointT const & point, PointT const & centroid, PointT const & axis)
  {
    return point - 2 * axis * viennagrid::inner_prod(axis, point-centroid);
//     return centroid + 2 * axis * viennagrid::inner_prod(point-centroid, axis) - (point-centroid);
  }

  template<typename PointT>
  PointT rotate(PointT const & point, PointT const & centroid, PointT const & axis, double angle)
  {
    PointT vector = point-centroid;
    PointT result(point.size());

    if (point.size() == 2)
    {
      result[0] = std::cos(angle) * vector[0] - std::sin(angle) * vector[1];
      result[1] = std::sin(angle) * vector[0] + std::cos(angle) * vector[1];
    }
    else if (point.size() == 3)
    {
      double cos_angle = std::cos(angle);
      double sin_angle = std::sin(angle);

      // http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
      result[0] = (axis[0]*axis[0] * (1-cos_angle) + cos_angle) * vector[0] +
                  (axis[0]*axis[1] * (1-cos_angle) - axis[2]*sin_angle) * vector[1] +
                  (axis[0]*axis[2] * (1-cos_angle) + axis[1]*sin_angle) * vector[2];

      result[1] = (axis[1]*axis[0] * (1-cos_angle) + axis[2]*sin_angle) * vector[0] +
                  (axis[1]*axis[1] * (1-cos_angle) + cos_angle) * vector[1] +
                  (axis[1]*axis[2] * (1-cos_angle) - axis[0]*sin_angle) * vector[2];

      result[2] = (axis[2]*axis[0] * (1-cos_angle) - axis[1]*sin_angle) * vector[0] +
                  (axis[2]*axis[1] * (1-cos_angle) + axis[0]*sin_angle) * vector[1] +
                  (axis[2]*axis[2] * (1-cos_angle) + cos_angle) * vector[2];
    }
    else
      assert(false);

    return centroid + result;
  }


  recombine_symmetric_slice::recombine_symmetric_slice() {}
  std::string recombine_symmetric_slice::name() { return "recombine_symmetric_slice"; }

  bool recombine_symmetric_slice::run(viennamesh::algorithm_handle &)
  {
    typedef viennagrid::mesh_t MeshType;
    typedef viennagrid::result_of::point<MeshType>::type PointType;
    typedef viennagrid::result_of::element<MeshType>::type ElementType;

    typedef viennagrid::result_of::const_element_range<MeshType>::type ConstElementRangeType;
    typedef viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

    typedef viennagrid::result_of::const_element_range<ElementType>::type ConstBoundaryElementRangeType;
    typedef viennagrid::result_of::iterator<ConstBoundaryElementRangeType>::type ConstBoundaryElementIteratorType;

    double tol = 1e-6;

    mesh_handle input_mesh = get_required_input<mesh_handle>("mesh");

    int geometric_dimension = viennagrid::geometric_dimension(input_mesh());
    int cell_dimension = viennagrid::cell_dimension(input_mesh());

    int rotational_frequency = 0;
    bool rotational_frequency_used = false;
    bool half_slice = false;

    if (get_input<bool>("half_slice").valid())
      half_slice = get_input<bool>("half_slice")();

    PointType mirror_axis;

    if (get_input<PointType>("mirror_axis").valid())
    {
      mirror_axis = get_input<PointType>("mirror_axis")();
    }
    else
    {
      if (geometric_dimension == 3)
        error(1) << "Geometric dimension is 3 and no mirror axis is specified" << std::endl;

      if ( (geometric_dimension == 2) && half_slice )
        error(1) << "Geometric dimension is 2, half slice is activated and no mirror axis is specified" << std::endl;

      return false;
    }

    mirror_axis.normalize();


    if (get_input<int>("rotational_frequency").valid())
    {
      rotational_frequency = get_input<int>("rotational_frequency")();
      rotational_frequency_used = true;
    }


    point_handle input_centroid = get_input<point_handle>("centroid");
    PointType centroid(geometric_dimension);
    if (input_centroid.valid())
      centroid = input_centroid();

    if (rotational_frequency_used)
      info(1) << "rotational_frequency " << rotational_frequency << std::endl;
    info(1) << "mirror_axis " << mirror_axis << std::endl;
    info(1) << "centroid " << centroid << std::endl;

    mesh_handle output_mesh = make_data<mesh_handle>();
    viennagrid::result_of::element_copy_map<>::type copy_map( output_mesh(), tol );

    ConstElementRangeType cells(input_mesh(), cell_dimension);
    for (ConstElementIteratorType cit = cells.begin(); cit != cells.end(); ++cit)
    {
      copy_map(*cit);

      if (half_slice)
      {
        ConstBoundaryElementRangeType vertices(*cit, 0);
        std::vector<ElementType> mirrored_vertices( vertices.size() );

        int index = 0;
        for (ConstBoundaryElementIteratorType bvit = vertices.begin(); bvit != vertices.end(); ++bvit, ++index)
          mirrored_vertices[index] = viennagrid::make_unique_vertex(
                output_mesh(),
                reflect(viennagrid::get_point(*bvit), centroid, mirror_axis),
                tol );

        viennagrid::make_element( output_mesh(), (*cit).tag(), mirrored_vertices.begin(), mirrored_vertices.end() );
      }
    }

    std::vector<ElementType> slice_cells;
    ConstElementRangeType output_cells(input_mesh(), cell_dimension);
    for (ConstElementIteratorType cit = output_cells.begin(); cit != output_cells.end(); ++cit)
      slice_cells.push_back(*cit);

    for (std::vector<ElementType>::iterator cit = slice_cells.begin(); cit != slice_cells.end(); ++cit)
    {
      for (int i = 1; i != rotational_frequency; ++i)
      {
        double angle = 2*M_PI*static_cast<double>(i)/static_cast<double>(rotational_frequency);

        ConstBoundaryElementRangeType vertices(*cit, 0);
        std::vector<ElementType> mirrored_vertices( vertices.size() );

        int index = 0;
        for (ConstBoundaryElementIteratorType bvit = vertices.begin(); bvit != vertices.end(); ++bvit, ++index)
        {
          PointType output_point = rotate(viennagrid::get_point(*bvit), centroid, mirror_axis, angle);

          mirrored_vertices[index] = viennagrid::make_unique_vertex(
                output_mesh(),
                output_point,
                tol );
        }

        viennagrid::make_element( output_mesh(), (*cit).tag(), mirrored_vertices.begin(), mirrored_vertices.end() );
      }
    }

    set_output( "mesh", output_mesh );

    return true;
  }

}
