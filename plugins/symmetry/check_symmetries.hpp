#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_CHECK_SYMMETRIES_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_CHECK_SYMMETRIES_HPP

#include "viennagridpp/algorithm/distance.hpp"

namespace viennamesh
{

  viennagrid::point_t reflect(viennagrid::point_t const & pt, viennagrid::point_t const & axis)
  {
    return pt - 2.0 * axis * viennagrid::inner_prod(pt, axis);
  }


  template<bool mesh_is_const>
  double distance(viennagrid::base_mesh<mesh_is_const> const & mesh, viennagrid::point_t const & pt)
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


  template<bool mesh_is_const>
  bool check_mirror(viennagrid::base_mesh<mesh_is_const> const & mesh,
                    viennagrid::point_t axis,
                    double tolerance)
  {
    axis /= viennagrid::norm_2(axis);

    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::point<MeshType>::type PointType;

    typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

    double max = -1.0;
    ConstVertexRangeType vertices(mesh);
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType reflected = reflect( viennagrid::get_point(*vit), axis );
      double d = distance(mesh, reflected);
      max = std::max(max, d);
    }

    std::cout << "Max distance = " << max  << std::endl;
    return max < tolerance;
  }



  template<bool mesh_is_const>
  bool check_rotational(viennagrid::base_mesh<mesh_is_const> const & mesh,
                        viennagrid::point_t axis, int frequency,
                        double tolerance)
  {
    axis /= viennagrid::norm_2(axis);

    double angle = 2*M_PI / frequency;
    double cos_angle = std::cos(angle);
    double sin_angle = std::sin(angle);

    ublas::matrix<double> rot(3,3);

    rot(0,0) = axis[0]*axis[0] * (1-cos_angle) + cos_angle;
    rot(0,1) = axis[0]*axis[1] * (1-cos_angle) - axis[2]*sin_angle;
    rot(0,2) = axis[0]*axis[2] * (1-cos_angle) + axis[1]*sin_angle;

    rot(1,0) = axis[1]*axis[0] * (1-cos_angle) + axis[2]*sin_angle;
    rot(1,1) = axis[1]*axis[1] * (1-cos_angle) + cos_angle;
    rot(1,2) = axis[1]*axis[2] * (1-cos_angle) - axis[0]*sin_angle;

    rot(2,0) = axis[2]*axis[0] * (1-cos_angle) - axis[1]*sin_angle;
    rot(2,1) = axis[2]*axis[1] * (1-cos_angle) + axis[0]*sin_angle;
    rot(2,2) = axis[2]*axis[2] * (1-cos_angle) + cos_angle;


    typedef viennagrid::base_mesh<mesh_is_const> MeshType;
    typedef typename viennagrid::result_of::point<MeshType>::type PointType;

    typedef typename viennagrid::result_of::const_vertex_range<MeshType>::type ConstVertexRangeType;
    typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

    double max = -1.0;
    ConstVertexRangeType vertices(mesh);
    for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
    {
      PointType reflected = ublas::prod(rot, viennagrid::get_point(*vit));
      double d = distance(mesh, reflected);
      max = std::max(max, d);
    }

    std::cout << "Max distance = " << max  << std::endl;
    return max < tolerance;
  }


}

#endif
