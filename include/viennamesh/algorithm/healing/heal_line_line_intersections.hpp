#ifndef VIENNAMESH_ALGORITHM_HEALING_LINE_LINE_INTERSECTIONS_HPP
#define VIENNAMESH_ALGORITHM_HEALING_LINE_LINE_INTERSECTIONS_HPP

#include "viennagrid/algorithm/norm.hpp"
#include "viennagrid/algorithm/closest_points.hpp"
#include "viennagrid/algorithm/detail/numeric.hpp"
#include "viennagrid/algorithm/refine.hpp"

// #include "viennamesh/algorithm/healing/common.hpp"

namespace viennamesh
{
  template<typename NumericConfigT>
  struct line_line_intersection_heal_functor
  {
    line_line_intersection_heal_functor(NumericConfigT nc_) : nc(nc_) {}


    template<typename MeshT>
    bool operator()(MeshT const & mesh) const
    {
      typedef typename viennagrid::result_of::point<MeshT>::type PointType;

      typedef typename viennagrid::result_of::const_line_range<MeshT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

      ConstLineRangeType lines(mesh);

      for (ConstLineRangeIterator lit0 = lines.begin(); lit0 != lines.end(); ++lit0)
      {
        PointType const & line0_start = viennagrid::point( viennagrid::vertices(*lit0)[0] );
        PointType const & line0_end = viennagrid::point( viennagrid::vertices(*lit0)[1] );

        ConstLineRangeIterator lit1 = lit0; ++lit1;
        for (; lit1 != lines.end(); ++lit1)
        {
          PointType const & line1_start = viennagrid::point( viennagrid::vertices(*lit1)[0] );
          PointType const & line1_end = viennagrid::point( viennagrid::vertices(*lit1)[1] );

          if ( viennagrid::detail::is_equal_point(line0_start, line1_start, nc) ||
               viennagrid::detail::is_equal_point(line0_end, line1_end, nc) ||
               viennagrid::detail::is_equal_point(line0_start, line1_end, nc) ||
               viennagrid::detail::is_equal_point(line0_end, line1_start, nc) )
            continue;

          std::pair<PointType, PointType> closest_points = viennagrid::closest_points( *lit0, *lit1 );
          if ( viennagrid::detail::is_equal_point(closest_points.first, closest_points.second, nc) )
          {
            info(1) << "Found line-line intersection: " << std::endl;
            info(1) << "  [" << line0_start << "," << line0_end << "]"  << std::endl;
            info(1) << "  [" << line1_start << "," << line1_end << "]"  << std::endl;
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

      typedef typename viennagrid::result_of::line<MeshT>::type LineType;
      typedef typename viennagrid::result_of::const_line_range<MeshT>::type ConstLineRangeType;
      typedef typename viennagrid::result_of::iterator<ConstLineRangeType>::type ConstLineRangeIterator;

      typedef typename viennagrid::result_of::vertex_handle<MeshT>::type VertexHandleType;


      std::deque<bool> line_refinement_tag_container;
      typename viennagrid::result_of::accessor<std::deque<bool>, LineType>::type line_refinement_tag_accessor(line_refinement_tag_container);

      std::deque<VertexHandleType> line_refinement_vertex_handle_container;
      typename viennagrid::result_of::accessor<std::deque<VertexHandleType>, LineType>::type line_refinement_vertex_handle_accessor(line_refinement_vertex_handle_container);


      viennagrid::vertex_copy_map<MeshT, MeshT> vertex_map(output_mesh);

      std::size_t intersection_count = 0;
      ConstLineRangeType lines(input_mesh);
      for (ConstLineRangeIterator lit0 = lines.begin(); lit0 != lines.end(); ++lit0)
      {
        if (line_refinement_tag_accessor(*lit0))
          continue;

        PointType const & line0_start = viennagrid::point( viennagrid::vertices(*lit0)[0] );
        PointType const & line0_end = viennagrid::point( viennagrid::vertices(*lit0)[1] );

        ConstLineRangeIterator lit1 = lit0; ++lit1;
        for (; lit1 != lines.end(); ++lit1)
        {
          if (line_refinement_tag_accessor(*lit1))
            continue;

          PointType const & line1_start = viennagrid::point( viennagrid::vertices(*lit1)[0] );
          PointType const & line1_end = viennagrid::point( viennagrid::vertices(*lit1)[1] );


          if ( viennagrid::detail::is_equal_point(line0_start, line1_start, nc) ||
               viennagrid::detail::is_equal_point(line0_end, line1_end, nc) ||
               viennagrid::detail::is_equal_point(line0_start, line1_end, nc) ||
               viennagrid::detail::is_equal_point(line0_end, line1_start, nc) )
            continue;

          std::pair<PointType, PointType> closest_points = viennagrid::closest_points( *lit0, *lit1 );
          if ( viennagrid::detail::is_equal_point(closest_points.first, closest_points.second, nc) )
          {
            ++intersection_count;
            line_refinement_tag_accessor(*lit0) = true;
            line_refinement_tag_accessor(*lit1) = true;

            VertexHandleType new_vertex = viennagrid::make_unique_vertex( output_mesh, closest_points.first );

            line_refinement_vertex_handle_accessor(*lit0) = new_vertex;
            line_refinement_vertex_handle_accessor(*lit1) = new_vertex;
            break;
          }
        }
      }

      viennagrid::simple_refine<CellTag>(input_mesh, output_mesh,
                                          vertex_map,
                                          line_refinement_tag_accessor,
                                          line_refinement_vertex_handle_accessor);

      return intersection_count;
    }

    NumericConfigT nc;
  };
}

#endif
