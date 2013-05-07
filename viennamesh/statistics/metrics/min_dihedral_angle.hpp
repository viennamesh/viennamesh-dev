#ifndef VIENNAMESH_STATISTICS_METRICS_MIN_DIHEDRAL_ANGLE_HPP
#define VIENNAMESH_STATISTICS_METRICS_MIN_DIHEDRAL_ANGLE_HPP

#include "viennagrid/algorithm/angle.hpp"

namespace viennamesh
{
    namespace metrics
    { 
        template<typename DomainType, typename ElementType, typename NumericLimitsType>
        typename viennagrid::result_of::coord_type<DomainType>::type min_dihedral_angle_impl(DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, viennagrid::tetrahedron_tag)
        {
            typedef typename viennagrid::result_of::coord_type<DomainType>::type            NumericType;
            typedef typename viennagrid::result_of::point_type<DomainType>::type            PointType;
            typedef typename viennagrid::result_of::const_vertex_range<ElementType>::type   VertexOnCellContainer;
            
            PointType const & p0 = viennagrid::point( domain, viennagrid::vertices(element)[0] );
            PointType const & p1 = viennagrid::point( domain, viennagrid::vertices(element)[1] );
            PointType const & p2 = viennagrid::point( domain, viennagrid::vertices(element)[2] );
            PointType const & p3 = viennagrid::point( domain, viennagrid::vertices(element)[3] );
            
            double da_01 = viennagrid::dihedral_angle( p0, p1, p2, p0, p1, p3 );
            double da_02 = viennagrid::dihedral_angle( p0, p2, p1, p0, p2, p3 );
            double da_03 = viennagrid::dihedral_angle( p0, p3, p1, p0, p3, p2 );
            double da_12 = viennagrid::dihedral_angle( p1, p2, p0, p1, p2, p3 );
            double da_13 = viennagrid::dihedral_angle( p1, p3, p0, p1, p3, p2 );
            double da_23 = viennagrid::dihedral_angle( p2, p3, p0, p2, p3, p1 );
            
            return std::min( std::min( std::min( da_01, da_02 ), std::min(da_03, da_12) ), std::min(da_13, da_23) );
        }
    }
    
    
    struct min_dihedral_angle_tag {};
    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type min_dihedral_angle( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits )
    {
        return metrics::min_dihedral_angle_impl(domain, element, numeric_limits, typename ElementType::tag());
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type min_dihedral_angle( DomainType const & domain, ElementType const & element )
    {
        return min_dihedral_angle(domain, element, std::numeric_limits< typename viennagrid::result_of::coord_type<DomainType>::type >() );
    }
    
    template<typename DomainType, typename ElementType, typename NumericLimitsType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element, NumericLimitsType numeric_limits, min_dihedral_angle_tag)
    {
        return min_dihedral_angle(domain, element, numeric_limits);       
    }
    
    template<typename DomainType, typename ElementType>
    typename viennagrid::result_of::coord_type<DomainType>::type metric( DomainType const & domain, ElementType const & element, min_dihedral_angle_tag)
    {
        return min_dihedral_angle(domain, element);       
    }
    
    
}


#endif