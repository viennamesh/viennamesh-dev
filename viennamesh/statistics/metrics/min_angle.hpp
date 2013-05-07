#ifndef VIENNAMESH_STATISTICS_METRICS_MIN_ANGLE_HPP
#define VIENNAMESH_STATISTICS_METRICS_MIN_ANGLE_HPP

#include "viennagrid/algorithm/angle.hpp"

namespace viennamesh
{
    namespace metrics
    {
        
        template<typename DomainType, typename ElementType, typename NumericLimitsType>
        typename viennagrid::result_of::coord_type<DomainType>::type min_angle_impl(DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, viennagrid::triangle_tag)
        {
            typedef typename viennagrid::result_of::coord_type<DomainType>::type            NumericType;
            typedef typename viennagrid::result_of::point_type<DomainType>::type            PointType;
            typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type   VertexOnCellContainer;
            
            PointType const & a = viennagrid::point( domain, viennagrid::vertices(element)[0] );
            PointType const & b = viennagrid::point( domain, viennagrid::vertices(element)[1] );
            PointType const & c = viennagrid::point( domain, viennagrid::vertices(element)[2] );
            
            NumericType alpha = viennagrid::angle( a, b, c );
            NumericType beta = viennagrid::angle( a, b, c );
            NumericType gamma = M_PI - alpha - beta;
            
            return std::min(std::min( alpha, beta ), gamma);
        }
        
        
        template<typename DomainType, typename ElementType, typename NumericLimitsType>
        typename viennagrid::result_of::coord_type<DomainType>::type min_angle_impl(DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, viennagrid::tetrahedron_tag)
        {
            typedef typename viennagrid::result_of::coord_type<DomainType>::type            NumericType;
            typedef typename viennagrid::result_of::point_type<DomainType>::type            PointType;
            typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type   VertexOnCellContainer;
            
            PointType const & a = viennagrid::point( domain, viennagrid::vertices(element)[0] );
            PointType const & b = viennagrid::point( domain, viennagrid::vertices(element)[1] );
            PointType const & c = viennagrid::point( domain, viennagrid::vertices(element)[2] );
            PointType const & d = viennagrid::point( domain, viennagrid::vertices(element)[3] );
            
            NumericType alpha = viennagrid::angle( a, b, c, d );
            NumericType beta = viennagrid::angle( b, a, c, d );
            NumericType gamma = viennagrid::angle( c, b, a, d );
            NumericType delta = viennagrid::angle( d, b, c, a );
            
            return std::min( std::min( alpha, beta ), std::min(gamma, delta) );
        }
    }
    
    
    struct min_angle_tag {};
    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type min_angle( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits )
    {
        return metrics::min_angle_impl(domain, element, numeric_limits, typename ElementType::tag());
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type min_angle( DomainType const & domain, ElementType const & element )
    {
        return min_angle(domain, element, std::numeric_limits< typename viennagrid::result_of::coord_type<DomainType>::type >() );
    }
    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, min_angle_tag)
    {
        return min_angle(domain, element, numeric_limits);       
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element, min_angle_tag)
    {
        return min_angle(domain, element);       
    }
    
    
}


#endif