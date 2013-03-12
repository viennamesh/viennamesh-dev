#ifndef VIENNAMESH_ALGORITHM_CGAL_PLC_3D_MESHER_HPP
#define VIENNAMESH_ALGORITHM_CGAL_PLC_3D_MESHER_HPP

#include "viennamesh/domain/cgal_plc_3d.hpp"
#include "viennamesh/base/algorithm.hpp"


namespace viennamesh
{
    struct cgal_plc_3d_mesher_tag {};
    
    
    
    
    struct cgal_plc_3d_settings
    {
        cgal_plc_3d_settings() : shortes_edge_circumradius_ratio(0.125), size_bound(0.0) {}
        cgal_plc_3d_settings(double shortes_edge_circumradius_ratio_, double size_bound_) :
            shortes_edge_circumradius_ratio(shortes_edge_circumradius_ratio_), size_bound(size_bound_) {}
        
        double shortes_edge_circumradius_ratio;
        double size_bound;
    };
    
    struct parameter_shortes_edge_circumradius_ratio {};
    struct parameter_size_bound {};
    
    
    
    template<>
    struct settings_parameter<cgal_plc_3d_settings, parameter_shortes_edge_circumradius_ratio>
    {
        typedef double type;
        
        static type get( cgal_plc_3d_settings const & settings) { return settings.shortes_edge_circumradius_ratio; }
    };
    
    template<>
    struct settings_parameter<cgal_plc_3d_settings, parameter_size_bound>
    {
        typedef double type;
        
        static type get( cgal_plc_3d_settings const & settings) { return settings.size_bound; }
    };
    
    
    
    
    
    
    
    
    namespace result_of
    {
        template<>
        struct works_in_place<cgal_plc_3d_mesher_tag>
        {
            static const bool value = true;
        };
        
        template<typename domain_type_or_tag>
        struct best_matching_native_input<cgal_plc_3d_mesher_tag, domain_type_or_tag>
        {
            typedef cgal_plc_3d_domain type;
        };

        template<typename domain_type_or_tag>
        struct best_matching_native_output<cgal_plc_3d_mesher_tag, domain_type_or_tag>
        {
            typedef cgal_plc_3d_domain type;
        };
        
        template<>
        struct settings<cgal_plc_3d_mesher_tag>
        {
            typedef cgal_plc_3d_settings type;
        };
    }
    
    
    template<>
    struct native_algorithm_impl<cgal_plc_3d_mesher_tag>
    {
        
        template<typename native_domain_type, typename settings_type>
        static bool run( native_domain_type & native_domain, settings_type const & settings )
        {
            typedef cgal_plc_3d_domain cgal_domain_type;
            
            for (cgal_domain_type::cell_container::iterator it = native_domain.cells.begin(); it != native_domain.cells.end(); ++it)
                CGAL::refine_Delaunay_mesh_2(it->cdt,
                                             it->cgal_list_of_holes.begin(),
                                             it->cgal_list_of_holes.end(),
                                             cgal_plc_3d_element::Criteria(
                                                get_settings_parameter<parameter_shortes_edge_circumradius_ratio>(settings),
                                                get_settings_parameter<parameter_size_bound>(settings)
                                            ));
            
            return true;
        }
        
    };
    
}

#endif