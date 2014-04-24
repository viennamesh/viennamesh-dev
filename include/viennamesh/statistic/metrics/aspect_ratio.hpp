#ifndef VIENNAMESH_STATISTICS_METRICS_ASPECT_RATIO_HPP
#define VIENNAMESH_STATISTICS_METRICS_ASPECT_RATIO_HPP

#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"
#include "viennamesh/statistics/forwards.hpp"

namespace viennamesh
{
  namespace metrics
  {

    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type aspect_ratio_impl(PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits, viennagrid::triangle_tag)
    {
      typedef typename PointAccessorT::value_type            PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type            NumericType;

      typedef typename viennagrid::result_of::const_vertex_range<ElementT>::type     VertexOnCellContainer;

      PointType const & p0 = point_accessor( viennagrid::vertices(element)[0] );
      PointType const & p1 = point_accessor( viennagrid::vertices(element)[1] );
      PointType const & p2 = point_accessor( viennagrid::vertices(element)[2] );

      NumericType area = viennagrid::volume(point_accessor, element);
      if (std::abs(area) < numeric_limits.epsilon()) return numeric_limits.max(); // TODO relativ zum umfang hoch 2

      PointType tmp = p1-p0;
      NumericType result = viennagrid::inner_prod(tmp, tmp);

      tmp = p2-p1;
      result += viennagrid::inner_prod(tmp, tmp);

      tmp = p2-p0;
      result += viennagrid::inner_prod(tmp, tmp);

      return result / ( 4 * area * std::sqrt(3) );
    }


    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type aspect_ratio_impl(PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT, viennagrid::tetrahedron_tag)
    {
      typedef typename PointAccessorT::value_type            PointType;
      typedef typename viennagrid::result_of::coord<PointType>::type            NumericType;
      typedef typename viennagrid::result_of::const_vertex_range<ElementT>::type   VertexOnCellContainer;

      PointType const & p0 = point_accessor( viennagrid::vertices(element)[0] );
      PointType const & p1 = point_accessor( viennagrid::vertices(element)[1] );
      PointType const & p2 = point_accessor( viennagrid::vertices(element)[2] );
      PointType const & p3 = point_accessor( viennagrid::vertices(element)[3] );

      PointType l0 = p1-p0;
//       PointType l1 = p2-p1;
      PointType l2 = p0-p2;
      PointType l3 = p3-p0;
//       PointType l4 = p3-p1;
//       PointType l5 = p3-p2;

      NumericType volume = viennagrid::volume(point_accessor, element);
      NumericType area = viennagrid::surface(point_accessor, element);
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

  template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type aspect_ratio( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits )
  {
    return metrics::aspect_ratio_impl(point_accessor, element, numeric_limits, typename ElementT::tag());
  }

  template<typename PointAccessorT, typename ElementT>
  typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type aspect_ratio( PointAccessorT const point_accessor, ElementT const & element )
  {
    return aspect_ratio(point_accessor, element, std::numeric_limits< typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type >() );
  }

  template<typename ElementT>
  typename viennagrid::result_of::coord< ElementT >::type aspect_ratio(ElementT const & element)
  {
    return aspect_ratio( viennagrid::default_point_accessor(element), element);
  }

  namespace detail
  {
    template<typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits, aspect_ratio_tag)
    {
      return aspect_ratio(point_accessor, element, numeric_limits);
    }

    template<typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<typename PointAccessorT::value_type>::type metric( PointAccessorT const point_accessor, ElementT const & element, aspect_ratio_tag)
    {
      return aspect_ratio(point_accessor, element);
    }

    template<typename ElementT>
    typename viennagrid::result_of::coord< ElementT >::type metric( ElementT const & element, aspect_ratio_tag)
    {
      return aspect_ratio(element);
    }
  }

}


#endif
