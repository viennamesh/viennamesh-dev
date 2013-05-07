#ifndef VIENNAMESH_STATISTICS_METRICS_CONDITION_NUMBER_HPP
#define VIENNAMESH_STATISTICS_METRICS_CONDITION_NUMBER_HPP

#include "viennagrid/algorithm/volume.hpp"

namespace viennamesh
{
    namespace metrics
    {
        template<typename DomainType, typename ElementType, typename NumericLimitsType>
        typename viennagrid::result_of::coord_type<DomainType>::type condition_number_impl(DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, viennagrid::triangle_tag)
        {
            typedef typename viennagrid::result_of::coord_type<DomainType>::type            NumericType;
            typedef typename viennagrid::result_of::point_type<DomainType>::type            PointType;
            typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type   VertexOnCellContainer;
            
            PointType const & p0 = viennagrid::point( domain, viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::point( domain, viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::point( domain, viennagrid::vertices(element)[2] );
            
            NumericType area = viennagrid::volume(domain, element);
            if (std::abs(area) < numeric_limits.epsilon()) return numeric_limits.max();
            
            PointType v1 = p1-p0;
            PointType v2 = p2-p0;
            
            return ( viennagrid::inner_prod(v1, v1) + 
                     viennagrid::inner_prod(v2, v2) -
                     viennagrid::inner_prod(v1, v2) ) /
                   ( 2 * area * std::sqrt(3) );
        }
    }
    
    
    struct condition_number_tag {};
    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type condition_number( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits )
    {
        return metrics::condition_number_impl(domain, element, numeric_limits, typename ElementType::tag());
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type condition_number( DomainType const & domain, ElementType const & element )
    {
        return condition_number(domain, element, std::numeric_limits< typename viennagrid::result_of::coord_type<DomainType>::type >() );
    }   
    
    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, condition_number_tag)
    {
        return condition_number(domain, element, numeric_limits);       
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element, condition_number_tag)
    {
        return condition_number(domain, element);       
    }
    
    
}


#endif