#ifndef VIENNAMESH_ALGORITHM_HEALING_POINT_TRIANGLE_INTERSECTIONS_HPP
#define VIENNAMESH_ALGORITHM_HEALING_POINT_TRIANGLE_INTERSECTIONS_HPP


#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"
#include "viennagrid/algorithm/refine.hpp"

namespace viennamesh
{
  template<typename PointT, typename NumericConfigT>
  bool point_triangle_intersect(PointT const & point,
                                PointT const & p0, PointT const & p1, PointT const & p2,
                                NumericConfigT nc)
  {
    return viennagrid::detail::is_equal( nc,
                                         viennagrid::spanned_volume(p0, p1, p2),
                                         viennagrid::spanned_volume(p1, p2, point) + viennagrid::spanned_volume(p0, p2, point) + viennagrid::spanned_volume(p0, p1, point) );
  }



  template<typename NumericConfigT>
  struct point_triangle_intersection_heal_functor
  {
    point_triangle_intersection_heal_functor(NumericConfigT nc_) : nc(nc_) {}

    template<typename MeshT>
    bool operator()(MeshT const & mesh) const
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      typedef typename viennagrid::result_of::const_triangle_range<MeshT>::type ConstTriangleRangeType;
      typedef typename viennagrid::result_of::iterator<ConstTriangleRangeType>::type ConstTriangleRangeIterator;

      typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

      ConstTriangleRangeType triangles(mesh);
      ConstVertexRangeType vertices(mesh);

      for (ConstTriangleRangeIterator tit = triangles.begin(); tit != triangles.end(); ++tit)
      {
        PointType const & p0 = viennagrid::point( viennagrid::vertices(*tit)[0] );
        PointType const & p1 = viennagrid::point( viennagrid::vertices(*tit)[1] );
        PointType const & p2 = viennagrid::point( viennagrid::vertices(*tit)[2] );

        for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
        {
          if (vit.handle() == viennagrid::vertices(*tit).handle_at(0) ||
              vit.handle() == viennagrid::vertices(*tit).handle_at(1) ||
              vit.handle() == viennagrid::vertices(*tit).handle_at(2) )
            continue;

          PointType const & point = viennagrid::point(*vit);

          if (point_triangle_intersect(point, p0, p1, p2, nc))
          {
            info(1) << "Found point-triangle intersection: " << std::endl;
            info(1) << "  " << point << std::endl;
            info(1) << "  [" << p0 << "," << p1 << "," << p2 << "]"  << std::endl;
            return false;
          }
        }
      }

      return true;
    }

    template<typename MeshT>
    std::size_t operator()(MeshT const & input_mesh, MeshT & output_mesh) const
    {
      viennagrid::clear(output_mesh);

      typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      typedef typename viennagrid::result_of::const_triangle_range<MeshT>::type ConstTriangleRangeType;
      typedef typename viennagrid::result_of::iterator<ConstTriangleRangeType>::type ConstTriangleRangeIterator;

      typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;

      viennagrid::vertex_copy_map<MeshT, MeshT> vertex_map(output_mesh);

      ConstTriangleRangeType triangles(input_mesh);
      ConstVertexRangeType vertices(input_mesh);

      std::size_t intersection_count = 0;
      for (ConstTriangleRangeIterator tit = triangles.begin(); tit != triangles.end(); ++tit)
      {
        PointType const & p0 = viennagrid::point( viennagrid::vertices(*tit)[0] );
        PointType const & p1 = viennagrid::point( viennagrid::vertices(*tit)[1] );
        PointType const & p2 = viennagrid::point( viennagrid::vertices(*tit)[2] );

        ConstVertexRangeIterator vit = vertices.begin();
        for (; vit != vertices.end(); ++vit)
        {
          if (vit.handle() == viennagrid::vertices(*tit).handle_at(0) ||
              vit.handle() == viennagrid::vertices(*tit).handle_at(1) ||
              vit.handle() == viennagrid::vertices(*tit).handle_at(2) )
            continue;



          PointType const & point = viennagrid::point(*vit);

          if (point_triangle_intersect(point, p0, p1, p2, nc))
          {
            VertexHandleType vh0 = vertex_map( viennagrid::vertices(*tit)[0] );
            VertexHandleType vh1 = vertex_map( viennagrid::vertices(*tit)[1] );
            VertexHandleType vh2 = vertex_map( viennagrid::vertices(*tit)[2] );

            VertexHandleType vp = vertex_map(*vit);

            viennagrid::make_triangle( output_mesh, vh0, vh1, vp );
            viennagrid::make_triangle( output_mesh, vh0, vp, vh2 );
            viennagrid::make_triangle( output_mesh, vp, vh1, vh2 );

            ++intersection_count;
            break;
          }
        }

        if (vit == vertices.end())
          vertex_map.copy_element(*tit);
      }

      return intersection_count;
    }

    NumericConfigT nc;
  };
}


#endif
