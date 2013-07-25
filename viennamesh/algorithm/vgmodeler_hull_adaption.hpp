#ifndef VIENNAMESH_ALGORITHM_VGMODELER_HULL_ADAPTION_HPP
#define VIENNAMESH_ALGORITHM_VGMODELER_HULL_ADAPTION_HPP

#include "viennamesh/base/algorithm.hpp"
#include "viennamesh/base/settings.hpp"
#include "viennagrid/domain/segmentation.hpp"

#include "viennamesh/utils/utils.hpp"

#include "vgmodeler/vgmodeler.hpp"


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
        
        template<>
        struct algorithm_info<vgmodeler_hull_adaption_tag>
        {
            static const std::string name() { return "VGModeler/Netgen Triangle Hull Adaption"; }
        };
        
        template<typename domain_type>
        struct best_matching_native_input_domain<vgmodeler_hull_adaption_tag, domain_type>
        {
            typedef viennagrid::triangular_3d_domain type;
        };

        template<typename domain_type>
        struct best_matching_native_output_domain<vgmodeler_hull_adaption_tag, domain_type>
        {
            typedef viennagrid::triangular_3d_domain type;
        };
        
        template<typename segmentation_type>
        struct best_matching_native_input_segmentation<vgmodeler_hull_adaption_tag, segmentation_type>
        {
            typedef viennagrid::triangular_hull_3d_segmentation type;
        };

        template<typename segmentation_type>
        struct best_matching_native_output_segmentation<vgmodeler_hull_adaption_tag, segmentation_type>
        {
            typedef viennagrid::triangular_hull_3d_segmentation type;
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
        typedef vgmodeler_hull_adaption_tag algorithm_tag;
        
        template<typename native_input_domain_type, typename input_segmentation_type, typename native_output_domain_type, typename output_segmentation_type,typename settings_type>
        static algorithm_feedback run( native_input_domain_type const & native_input_domain, input_segmentation_type const & input_segmentation,
                         native_output_domain_type & native_output_domain, output_segmentation_type & output_segmentation,
                         settings_type settings )
        {
            algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
            vgmodeler::hull_adaptor adaptor;
        
            if (!settings.cell_size.is_ignored())
                adaptor.maxsize() = settings.cell_size();
            
            adaptor.process( native_input_domain, input_segmentation, native_output_domain, output_segmentation );
            
            feedback.set_success(); 
            return feedback;
        }
        
    };
    
}

#endif