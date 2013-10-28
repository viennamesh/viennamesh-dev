#ifndef VIENNAMESH_ALGORITHM_METIS_SEGMENTER_HPP
#define VIENNAMESH_ALGORITHM_METIS_SEGMENTER_HPP

#include "viennamesh/base/algorithm.hpp"
#include "viennamesh/base/settings.hpp"
#include "viennagrid/mesh/segmentation.hpp"

#include "viennamesh/utils/utils.hpp"

#include "vgmodeler/vgmodeler.hpp"


namespace viennamesh
{
    struct metis_segmenter_tag {};
    
    struct vgmodeler_hull_adaption_settings
    {
        typedef FieldParameter<double, viennagrid::config::point_type_3d> FieldParameterType;
        typedef ScalarParameter<double> ScalarParameterType;
        typedef ScalarParameter<int> ScalarIntParameterType;
        
        vgmodeler_hull_adaption_settings() : num_segments(0) {}
        
        ScalarIntParameterType num_segments;
    };

    namespace result_of
    {
        template<>
        struct works_in_place<metis_segmenter_tag>
        {
            static const bool value = false;
        };
        
        template<>
        struct algorithm_info<metis_segmenter_tag>
        {
            static const std::string name() { return "Metis mesh segmentation"; }
        };
        
        template<typename MeshT>
        struct best_matching_native_input_mesh<metis_segmenter_tag, MeshT>
        {
            typedef MeshT type;
        };

        template<typename MeshT>
        struct best_matching_native_output_mesh<vgmodeler_hull_adaption_tag, MeshT>
        {
            typedef MeshT type;
        };
        
        template<typename SegmentationT>
        struct best_matching_native_input_segmentation<vgmodeler_hull_adaption_tag, SegmentationT>
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
        
        template<typename native_input_mesh_type, typename input_segmentation_type, typename native_output_mesh_type, typename output_segmentation_type,typename settings_type>
        static algorithm_feedback run( native_input_mesh_type const & native_input_mesh, input_segmentation_type const & input_segmentation,
                         native_output_mesh_type & native_output_mesh, output_segmentation_type & output_segmentation,
                         settings_type settings )
        {
            algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );
            vgmodeler::hull_adaptor adaptor;
        
            if (!settings.cell_size.is_ignored())
                adaptor.maxsize() = settings.cell_size();
            
            adaptor.process( native_input_mesh, input_segmentation, native_output_mesh, output_segmentation );
            
            feedback.set_success(); 
            return feedback;
        }
        
    };
    
}

#endif