#ifndef VIENNAMESH_ALGORITHM_CGAL_DELAUNEY_TETRAHEDRON_MESHER_HPP
#define VIENNAMESH_ALGORITHM_CGAL_DELAUNEY_TETRAHEDRON_MESHER_HPP

#include "viennamesh/domain/cgal_delaunay_tetrahedron.hpp"
#include "viennamesh/base/algorithm.hpp"
#include "viennamesh/base/settings.hpp"

#include "viennamesh/utils/utils.hpp"

namespace viennamesh
{
    struct cgal_delaunay_tetrahedron_tag {};
    
    
    
    
    struct cgal_delaunay_tetrahedron_settings
    {
        typedef FieldParameter<double, viennagrid::config::point_type_3d> field_parameter_type;
        typedef ScalarParameter<double> scalar_parameter_type;
        
        cgal_delaunay_tetrahedron_settings() : cell_size(0), facet_angle(0), cell_radius_edge_ratio(0) {}
        
        field_parameter_type cell_size;
        
        scalar_parameter_type facet_angle;
        scalar_parameter_type cell_radius_edge_ratio;

    };
    
    
    
    namespace result_of
    {
        template<>
        struct works_in_place<cgal_delaunay_tetrahedron_tag>
        {
            static const bool value = false;
        };
        
        template<>
        struct algorithm_info< cgal_delaunay_tetrahedron_tag >
        {
            static const std::string name() { return "CGAL Triangle Hull to Tetrahedron Mesh mesher"; }
        };
        
        
        template<typename domain_type>
        struct best_matching_native_input_domain<cgal_delaunay_tetrahedron_tag, domain_type>
        {
            typedef cgal_mesh_polyhedron_domain type;
        };

        template<typename domain_type>
        struct best_matching_native_output_domain<cgal_delaunay_tetrahedron_tag, domain_type>
        {
            typedef cgal_delauney_tetdrahedron_domain type;
        };
        
        template<typename segmentation_type>
        struct best_matching_native_input_segmentation<cgal_delaunay_tetrahedron_tag, segmentation_type>
        {
            typedef viennagrid::dummy_segmentation<> type;
        };

        template<typename segmentation_type>
        struct best_matching_native_output_segmentation<cgal_delaunay_tetrahedron_tag, segmentation_type>
        {
            typedef viennagrid::dummy_segmentation<> type;
        };
        
        template<>
        struct settings<cgal_delaunay_tetrahedron_tag>
        {
            typedef cgal_delaunay_tetrahedron_settings type;
        };
    }
    
    
    template<typename FT_, typename Point, typename Index_, typename field_functor_ptr_type>
    struct cgal_sizing_field
    {
    public:
        typedef FT_ FT;
        typedef Point Point_3;
        typedef Index_ Index;
        
        typedef typename utils::element_type<field_functor_ptr_type>::type field_functor_type;
        
        cgal_sizing_field( field_functor_ptr_type field_) : field(field_) {}
        
        FT operator() ( Point_3 const & p, int dimension, Index const & index ) const
        {
            assert(field != 0);
            return field->look_up( typename field_functor_type::point_type( p.x(), p.y(), p.z() ) );
        }
        
    private:
        field_functor_ptr_type field;
    };
    
    
    
    

    template<typename mesh_cirteria>
    struct get_triangulation;
    
    template<typename Tr>
    struct get_triangulation< CGAL::Mesh_criteria_3<Tr> >
    {
        typedef Tr type;
    };
    
    
    
    
    template<typename cgal_mesh_criteria_type, typename index_type, typename settings_type>
    cgal_mesh_criteria_type * get_cirteria( settings_type settings )
    {
        if ( settings.cell_size.is_ignored() )
        {
            return new cgal_mesh_criteria_type(
                CGAL::parameters::edge_size = std::numeric_limits<double>::max(),
                CGAL::parameters::cell_radius_edge_ratio = settings.cell_radius_edge_ratio(),
                CGAL::parameters::facet_angle = settings.facet_angle(),
                CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH_WITH_ADJACENCY_CHECK
            );
        }
        
        if ( settings.cell_size.is_scalar() )
        {
            return new cgal_mesh_criteria_type(
                CGAL::parameters::edge_size = settings.cell_size(),
                CGAL::parameters::cell_radius_edge_ratio = settings.cell_radius_edge_ratio(),
                CGAL::parameters::facet_angle = settings.facet_angle(),
                CGAL::parameters::facet_size = settings.cell_size(),
                CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH_WITH_ADJACENCY_CHECK,
                CGAL::parameters::cell_size = settings.cell_size()
            );
        }
        
        
        typedef typename settings_type::field_parameter_type::field_functor_ptr_type field_functor_ptr_type;
        
        typedef typename get_triangulation<cgal_mesh_criteria_type>::type::Triangulation_3 Triangulation;
        typedef typename cgal_mesh_criteria_type::Edge_criteria::FT FT;
        typedef typename Triangulation::Point Point;
        
        typedef typename cgal_mesh_criteria_type::Edge_criteria::Point_3 EdgePoint;
 
        typedef index_type Index;
        
        cgal_sizing_field<
            FT,
            Point,
            Index,
            field_functor_ptr_type
            > cell_and_facet_sizing_field( settings.cell_size.get_field() );
        
        cgal_sizing_field<
            FT,
            EdgePoint,
            Index,
            field_functor_ptr_type
            > edge_sizing_field( settings.cell_size.get_field() );
            
        return new cgal_mesh_criteria_type(
            CGAL::parameters::edge_size = edge_sizing_field,
            CGAL::parameters::cell_radius_edge_ratio = settings.cell_radius_edge_ratio(),
            CGAL::parameters::facet_angle = settings.facet_angle(),
            CGAL::parameters::facet_size = cell_and_facet_sizing_field,
            CGAL::parameters::facet_topology = CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH_WITH_ADJACENCY_CHECK,
            CGAL::parameters::cell_size = cell_and_facet_sizing_field
        );

    }
    
    
    
    
    
    template<>
    struct native_algorithm_impl<cgal_delaunay_tetrahedron_tag>
    {
        typedef cgal_delaunay_tetrahedron_tag algorithm_tag;
        
        template<typename native_input_domain_type, typename native_output_domain_type, typename settings_type>
        static algorithm_feedback run( native_input_domain_type const & native_input_domain, native_output_domain_type & native_output_domain, settings_type const & settings )
        {
            algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
            typename native_output_domain_type::Mesh_domain mesh_domain( native_input_domain.polyhedron );
            
            typename native_input_domain_type::feature_lines_type feature_lines(native_input_domain.feature_lines);
            mesh_domain.add_features( feature_lines.begin(), feature_lines.end() );
            
            
            typedef CGAL::Mesh_criteria_3<typename native_output_domain_type::Tr> mesh_criteria_type;
            mesh_criteria_type * criteria = get_cirteria<mesh_criteria_type, typename native_input_domain_type::Mesh_domain::Index>(settings);

            
            native_output_domain.tetdrahedron_triangulation = CGAL::make_mesh_3<typename native_output_domain_type::C3t3>(mesh_domain, *criteria);
            
            delete criteria;
            
            feedback.set_success();
            return feedback;
        }
        
    };
    
}

#endif