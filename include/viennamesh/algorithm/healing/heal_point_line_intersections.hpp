#ifndef VIENNAMESH_ALGORITHM_HEALING_POINT_LINE_INTERSECTIONS_HPP
#define VIENNAMESH_ALGORITHM_HEALING_POINT_LINE_INTERSECTIONS_HPP

#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"
#include "viennagrid/algorithm/refine.hpp"

namespace viennamesh
{
  template<typename PointT, typename NumericConfigT>
  bool point_line_intersect(PointT const & point,
                            PointT const & line_start, PointT const & line_end,
                            NumericConfigT nc)
  {
    return viennagrid::detail::is_equal( nc,
                                         viennagrid::norm_2(line_end-line_start),
                                         viennagrid::norm_2(line_start-point)+viennagrid::norm_2(line_end-point) );
  }



  template<typename NumericConfigT>
  struct point_line_intersection_heal_functor
  {
    point_line_intersection_heal_functor(NumericConfigT nc_) : nc(nc_) {}

    template<typename MeshT>
    std::size_t operator()(MeshT const & mesh) const
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      typedef typename viennagrid::result_of::const_line_range<MeshT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

      typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

      std::size_t intersection_count = 0;
      ConstLineRangeType lines(mesh);
      ConstVertexRangeType vertices(mesh);

      for (ConstLineRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
      {
        PointType const & line_start = viennagrid::point( viennagrid::vertices(*lit)[0] );
        PointType const & line_end = viennagrid::point( viennagrid::vertices(*lit)[1] );

        for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
        {
          if (vit.handle() == viennagrid::vertices(*lit).handle_at(0) ||
              vit.handle() == viennagrid::vertices(*lit).handle_at(1))
            continue;

          PointType const & point = viennagrid::point(*vit);

          if (point_line_intersect(point, line_start, line_end, nc))
          {
            ++intersection_count;
            break;
          }
        }
      }

      return intersection_count;
    }

    template<typename MeshT>
    void operator()(MeshT const & input_mesh, MeshT & output_mesh) const
    {
      viennagrid::clear(output_mesh);

      typedef typename viennagrid::result_of::cell_tag<MeshT>::type CellTag;
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      typedef typename viennagrid::result_of::line<MeshT>::type LineType;
      typedef typename viennagrid::result_of::const_line_range<MeshT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

      typedef typename viennagrid::result_of::const_vertex_range<MeshT>::type ConstVertexRangeType;
      typedef typename viennagrid::result_of::iterator<ConstVertexRangeType>::type ConstVertexRangeIterator;

      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;


      std::deque<bool> line_refinement_tag_container;
      typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

      std::deque<VertexHandleType> line_refinement_vertex_handle_container;
      typename viennagrid::result_of::accessor<std::deque<VertexHandleType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);


      viennagrid::vertex_copy_map<MeshT, MeshT> vertex_map(output_mesh);

      ConstLineRangeType lines(input_mesh);
      ConstVertexRangeType vertices(input_mesh);

      for (ConstLineRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
      {
        PointType const & line_start = viennagrid::point( viennagrid::vertices(*lit)[0] );
        PointType const & line_end = viennagrid::point( viennagrid::vertices(*lit)[1] );

        line_refinement_tag_accessor(*lit) = false;
        for (ConstVertexRangeIterator vit = vertices.begin(); vit != vertices.end(); ++vit)
        {
          if (vit.handle() == viennagrid::vertices(*lit).handle_at(0) ||
              vit.handle() == viennagrid::vertices(*lit).handle_at(1))
            continue;

          PointType const & point = viennagrid::point(*vit);

          if (point_line_intersect(point, line_start, line_end, nc))
          {
            line_refinement_tag_accessor(*lit) = true;
            line_refinement_vertex_handle_accessor(*lit) = vertex_map(*vit);
            break;
          }
        }
      }

      viennagrid::simple_refine<CellTag>(input_mesh, output_mesh,
                                        vertex_map,
                                        line_refinement_tag_accessor,
                                        line_refinement_vertex_handle_accessor);
    }

    NumericConfigT nc;
  };
}

#endif
