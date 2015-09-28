#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_GEOMETRY_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_GEOMETRY_HPP

#include "viennagrid/algorithm/distance.hpp"

namespace viennamesh
{



  // determines if an element is on a plane
  template<typename ElementT, typename PointT, typename NumericConfigT>
  bool is_on_plane(ElementT const & element,
                   PointT const & point, PointT const & normal,
                   NumericConfigT numeric_config)
  {
    typedef typename viennagrid::result_of::coord<PointT>::type CoordType;

    CoordType max_distance;

    if (element.tag().is_vertex())
    {
      max_distance = std::abs( viennagrid::inner_prod(viennagrid::get_point(element)-point, normal) );
    }
    else
    {
      typedef typename viennagrid::result_of::const_element_range<ElementT>::type ConstElementRangeType;
      typedef typename viennagrid::result_of::iterator<ConstElementRangeType>::type ConstElementIteratorType;

      ConstElementRangeType vertices(element, 0);
      ConstElementIteratorType vit = vertices.begin();

      max_distance = std::abs( viennagrid::inner_prod(viennagrid::get_point(*vit)-point, normal) );
      ++vit;

      for (; vit != vertices.end(); ++vit)
      {
        CoordType distance = std::abs( viennagrid::inner_prod(viennagrid::get_point(*vit)-point, normal) );
        max_distance = std::max(max_distance, distance);
      }
    }

//     std::cout << "Element " << element << " has max distance to plane " << max_distance << std::endl;
    return max_distance < viennagrid::detail::absolute_tolerance<CoordType>(numeric_config);
  }




  // reflect a point using a centroid and an axis
  template<typename PointT>
  PointT reflect(PointT const & point, PointT const & centroid, PointT const & axis)
  {
    return point - 2 * axis * viennagrid::inner_prod(axis, point-centroid);
  }

  // reflect a point using a centroid and an axis
  template<typename PointT>
  PointT reflect(PointT const & point, PointT const & axis)
  {
    return reflect(point, PointT(point.size()), axis);
  }


  // rotates a point by angle using a centroid and an axis
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

  template<typename PointT>
  PointT rotate(PointT const & vector, PointT const & axis, double angle)
  {
    PointT result(vector.size());

    if (vector.size() == 2)
    {
      result[0] = std::cos(angle) * vector[0] - std::sin(angle) * vector[1];
      result[1] = std::sin(angle) * vector[0] + std::cos(angle) * vector[1];
    }
    else if (vector.size() == 3)
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

    return result;
  }


  template<bool mesh_is_const>
  double distance(viennagrid::base_mesh<mesh_is_const> const & mesh, viennagrid::point const & pt)
  {
    typedef viennagrid::base_mesh<mesh_is_const> MeshType;

    typedef typename viennagrid::result_of::const_cell_range<MeshType>::type ConstCellRangeType;
    typedef typename viennagrid::result_of::iterator<ConstCellRangeType>::type ConstCellRangeIterator;

    ConstCellRangeType cells(mesh);
    if (cells.empty())
      return -1;

    ConstCellRangeIterator cit = cells.begin();
    double d = viennagrid::distance(*cit, pt);
    ++cit;

    for (; cit != cells.end(); ++cit)
    {
      double tmp = viennagrid::distance(*cit, pt);
      d = std::min(d, tmp);
    }

    return d;
  }

}


#endif
