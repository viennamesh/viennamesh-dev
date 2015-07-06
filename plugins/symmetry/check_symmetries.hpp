#ifndef VIENNAMESH_ALGORITHM_SYMMETRY_CHECK_SYMMETRIES_HPP
#define VIENNAMESH_ALGORITHM_SYMMETRY_CHECK_SYMMETRIES_HPP

#include "geometry.hpp"

namespace viennamesh
{
  template<bool mesh_is_const>
  bool check_mirror(viennagrid::base_mesh<mesh_is_const> const & mesh,
                    viennagrid::point axis,
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
                        viennagrid::point axis, int frequency,
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
