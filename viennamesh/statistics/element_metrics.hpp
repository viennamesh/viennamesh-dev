#ifndef VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP
#define VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP


#include "viennamesh/statistics/metrics/aspect_ratio.hpp"
#include "viennamesh/statistics/metrics/condition_number.hpp"
#include "viennamesh/statistics/metrics/min_angle.hpp"
#include "viennamesh/statistics/metrics/min_dihedral_angle.hpp"


namespace viennamesh
{
    template<typename metric_tag, typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits)
    {
        return metric( domain, element, numeric_limits, metric_tag() );
    }

    
    template<typename metric_tag, typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element )
    {
        return metric( domain, element, metric_tag() );
    }
    
}


#endif