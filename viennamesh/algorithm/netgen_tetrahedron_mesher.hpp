#ifndef VIENNAMESH_ALGORITHM_NETGEN_TETRAHEDRON_MESHER_HPP
#define VIENNAMESH_ALGORITHM_NETGEN_TETRAHEDRON_MESHER_HPP

#include "viennamesh/base/algorithm.hpp"
#include "viennamesh/base/settings.hpp"
#include "viennamesh/domain/netgen_tetrahedron.hpp"

#include "viennamesh/utils/utils.hpp"


namespace viennamesh
{
    struct netgen_tetrahedron_tag {};
    
    
    
    
    struct netgen_tetrahedron_settings
    {
        typedef FieldParameter<double, viennagrid::config::point_type_3d> field_parameter_type;
        typedef ScalarParameter<double> scalar_parameter_type;
        
        netgen_tetrahedron_settings() : cell_size(0) {}
        
        scalar_parameter_type cell_size;
    };
    
    
    
    namespace result_of
    {
        template<>
        struct works_in_place<netgen_tetrahedron_tag>
        {
            static const bool value = true;
        };
        
        template<>
        struct algorithm_info<netgen_tetrahedron_tag>
        {
            static const std::string name() { return "Netgen 5.0.0 Triangle Hull to Tetrahedron Mesher"; }
        };
        
        template<typename domain_type>
        struct best_matching_native_input_domain<netgen_tetrahedron_tag, domain_type>
        {
            typedef netgen_tetrahedron_domain type;
        };

        template<typename domain_type>
        struct best_matching_native_output_domain<netgen_tetrahedron_tag, domain_type>
        {
            typedef netgen_tetrahedron_domain type;
        };
        
        
        template<typename domain_type>
        struct best_matching_native_input_segmentation<netgen_tetrahedron_tag, domain_type>
        {
            typedef viennagrid::dummy_segmentation<> type;
        };

        template<typename domain_type>
        struct best_matching_native_output_segmentation<netgen_tetrahedron_tag, domain_type>
        {
            typedef viennagrid::dummy_segmentation<> type;
        };
        
        
        template<>
        struct settings<netgen_tetrahedron_tag>
        {
            typedef netgen_tetrahedron_settings type;
        };
    }
    
    
    template<>
    struct native_algorithm_impl<netgen_tetrahedron_tag>
    {
        typedef netgen_tetrahedron_tag algorithm_tag;
        
        template<typename native_domain_type, typename native_segmentation_type, typename settings_type>
        static algorithm_feedback run( native_domain_type & native_domain,
                         native_segmentation_type & native_segmentation,
                         settings_type const & settings )
        {
            algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
            nglib::Ng_Meshing_Parameters mesh_parameters;
            
            for (typename native_domain_type::netgen_mesh_container_type::const_iterator it = native_domain.meshes.begin(); it != native_domain.meshes.end(); ++it)            
                nglib::Ng_Result result = nglib::Ng_GenerateVolumeMesh(it->second, &mesh_parameters);
            
            feedback.set_success();
            return feedback;
        }
        
    };
    
}

#endif