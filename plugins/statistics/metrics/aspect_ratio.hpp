#ifndef VIENNAMESH_STATISTICS_METRICS_ASPECT_RATIO_HPP
#define VIENNAMESH_STATISTICS_METRICS_ASPECT_RATIO_HPP

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
#include "viennagrid/algorithm/surface.hpp"
#include "forwards.hpp"

namespace viennamesh
{
  namespace metrics
  {

    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type aspect_ratio_impl(ElementT const & element, NumericLimitsT numeric_limits, viennagrid::triangle_tag)
    {
      typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
      PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
      PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );

      NumericType area = viennagrid::volume(element);
      if (std::abs(area) < numeric_limits.epsilon()) return numeric_limits.max(); // TODO relativ zum umfang hoch 2

      PointType tmp = p1-p0;
      NumericType result = viennagrid::inner_prod(tmp, tmp);

      tmp = p2-p1;
      result += viennagrid::inner_prod(tmp, tmp);

      tmp = p2-p0;
      result += viennagrid::inner_prod(tmp, tmp);

      return result / ( 4 * area * std::sqrt(3) );
    }


    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type aspect_ratio_impl(ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
    {
      typedef typename viennagrid::result_of::point<ElementT>::type  PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type NumericType;

      PointType const & p0 = viennagrid::get_point( viennagrid::vertices(element)[0] );
      PointType const & p1 = viennagrid::get_point( viennagrid::vertices(element)[1] );
      PointType const & p2 = viennagrid::get_point( viennagrid::vertices(element)[2] );
      PointType const & p3 = viennagrid::get_point( viennagrid::vertices(element)[3] );

      PointType l0 = p1-p0;
      PointType l2 = p0-p2;
      PointType l3 = p3-p0;

      NumericType volume = viennagrid::volume(element);
      NumericType area = viennagrid::surface(element);
      NumericType rad_inscribed = 3 * volume / area;

      NumericType rad_circum = viennagrid::norm_2(
          viennagrid::inner_prod(l3,l3) * viennagrid::cross_prod(l2,l0) +
          viennagrid::inner_prod(l2,l2) * viennagrid::cross_prod(l3,l0) +
          viennagrid::inner_prod(l0,l0) * viennagrid::cross_prod(l3,l2) )
              / (12 * volume);

      return rad_circum / (3 * rad_inscribed);
    }
  }


  struct aspect_ratio_tag {};

  namespace result_of
  {
    template<>
    struct metric_ordering_tag<aspect_ratio_tag>
    {
      typedef lower_is_better_tag type;
    };
  }

  template<typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<ElementT>::type aspect_ratio( ElementT const & element, NumericLimitsT numeric_limits )
  {

    if ( element.tag().is_triangle() )
      return metrics::aspect_ratio_impl(element, numeric_limits, viennagrid::triangle_tag());
    else if ( element.tag().is_tetrahedron() )
      return metrics::aspect_ratio_impl(element, numeric_limits, viennagrid::tetrahedron_tag());

    assert(false);
    return 0;
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord<ElementT>::type aspect_ratio( ElementT const & element )
  {
    return aspect_ratio(element, std::numeric_limits< typename viennagrid::result_of::coord<ElementT>::type >() );
  }

  namespace detail
  {
    template<typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<ElementT>::type metric( ElementT const & element, NumericLimitsT numeric_limits, aspect_ratio_tag)
    {
      return aspect_ratio(element, numeric_limits);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord<ElementT>::type metric(ElementT const & element, aspect_ratio_tag)
    {
      return aspect_ratio(element);
    }
  }

}


#endif
