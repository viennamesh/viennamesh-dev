#ifndef VIENNAMESH_ALGORITHM_CGAL_DELAUNEY_TETRAHEDRON_MESHER_HPP
#define VIENNAMESH_ALGORITHM_CGAL_DELAUNEY_TETRAHEDRON_MESHER_HPP

#include "viennamesh/domain/cgal_delaunay_tetrahedron.hpp"
#include "viennamesh/base/algorithm.hpp"
#include <boost/concept_check.hpp>


namespace viennamesh
{
    struct cgal_delaunay_tetrahedron_tag {};
    
    
    
    
    struct cgal_delaunay_tetrahedron_settings
    {
        cgal_delaunay_tetrahedron_settings() {}
        
        double cell_radius_edge_ratio;
        double cell_size;
    };
    

    struct parameter_cell_radius_edge_ratio {};
    struct parameter_cell_size {};
    
    template<>
    struct settings_parameter<cgal_delaunay_tetrahedron_settings, parameter_cell_radius_edge_ratio>
    {
        typedef double type;
        
        static type get( cgal_delaunay_tetrahedron_settings const & settings) { return settings.cell_radius_edge_ratio; }
    };
    
    template<>
    struct settings_parameter<cgal_delaunay_tetrahedron_settings, parameter_cell_size>
    {
        typedef double type;
        
        static type get( cgal_delaunay_tetrahedron_settings const & settings) { return settings.cell_size; }
    };
    
    
    
    
    
    namespace result_of
    {
        template<>
        struct works_in_place<cgal_delaunay_tetrahedron_tag>
        {
            static const bool value = false;
        };
        
        template<typename domain_type>
        struct best_matching_native_input<cgal_delaunay_tetrahedron_tag, domain_type>
        {
            typedef cgal_mesh_polyhedron_domain type;
        };

        template<typename domain_type>
        struct best_matching_native_output<cgal_delaunay_tetrahedron_tag, domain_type>
        {
            typedef cgal_delauney_tetdrahedron_domain type;
        };
        
        template<>
        struct settings<cgal_delaunay_tetrahedron_tag>
        {
            typedef cgal_delaunay_tetrahedron_settings type;
        };
    }
    
    
    template<typename FT_, typename Point, typename Index_>
    struct test_meshsize
    {
        typedef FT_ FT;
        typedef Point Point_3;
        typedef Index_ Index;
        
        template<typename point_type, typename index_type>
        double operator() ( point_type p, int dimension, index_type index ) const
        {
            return p.x();
        }
    };
    
    
    
    template<>
    struct native_algorithm_impl<cgal_delaunay_tetrahedron_tag>
    {
        
        template<typename native_input_domain_type, typename native_output_domain_type, typename settings_type>
        static bool run( native_input_domain_type const & native_input_domain, native_output_domain_type & native_output_domain, settings_type const & settings )
        {
            typedef CGAL::Mesh_criteria_3<typename native_output_domain_type::Tr> Mesh_criteria;
            
            typename native_input_domain_type::feature_lines_type feature_lines(native_input_domain.feature_lines);
            
//             test_meshsize test;
            
            Mesh_criteria criteria(
//                 CGAL::parameters::cell_radius_edge_ratio = get_settings_parameter<parameter_cell_radius_edge_ratio>(settings),
//                 CGAL::parameters::cell_size = get_settings_parameter<parameter_cell_size>(settings),
                CGAL::parameters::edge_size = 1.0,
                CGAL::parameters::facet_angle = 25,
//                 CGAL::parameters::facet_size = 0.05,
//                 CGAL::parameters::facet_distance = 0.0005,
                CGAL::parameters::cell_radius_edge_ratio = 3,
                CGAL::parameters::cell_size = 1.0
            );
            
            typename native_output_domain_type::Mesh_domain mesh_domain( native_input_domain.polyhedron );
            
//             mesh_domain.detect_features();
            mesh_domain.add_features( feature_lines.begin(), feature_lines.end() );
            
            
            native_output_domain.tetdrahedron_triangulation = CGAL::make_mesh_3<typename native_output_domain_type::C3t3>(mesh_domain, criteria);
            
            
//             Mesh_criteria new_criteria(
//                 CGAL::parameters::cell_radius_edge_ratio = get_settings_parameter<parameter_cell_radius_edge_ratio>(settings),
//                 CGAL::parameters::cell_size = get_settings_parameter<parameter_cell_size>(settings),
//                                        CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH
//             );
// 
//             
//             CGAL::refine_mesh_3( native_output_domain.tetdrahedron_triangulation, mesh_domain, new_criteria );
            
            return true;
        }
        
    };
    
}

#endif