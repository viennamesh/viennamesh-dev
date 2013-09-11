#ifndef VIENNAMESH_ALGORITHM_TETGEN_TETRAHEDRON_MESHER_HPP
#define VIENNAMESH_ALGORITHM_TETGEN_TETRAHEDRON_MESHER_HPP

#include "viennamesh/base/algorithm.hpp"
#include "viennamesh/base/settings.hpp"
#include "viennamesh/domain/tetgen_tetrahedron.hpp"

#include "viennamesh/utils/utils.hpp"


namespace viennamesh
{
    struct tetgen_tetrahedron_tag {};
    
    
    
    
    struct tetgen_tetrahedron_settings
    {
        typedef FieldParameter<double, viennagrid::config::point_type_3d> field_parameter_type;
        typedef ScalarParameter<double> scalar_parameter_type;
        
        tetgen_tetrahedron_settings() : cell_size(0), cell_radius_edge_ratio(0) {}
        
        scalar_parameter_type cell_size;
        scalar_parameter_type cell_radius_edge_ratio;
    };
    
    
    
    namespace result_of
    {
        template<>
        struct works_in_place<tetgen_tetrahedron_tag>
        {
            static const bool value = false;
        };
        
        template<>
        struct algorithm_info<tetgen_tetrahedron_tag>
        {
            static const std::string name() { return "Tetgen 1.4.3 Triangle Hull to Tetrahedron Mesher"; }
        };
        
        template<typename domain_type>
        struct best_matching_native_input_domain<tetgen_tetrahedron_tag, domain_type>
        {
            typedef tetgen_tetrahedron_domain type;
        };

        template<typename domain_type>
        struct best_matching_native_output_domain<tetgen_tetrahedron_tag, domain_type>
        {
            typedef tetgen_tetrahedron_domain type;
        };
        
        
        template<typename domain_type>
        struct best_matching_native_input_segmentation<tetgen_tetrahedron_tag, domain_type>
        {
            typedef NoSegmentation type;
        };

        template<typename domain_type>
        struct best_matching_native_output_segmentation<tetgen_tetrahedron_tag, domain_type>
        {
            typedef NoSegmentation type;
        };
        
        
        template<>
        struct settings<tetgen_tetrahedron_tag>
        {
            typedef tetgen_tetrahedron_settings type;
        };
    }
    
    
    
    
    template<>
    struct native_algorithm_impl<tetgen_tetrahedron_tag>
    {
        typedef tetgen_tetrahedron_tag algorithm_tag;
        
        template<typename native_input_domain_type, typename native_output_domain_type, typename settings_type>
        static algorithm_feedback run( native_input_domain_type const & native_input_domain, native_output_domain_type & native_output_domain, settings_type const & settings )
        {
            algorithm_feedback feedback( result_of::algorithm_info<algorithm_tag>::name() );

            tetgenbehavior tetgen_settings;
            tetgen_settings.quiet = 1;
            tetgen_settings.plc = 1;
            
            if (!settings.cell_radius_edge_ratio.is_ignored())
            {
              tetgen_settings.quality = 1;
              tetgen_settings.minratio = settings.cell_radius_edge_ratio();
            }

            
            if (!settings.cell_size.is_ignored())
            {
              tetgen_settings.fixedvolume = 1;
              tetgen_settings.maxvolume = settings.cell_size();
            }
            
            
            tetgen_settings.useshelles = tetgen_settings.plc || tetgen_settings.refine || tetgen_settings.coarse || tetgen_settings.quality; // tetgen.cxx:3008
            tetgen_settings.goodratio = tetgen_settings.minratio; // tetgen.cxx:3009
            tetgen_settings.goodratio *= tetgen_settings.goodratio; // tetgen.cxx:3010
            
            // tetgen.cxx:3040
            if (tetgen_settings.fixedvolume || tetgen_settings.varvolume) {
              if (tetgen_settings.quality == 0) {
                tetgen_settings.quality = 1;
              }
            }
            
            tetgen_settings.goodangle = cos(tetgen_settings.minangle * tetgenmesh::PI / 180.0);   // tetgen.cxx:3046
            tetgen_settings.goodangle *= tetgen_settings.goodangle;                               // tetgen.cxx:3047
            
//             tetgen_settings.parse_commandline( "pq1.414a0.1" );
            
            
            tetrahedralize(&tetgen_settings, const_cast<tetgenio*>(&native_input_domain), &native_output_domain);
            
            feedback.set_success();
            return feedback;
        }
        
    };
    

    
}

#endif