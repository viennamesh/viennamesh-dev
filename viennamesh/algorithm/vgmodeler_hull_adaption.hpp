#ifndef VIENNAMESH_ALGORITHM_VGMODELER_HULL_ADAPTION_HPP
#define VIENNAMESH_ALGORITHM_VGMODELER_HULL_ADAPTION_HPP

#include "viennamesh/base/algorithm.hpp"
#include "viennamesh/base/settings.hpp"
#include "viennamesh/base/segments.hpp"

#include "viennamesh/utils/utils.hpp"


#include "vgmodeler/vgmodeler.hpp"
#include "../../../ViennaGrid/viennagrid/algorithm/cross_prod.hpp"


namespace viennamesh
{
    struct vgmodeler_hull_adaption_tag {};
    
    struct vgmodeler_hull_adaption_settings
    {
        typedef FieldParameter<double, viennagrid::config::point_type_3d> field_parameter_type;
        typedef ScalarParameter<double> scalar_parameter_type;
        
        vgmodeler_hull_adaption_settings() : cell_size(0) {}
        
        scalar_parameter_type cell_size;
    };

    namespace result_of
    {
        template<>
        struct works_in_place<vgmodeler_hull_adaption_tag>
        {
            static const bool value = false;
        };
        
        template<typename domain_type>
        struct best_matching_native_input<vgmodeler_hull_adaption_tag, domain_type>
        {
            typedef viennagrid::config::triangular_3d_domain type;
        };

        template<typename domain_type>
        struct best_matching_native_output<vgmodeler_hull_adaption_tag, domain_type>
        {
            typedef viennagrid::config::triangular_3d_domain type;
        };
        
        template<>
        struct settings<vgmodeler_hull_adaption_tag>
        {
            typedef vgmodeler_hull_adaption_settings type;
        };
    }
    
    
    template<>
    struct native_algorithm_impl<vgmodeler_hull_adaption_tag>
    {
        
        template<typename native_input_domain_type, typename native_output_domain_type, typename settings_type>
        static bool run( native_input_domain_type const & native_input_domain, native_output_domain_type & native_output_domain, settings_type settings )
        {
            vgmodeler::hull_adaptor adaptor;
        
            if (!settings.cell_size.is_ignored())
                adaptor.maxsize() = settings.cell_size();
            
            adaptor.process( native_input_domain, native_output_domain );

            viennamesh::segment_seed_points(native_output_domain) = viennamesh::segment_seed_points(native_input_domain);
            
            return true;
        }
        
    };
    
}

#endif