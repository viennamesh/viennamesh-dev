#ifndef VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP
#define VIENNAMESH_STATISTICS_ELEMENT_METRICS_HPP

#include "viennagrid/algorithm/volume.hpp"
#include "viennagrid/algorithm/surface.hpp"

namespace viennamesh
{
    namespace metrics
    {
        
        template<typename DomainType, typename ElementType, typename NumericLimitsType>
        typename viennagrid::result_of::coord_type<DomainType>::type
        aspect_ratio_impl(DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, viennagrid::triangle_tag)
        {
            typedef typename viennagrid::result_of::coord_type<DomainType>::type            NumericType;
            typedef typename viennagrid::result_of::point_type<DomainType>::type            PointType;
            typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type   VertexOnCellContainer;
            
            PointType const & p0 = viennagrid::point( domain, viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::point( domain, viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::point( domain, viennagrid::vertices(element)[2] );
                        
            NumericType area = viennagrid::volume(domain, element);
            if (std::abs(area) < numeric_limits.epsilon()) return numeric_limits.max(); // TODO relativ zum umfang hoch 2
            
            PointType tmp = p1-p0;
            NumericType result = viennagrid::inner_prod(tmp, tmp);
            
            tmp = p2-p1;
            result += viennagrid::inner_prod(tmp, tmp);
            
            tmp = p2-p0;
            result += viennagrid::inner_prod(tmp, tmp);
            
            return result /
                   ( 4 * area * std::sqrt(3) );
        }
        
        
        template<typename DomainType, typename ElementType, typename NumericLimitsType>
        typename viennagrid::result_of::coord_type<DomainType>::type
        aspect_ratio_impl(DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, viennagrid::tetrahedron_tag)
        {
            typedef typename viennagrid::result_of::coord_type<DomainType>::type            NumericType;
            typedef typename viennagrid::result_of::point_type<DomainType>::type            PointType;
            typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type   VertexOnCellContainer;
            
            PointType const & p0 = viennagrid::point( domain, viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::point( domain, viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::point( domain, viennagrid::vertices(element)[2] );
            PointType const & p3 = viennagrid::point( domain, viennagrid::vertices(element)[3] );
            
            PointType l0 = p1-p0;
            PointType l1 = p2-p1;
            PointType l2 = p0-p2;
            PointType l3 = p3-p0;
            PointType l4 = p3-p1;
            PointType l5 = p3-p2;
            
            NumericType volume = viennagrid::volume(domain, element);
            NumericType area = viennagrid::surface(domain, element);
            NumericType rad_inscribed = 3 * volume / area;
            
            NumericType rad_circum = viennagrid::norm_2(
                viennagrid::inner_prod(l3,l3) * viennagrid::cross_prod(l2,l0) +
                viennagrid::inner_prod(l2,l2) * viennagrid::cross_prod(l3,l0) +
                viennagrid::inner_prod(l0,l0) * viennagrid::cross_prod(l3,l2) )
                    / (12 * volume);
                    
            return rad_circum / (3 * rad_inscribed);
        }
        
        
        
        
        
        
        
        
        template<typename DomainType, typename ElementType, typename NumericLimitsType>
        typename viennagrid::result_of::coord_type<DomainType>::type
        condition_number_impl(DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, viennagrid::triangle_tag)
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
        
    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    aspect_ratio( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits )
    {
        return metrics::aspect_ratio_impl(domain, element, numeric_limits, typename ElementType::tag());
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    aspect_ratio( DomainType const & domain, ElementType const & element )
    {
        return aspect_ratio(domain, element, std::numeric_limits< typename viennagrid::result_of::coord_type<DomainType>::type >() );
    }    

    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    condition_number( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits )
    {
        return metrics::condition_number_impl(domain, element, numeric_limits, typename ElementType::tag());
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    condition_number( DomainType const & domain, ElementType const & element )
    {
        return condition_number(domain, element, std::numeric_limits< typename viennagrid::result_of::coord_type<DomainType>::type >() );
    }   

    
    
    
    struct aspect_ratio_tag {};
    struct condition_number_tag {};

    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    metric( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, aspect_ratio_tag)
    {
        return aspect_ratio(element, numeric_limits);       
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    metric( DomainType const & domain, ElementType const & element, aspect_ratio_tag)
    {
        return aspect_ratio(element);       
    }

    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    metric( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, condition_number_tag)
    {
        return condition_number(element, numeric_limits);       
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type
    metric( DomainType const & domain, ElementType const & element, condition_number_tag)
    {
        return condition_number(element);       
    }
}


#endif