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
#include "forwards.hpp"

namespace viennamesh
{
  namespace metrics
  {
    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type condition_number_impl(ElementT const & element, NumericLimitsT numeric_limits, viennagrid::triangle_tag)
    {
      typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
      PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
      PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );

      NumericType area = viennagrid::volume(element);
      if (std::abs(area) < numeric_limits.epsilon()) return numeric_limits.max();

      PointType v1 = p1-p0;
      PointType v2 = p2-p0;

      return ( viennagrid::inner_prod(v1, v1) +
                viennagrid::inner_prod(v2, v2) -
                viennagrid::inner_prod(v1, v2) ) /
              ( 2 * area * std::sqrt(3) );
    }

    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type condition_number_impl(ElementT const & element, NumericLimitsT numeric_limits, viennagrid::tetrahedron_tag)
    {
      typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
      PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
      PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );
      PointType const & p3 = viennagrid::get_point( viennagrid::vertices(element)[3] );

      NumericType volume = viennagrid::volume(element);
      if (std::abs(volume) < numeric_limits.epsilon()) return numeric_limits.max();

      PointType l0 = p1-p0;
      PointType l1 = p2-p1;
      PointType l2 = p2-p0;
      PointType l3 = p3-p0;
      PointType l4 = p3-p1;
      PointType l5 = p3-p2;

      PointType c1 = l0;
      PointType c2 = (-2*l2 - l0) / std::sqrt(3);
      PointType c3 = (3*l3 + l2 - l0) / std::sqrt(6);

      NumericType t1 = viennagrid::inner_prod(c1,c1) + viennagrid::inner_prod(c2,c2) + viennagrid::inner_prod(c3,c3);
      NumericType t2 = viennagrid::inner_prod( viennagrid::cross_prod(c1,c2), viennagrid::cross_prod(c1,c2) ) +
                       viennagrid::inner_prod( viennagrid::cross_prod(c2,c3), viennagrid::cross_prod(c2,c3) ) +
                       viennagrid::inner_prod( viennagrid::cross_prod(c1,c3), viennagrid::cross_prod(c1,c3) );

      NumericType det = std::abs( viennagrid::inner_prod( c1, viennagrid::cross_prod(c2, c3) ) );

      return std::sqrt(t1*t2) / (3*det);
    }

    template<typename ElementT, typename NumericLimitsT, typename TagT>
    typename viennagrid::result_of::coord<ElementT>::type condition_number_impl(NumericLimitsT, TagT)
    {
      throw metric_not_implemented_or_supported_exception( "condition number not implemented for " + TagT::name() );
    }
  }


  struct condition_number_tag {};


  template<typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<ElementT>::type condition_number(ElementT const & element, NumericLimitsT numeric_limits)
  {
    if (element.tag().is_triangle())
      return metrics::condition_number_impl(element, numeric_limits, viennagrid::triangle_tag());
    else if (element.tag().is_tetrahedron())
      return metrics::condition_number_impl(element, numeric_limits, viennagrid::tetrahedron_tag());

    assert(false);
    return 0;
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord<ElementT>::type condition_number(ElementT const & element)
  {
    return condition_number(element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
  }


  namespace detail
  {
    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, NumericLimitsT numeric_limits, condition_number_tag)
    {
      return condition_number(element, numeric_limits);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element, condition_number_tag)
    {
      return condition_number(element);
    }
  }

}


#endif
