#ifndef VIENNAMESH_STATISTICS_METRICS_CONDITION_NUMBER_HPP
#define VIENNAMESH_STATISTICS_METRICS_CONDITION_NUMBER_HPP

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

#include "viennagrid/algorithm/volume.hpp"
#include "viennamesh/statistic/forwards.hpp"

namespace viennamesh
{
  namespace metrics
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type condition_number_impl(PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits, viennagrid::triangle_tag)
    {
      typedef typename PointAccessorT::value_type            PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type            NumericType;

      PointType const & p0 = point_accessor( viennagrid::vertices(element)[0] );
      PointType const & p1 = point_accessor( viennagrid::vertices(element)[1] );
      PointType const & p2 = point_accessor( viennagrid::vertices(element)[2] );

      NumericType area = viennagrid::volume(point_accessor, element);
      if (std::abs(area) < numeric_limits.epsilon()) return numeric_limits.max();

      PointType v1 = p1-p0;
      PointType v2 = p2-p0;

      return ( viennagrid::inner_prod(v1, v1) +
                viennagrid::inner_prod(v2, v2) -
                viennagrid::inner_prod(v1, v2) ) /
              ( 2 * area * std::sqrt(3) );
    }

    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT, typename TagT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type condition_number_impl(PointAccessorT const, ElementT const &, NumericLimitsT, TagT)
    {
      throw metric_not_implemented_or_supported_exception( "condition number not implemented for " + TagT::name() );
    }
  }


  struct condition_number_tag {};


  template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type condition_number( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits )
  {
    return metrics::condition_number_impl(point_accessor, element, numeric_limits, typename ElementT::tag());
  }

  template<typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type condition_number( PointAccessorT const point_accessor, ElementT const & element )
  {
    return condition_number(point_accessor, element, std::numeric_limits< typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type >() );
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord< ElementT >::type condition_number(ElementT const & element)
  {
    return condition_number( viennagrid::default_point_accessor(element), element);
  }


  namespace detail
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits, condition_number_tag)
    {
      return condition_number(point_accessor, element, numeric_limits);
    }

    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, condition_number_tag)
    {
      return condition_number(point_accessor, element);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element, condition_number_tag)
    {
      return condition_number(element);
    }
  }

}


#endif
