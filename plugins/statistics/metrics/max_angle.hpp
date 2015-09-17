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

    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type max_angle_impl(PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT, viennagrid::triangle_tag)
    {
      typedef typename PointAccessorT::value_type            PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type            NumericType;

      PointType const & a = point_accessor( viennagrid::vertices(element)[0] );
      PointType const & b = point_accessor( viennagrid::vertices(element)[1] );
      PointType const & c = point_accessor( viennagrid::vertices(element)[2] );

      NumericType alpha = viennagrid::angle( b, c, a );
      NumericType beta = viennagrid::angle( a, c, b );
      NumericType gamma = M_PI - alpha - beta;

      return std::max(std::max( alpha, beta ), gamma);
    }


    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type max_angle_impl(PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
    {
      typedef typename PointAccessorT::value_type            PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type            NumericType;

      PointType const & a = point_accessor( viennagrid::vertices(element)[0] );
      PointType const & b = point_accessor( viennagrid::vertices(element)[1] );
      PointType const & c = point_accessor( viennagrid::vertices(element)[2] );
      PointType const & d = point_accessor( viennagrid::vertices(element)[3] );

      NumericType alpha = viennagrid::solid_angle( b, c, d, a );
      NumericType beta = viennagrid::solid_angle( a, c, d, b );
      NumericType gamma = viennagrid::solid_angle( a, b, d, c );
      NumericType delta = viennagrid::solid_angle( a, b, c, d );

      return std::max( std::max( alpha, beta ), std::max(gamma, delta) );
    }
  }


  struct max_angle_tag {};

  template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type max_angle( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits )
  {
    return metrics::max_angle_impl(point_accessor, element, numeric_limits, typename ElementT::tag());
  }

  template<typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type max_angle( PointAccessorT const point_accessor, ElementT const & element )
  {
    return max_angle(point_accessor, element, std::numeric_limits< typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type >() );
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord< ElementT >::type max_angle(ElementT const & element)
  {
    return max_angle( viennagrid::root_mesh_point_accessor(element), element);
  }


  namespace detail
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits, max_angle_tag)
    {
      return max_angle(point_accessor, element, numeric_limits);
    }

    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, max_angle_tag)
    {
      return max_angle(point_accessor, element);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element, max_angle_tag)
    {
      return max_angle(element);
    }
  }

}


#endif
