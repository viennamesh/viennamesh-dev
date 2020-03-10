#ifndef VIENNAMESH_STATISTICS_METRICS_MAX_ANGLE_HPP
#define VIENNAMESH_STATISTICS_METRICS_MAX_ANGLE_HPP

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

#include "viennagrid/algorithm/angle.hpp"

#include "forwards.hpp"

namespace viennamesh
{
  namespace metrics
  {

    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type max_angle_impl(ElementT const & element, NumericLimitsT, viennagrid::triangle_tag)
    {
      typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      PointType const & a = viennagrid::get_point( viennagrid::vertices(element)[0] );
      PointType const & b = viennagrid::get_point( viennagrid::vertices(element)[1] );
      PointType const & c = viennagrid::get_point( viennagrid::vertices(element)[2] );

      NumericType alpha = viennagrid::angle( b, c, a );
      NumericType beta = viennagrid::angle( a, c, b );
      NumericType gamma = M_PI - alpha - beta;

      return std::max(std::max( alpha, beta ), gamma);
    }


    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type max_angle_impl(ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
    {
      typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

       PointType const & a = viennagrid::get_point( viennagrid::vertices(element)[0] );
      PointType const & b = viennagrid::get_point( viennagrid::vertices(element)[1] );
      PointType const & c = viennagrid::get_point( viennagrid::vertices(element)[2] );
      PointType const & d = viennagrid::get_point( viennagrid::vertices(element)[3] );

      NumericType alpha = viennagrid::solid_angle( b, c, d, a );
      NumericType beta = viennagrid::solid_angle( a, c, d, b );
      NumericType gamma = viennagrid::solid_angle( a, b, d, c );
      NumericType delta = viennagrid::solid_angle( a, b, c, d );

      //TODO Why do we need NumericLimits?

      return std::max( std::max( alpha, beta ), std::max(gamma, delta) );
    }
  }


  struct max_angle_tag {};

  namespace result_of
  {
    template<>
    struct metric_ordering_tag<max_angle_tag>
    {
      typedef lower_is_better_tag type;
    };
  }

  template<typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<ElementT>::type max_angle( ElementT const & element, NumericLimitsT numeric_limits )
  {

    if ( element.tag().is_triangle() )
      return metrics::max_angle_impl(element, numeric_limits, viennagrid::triangle_tag());
    else if ( element.tag().is_tetrahedron() )
      return metrics::max_angle_impl(element, numeric_limits, viennagrid::tetrahedron_tag());

    assert(false);
    return 0;
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord<ElementT>::type max_angle( ElementT const & element )
  {
    return max_angle(element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
  }

  namespace detail
  {
    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type metric( ElementT const & element, NumericLimitsT numeric_limits, max_angle_tag)
    {
      return max_angle(element, numeric_limits);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, max_angle_tag)
    {
      return max_angle(element);
    }
  }

}


#endif
