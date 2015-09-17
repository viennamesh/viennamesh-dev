#ifndef VIENNAMESH_STATISTICS_METRICS_RADIUS_EDGE_RATIO_HPP
#define VIENNAMESH_STATISTICS_METRICS_RADIUS_EDGE_RATIO_HPP

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

#include "viennagrid/algorithm/circumcenter.hpp"
#include "forwards.hpp"

namespace viennamesh
{
  namespace metrics
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type aspect_ratio_impl(PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT)
    {
      typedef typename PointAccessorT::point_type               PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type               NumericType;
      typedef typename viennagrid::result_of::const_element_range<ElementT>::type          LinesOnElementRangeType;
      typedef typename viennagrid::result_of::iterator<LinesOnElementRangeType>::type   LinesOnElementRangeIterator;

      PointType circum_center = viennagrid::circumcenter(point_accessor, element);
      NumericType circum_radius = viennagrid::norm_2( point_accessor.get(viennagrid::vertices(element)[0]) - circum_center );

      LinesOnElementRangeType lines(element, 1);

      NumericType min_length = 2 * circum_radius;
      std::vector<NumericType> edge_length( lines.size() );
//       unsigned int index = 0;
      for (LinesOnElementRangeIterator lit = lines.begin(); lit != lines.end(); ++lit)
      {
        PointType line = point_accessor.get( viennagrid::vertices(*lit)[0] ) -
                         point_accessor.get( viennagrid::vertices(*lit)[1] );

        NumericType length = viennagrid::norm_2( line );
        if (length < min_length)
          min_length = length;
      }

      return circum_radius / min_length;
    }
  }


  struct radius_edge_ratio_tag {};

  namespace result_of
  {
    template<>
    struct metric_ordering_tag<radius_edge_ratio_tag>
    {
      typedef lower_is_better_tag type;
    };
  }


  template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type radius_edge_ratio( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits )
  {
    return metrics::aspect_ratio_impl(point_accessor, element, numeric_limits);
  }

  template<typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type radius_edge_ratio( PointAccessorT const point_accessor, ElementT const & element )
  {
    return radius_edge_ratio(point_accessor, element, std::numeric_limits< typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type >() );
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord< ElementT >::type radius_edge_ratio(ElementT const & element)
  {
    return radius_edge_ratio( viennagrid::root_mesh_point_accessor(), element);
  }

  namespace detail
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits, radius_edge_ratio_tag)
    {
        return radius_edge_ratio(point_accessor, element, numeric_limits);
    }

    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<typename PointAccessorT::point_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, radius_edge_ratio_tag)
    {
        return radius_edge_ratio(point_accessor, element);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element, radius_edge_ratio_tag)
    {
        return radius_edge_ratio(element);
    }
  }


}


#endif
