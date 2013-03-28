#ifndef VIENNAMESH_ALGORITHM_CGAL_PLC_MESHER_HPP
#define VIENNAMESH_ALGORITHM_CGAL_PLC_MESHER_HPP

#include "viennamesh/domain/cgal_plc.hpp"
#include "viennamesh/base/algorithm.hpp"
#include "viennamesh/base/settings.hpp"


#include <CGAL/Triangulation_conformer_2.h>

namespace viennamesh
{
    struct cgal_plc_2d_mesher_tag {};
    struct cgal_plc_3d_mesher_tag {};
    
    
    
    struct cgal_plc_settings
    {
        typedef FieldParameter<double, viennagrid::config::point_type_2d> field_parameter_type;
        typedef ScalarParameter<double> scalar_parameter_type;
        
        cgal_plc_settings() : shortes_edge_circumradius_ratio(0), size_bound(0) {}
        cgal_plc_settings( double shortes_edge_circumradius_ratio_, double size_bound_ ) : shortes_edge_circumradius_ratio(0.125), size_bound(0)
        {
            shortes_edge_circumradius_ratio = shortes_edge_circumradius_ratio_;
            size_bound = size_bound_;
        }
        
        scalar_parameter_type shortes_edge_circumradius_ratio;
        scalar_parameter_type size_bound;

    };
    

    
    
    
    
    namespace result_of
    {
        template<>
        struct works_in_place<cgal_plc_2d_mesher_tag>
        {
            static const bool value = true;
        };
        
        template<>
        struct works_in_place<cgal_plc_3d_mesher_tag>
        {
            static const bool value = true;
        };
        
        
        
        template<typename domain_type_or_tag>
        struct best_matching_native_input<cgal_plc_2d_mesher_tag, domain_type_or_tag>
        {
            typedef cgal_plc_2d_domain type;
        };
        
        template<typename domain_type_or_tag>
        struct best_matching_native_input<cgal_plc_3d_mesher_tag, domain_type_or_tag>
        {
            typedef cgal_plc_3d_domain type;
        };
        
        

        template<typename domain_type_or_tag>
        struct best_matching_native_output<cgal_plc_2d_mesher_tag, domain_type_or_tag>
        {
            typedef cgal_plc_2d_domain type;
        };
        
        template<typename domain_type_or_tag>
        struct best_matching_native_output<cgal_plc_3d_mesher_tag, domain_type_or_tag>
        {
            typedef cgal_plc_3d_domain type;
        };
        
        
        
        template<>
        struct settings<cgal_plc_2d_mesher_tag>
        {
            typedef cgal_plc_settings type;
        };
        
        template<>
        struct settings<cgal_plc_3d_mesher_tag>
        {
            typedef cgal_plc_settings type;
        };
    }
    
    
    template<>
    struct native_algorithm_impl<cgal_plc_2d_mesher_tag>
    {
        
        template<typename native_domain_type, typename settings_type>
        static bool run( native_domain_type & native_domain, settings_type const & settings )
        {
            typedef cgal_plc_2d_domain cgal_domain_type;
            
            for (cgal_domain_type::cell_container::iterator it = native_domain.cells.begin(); it != native_domain.cells.end(); ++it)
            {
                cgal_plc_3d_element::Criteria crit(settings.shortes_edge_circumradius_ratio(), settings.size_bound());
                cgal_plc_3d_element::Mesher m(it->cdt,crit);
                m.set_seeds( it->cgal_list_of_holes.begin(), it->cgal_list_of_holes.end(), false );
                m.init();

//                 CGAL::refine_Delaunay_mesh_2(it->cdt,
//                                              it->cgal_list_of_holes.begin(),
//                                              it->cgal_list_of_holes.end(),
//                                              cgal_plc_2d_element::Criteria(
//                                                 settings.shortes_edge_circumradius_ratio(),
//                                                 settings.size_bound()
//                                             ));
            }
            
            return true;
        }
        
    };
    
    template<>
    struct native_algorithm_impl<cgal_plc_3d_mesher_tag>
    {
        
        template<typename native_domain_type, typename settings_type>
        static bool run( native_domain_type & native_domain, settings_type const & settings )
        {
            typedef cgal_plc_3d_domain cgal_domain_type;
            
            for (cgal_domain_type::cell_container::iterator it = native_domain.cells.begin(); it != native_domain.cells.end(); ++it)
            {
                cgal_plc_3d_element::Criteria crit(settings.shortes_edge_circumradius_ratio(), settings.size_bound());
                cgal_plc_3d_element::Mesher m(it->cdt,crit);
                m.set_seeds( it->cgal_list_of_holes.begin(), it->cgal_list_of_holes.end(), false );
                m.init();


                
//                 CGAL::refine_Delaunay_mesh_2(it->cdt,
//                                              it->cgal_list_of_holes.begin(),
//                                              it->cgal_list_of_holes.end(),
//                                              cgal_plc_3d_element::Criteria(
//                                                 settings.shortes_edge_circumradius_ratio(),
//                                                 settings.size_bound()
//                                             ));
            }
            
            return true;
        }
        
    };
    
}

#endif