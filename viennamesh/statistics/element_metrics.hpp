#ifndef VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP
#define VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP


#include "viennamesh/statistics/metrics/aspect_ratio.hpp"
#include "viennamesh/statistics/metrics/condition_number.hpp"
#include "viennamesh/statistics/metrics/min_angle.hpp"
#include "viennamesh/statistics/metrics/min_dihedral_angle.hpp"


namespace viennamesh
{
    template<typename MetricTagT, typename PointAccessorT, typename ElementT, typename NumericLimitsT>
    typename viennagrid::result_of::coord<PointAccessorT>::type metric( PointAccessorT const point_accessor, ElementT const & element, NumericLimitsT numeric_limits)
    {
        return metric( point_accessor, element, numeric_limits, MetricTagT() );
    }

    
    template<typename MetricTagT, typename PointAccessorT, typename ElementT>
    typename viennagrid::result_of::coord<PointAccessorT>::type metric( PointAccessorT const point_accessor, ElementT const & element )
    {
        return metric( point_accessor, element, MetricTagT() );
    }

    template<typename MetricTagT, typename ElementT>
    typename viennagrid::result_of::coord< typename viennagrid::result_of::default_point_accessor<ElementT>::type >::type metric( ElementT const & element )
    {
        return metric(element, MetricTagT());
    }
    
}


#endif